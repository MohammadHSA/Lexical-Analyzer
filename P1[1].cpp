/*----------------------------------------------------------------------
A basic lexer with a getToken function which assumes the input
string consists of identifiers, numbers, and operators.

g++ -std=c++11 DFA.cpp simple_lexerDFA.cpp
./a.out < charstream.txt > output.txt
more output.txt
------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <utility>
#include "DFA.h"
#include "NFA.h"
#include "divider.h"
using namespace std;

//-----------
//Global language across the assignment
//-----------

set<char> language = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
        'p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I'
        ,'J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2'
        ,'3','4','5','6','7','8','9',' ','\t','\n' };

//--------------------------------------------------------------
// shunting code
//--------------------------------------------------------------
int Precedence(char op) {
    if (op == '*') return 3;  // Kleene star (*) highest precedence
    if (op == '.') return 2;  // Concatenation
    if (op == '|') return 1;  // Alternation
    return 0;
}

// Recognize alphanumeric operands and whitespace for the assignment language
bool IsOperand(char c) {
    return isalnum(c) || c == ' ' || c == '\t' || c == '\n';
}

// Convert infix regex to postfix for Thompson's construction
string InfixToPostfix(const string& infix)
{
    stack<char> ops;        // stack for operators
    string postfix = "";    // resulting postfix expression 'queue'

    for (size_t i = 0; i < infix.size(); i++)
    {
        char c = infix[i];

        if (IsOperand(c))
        {
            // if c is an operand, put it on the output queue
            postfix += c;
        }
        else if (c == '(')
        {
            // if it's an opening parenthesis, push it onto the stack
            ops.push(c);
        }
        else if (c == ')')
        {
            // pop off the stack until matching '(' is found
            while (!ops.empty() && ops.top() != '(')
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();  // pop the '('
        }
        else if (c == '|')
        {
            // if it's an alternation operator, pop operators of higher precedence
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c))
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);  // push the alternation operator onto the stack
        }
        else if (c == '.')
        {
            // do same for concatenation (.)
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c))
            {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);  // push the . operator onto the stack
        }
        else if (c == '*')
        {
            // and for (*)
            ops.push(c);  // push the * operator onto the stack
        }
    }

    // pop all remaining operators off the stack
    while (!ops.empty())
    {
        postfix += ops.top();
        ops.pop();
    }

    return postfix;
}

//--------------------------------------------------------------
// regex to NFA
//--------------------------------------------------------------
// Thompson’s Construction: Convert postfix regex to NFA
NFA postfixToNFA(const string& postfix) {
    stack<NFA> nfaStack;
    int counter = 0;

    for (char c : postfix) {
        if (isalnum(c)) {  // If operand (character)
            NFA base(language, counter, { counter + 1 });

            base.AddTransition(base.init_state, base.fin_states, c);
            nfaStack.push(base);

            //cout << "[DEBUG] Created NFA for character '" << c << "' with states " << base.init_state << " -> " << base.init_state + 1 << endl;
            counter += 2;
        }
        else if (c == '|') {  // Union 
            NFA nfa2 = nfaStack.top(); nfaStack.pop();

            NFA nfa1 = nfaStack.top(); nfaStack.pop();

            NFA base(language, counter, { counter + 1 });

            base.AddTransition(base.init_state, { nfa1.init_state,nfa2.init_state }, '#');

            base.Ntran.insert(nfa1.Ntran.begin(), nfa1.Ntran.end());
            base.Ntran.insert(nfa2.Ntran.begin(), nfa2.Ntran.end());

            base.AddTransition(*nfa1.fin_states.begin(), base.fin_states, '#');
            base.AddTransition(*nfa2.fin_states.begin(), base.fin_states, '#');


            nfaStack.push(base);

            //cout << "[DEBUG] Created UNION NFA with new states " << *nfa1.fin_states.begin() << " and " << *nfa2.fin_states.begin() << endl;
            counter += 2;
        }
        else if (c == '.') {  // Concatenation
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();

            NFA base(language, counter, { counter + 1 });
            base.Ntran.insert(nfa1.Ntran.begin(), nfa1.Ntran.end());
            base.Ntran.insert(nfa2.Ntran.begin(), nfa2.Ntran.end());

            base.AddTransition(base.init_state, { nfa1.init_state }, '#');
            base.AddTransition(*nfa1.fin_states.begin(), { nfa2.init_state }, '#');
            base.AddTransition(*nfa2.fin_states.begin(), base.fin_states, '#');


            //cout << "[DEBUG] Created CONCATENATION NFA, linking " << *nfa1.fin_states.begin() << " -> " << nfa2.init_state << endl;
            counter += 2;
            nfaStack.push(base);
        }
        else if (c == '*') {  // Kleene Star
            NFA nfa1 = nfaStack.top(); nfaStack.pop();

            NFA base(language, counter, { counter + 1 });

            base.AddTransition(base.init_state, { *base.fin_states.begin(), nfa1.init_state }, '#');
            nfa1.AddTransition(*nfa1.fin_states.begin(), { nfa1.init_state, *base.fin_states.begin() }, '#');

            base.Ntran.insert(nfa1.Ntran.begin(), nfa1.Ntran.end());

            counter += 2;
            nfaStack.push({ base });

            //cout << "[DEBUG] Created KLEENE STAR NFA with states " << nfa1.init_state << " and " << *base.fin_states.begin() << endl;

        }
    }

    return nfaStack.top();
}

//--------------------------------------------------------------
// NFA to DFA.      The star of this abomination.
//--------------------------------------------------------------
// Function to convert NFA to DFA using Subset Construction
DFA NFAtoDFA(NFA nfa) {
    DFA dfa;
    dfa.name = nfa.name;
    map<set<int>, int> state_map;  // Maps NFA state sets to DFA states
    queue<set<int>> state_queue;   // Queue for unprocessed states

    set<int> start_closure = nfa.EpsilonClosure({ nfa.init_state });
    state_map[start_closure] = 0;
    dfa.init_states = { 0 };
    state_queue.push(start_closure);

    int state_count = 1;
    while (!state_queue.empty()) {
        set<int> current_set = state_queue.front(); state_queue.pop();
        int current_state = state_map[current_set];

        for (char sym : nfa.alphabet) {
            if (sym == '#') continue;  // Skip epsilon transitions
            set<int> next_set;

            for (int st : current_set) {
                if (nfa.Ntran.find(st) != nfa.Ntran.end() && nfa.Ntran.at(st).find(sym) != nfa.Ntran.at(st).end()) {
                    next_set.insert(nfa.Ntran.at(st).at(sym).begin(), nfa.Ntran.at(st).at(sym).end());
                }
            }

            if (!next_set.empty()) {
                next_set = nfa.EpsilonClosure(next_set);
                if (state_map.find(next_set) == state_map.end()) {
                    state_map[next_set] = state_count++;
                    state_queue.push(next_set);
                }
                dfa.AddTransition(current_state, state_map[next_set], sym);
            }
        }

        for (int st : current_set) {
            if (nfa.fin_states.find(st) != nfa.fin_states.end()) {
                dfa.fin_states.insert(current_state);
                break;
            }
        }
    }

    dfa.alpha = nfa.alphabet;
    return dfa;
}


//--------------------------------------------------------------
// main
//--------------------------------------------------------------


int main() {
    string grammarInput;
    cout << "Enter line: ";
    getline(cin, grammarInput);
    vector<pair<string, string>> grammarRules = divider(grammarInput); // Parse terminals

    cout << "Enter tokens: ";
    getline(cin, grammarInput);
    grammarInput.erase(0, 1);
    grammarInput.erase(grammarInput.size() - 1, grammarInput.size());
    vector<string> tokenList = dividerTokens(grammarInput); // Parse tokens

    vector<DFA> dfaList;   // Store DFA terminals
    vector<string> parsedTokens; // Store recognized lexemes

    // Convert each terminal into a DFA
    for (const auto& rule : grammarRules) {
        string postfixExpr = InfixToPostfix(rule.second);
        NFA generatedNFA = postfixToNFA(postfixExpr);
        generatedNFA.name = rule.first;
        dfaList.push_back(NFAtoDFA(generatedNFA));
    }

    // Process each token
    for (auto token : tokenList) {
        vector<string> remainingStrings;
        vector<string> matchedRules;

        for (auto& dfa : dfaList) { // Run each DFA on the token
            string remainingPart = dfa.Move(token);
            remainingStrings.push_back(remainingPart);
        }

        // Find the best match
        int bestMatchIndex = -1;
        string smallestRemaining = token;
        for (int i = 0; i < remainingStrings.size(); i++) {
            if (dfaList[i].GetStatus() == ACCEPT && remainingStrings[i] == "") {
                parsedTokens.push_back(dfaList[i].name + ", \"" + token + "\"");
                bestMatchIndex = -1;
                break;
            }
            if (remainingStrings[i].length() < smallestRemaining.length()) {
                smallestRemaining = remainingStrings[i];
                bestMatchIndex = i;
            }
        }

        if (bestMatchIndex != -1) {
            string bestMatchName = dfaList[bestMatchIndex].name;
            matchedRules.push_back(bestMatchName + ", \"" + token.substr(0, token.length() - smallestRemaining.length()) + "\"");

            // Process remaining part of token
            while (!smallestRemaining.empty()) {
                vector<string> newRemainingStrings;
                string nextMatch = "";

                for (auto& dfa : dfaList) {
                    newRemainingStrings.push_back(dfa.Move(smallestRemaining));
                }

                int nextIndex = -1;
                string nextSmallest = smallestRemaining;
                for (int i = 0; i < newRemainingStrings.size(); i++) {
                    if (newRemainingStrings[i].length() < nextSmallest.length()) {
                        nextSmallest = newRemainingStrings[i];
                        nextIndex = i;
                    }
                }

                if (nextIndex != -1) {
                    nextMatch = dfaList[nextIndex].name;
                    matchedRules.push_back(nextMatch + ", \"" + smallestRemaining.substr(0, smallestRemaining.length() - nextSmallest.length()) + "\"");
                    smallestRemaining = nextSmallest;
                }
                else {
                    break;
                }
            }
            for (const auto& match : matchedRules) {
                parsedTokens.push_back(match);
            }
        }

        for (auto& dfa : dfaList) { // Reset DFA states
            dfa.Reset();
        }
    }

    // Print output
    for (auto entry : parsedTokens) {
        cout << entry << endl;
    }

    return 0;
}



