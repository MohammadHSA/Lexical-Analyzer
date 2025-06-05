#ifndef DFA_H
#define DFA_H
#include <string>
#include <set>
#include <map>

using namespace std;

enum DFAstatus { START, FAIL, POTENTIAL, ACCEPT };

//---------------------------------------------------------------------------------
// class DFA
// (S, ?, ?, s0, F)
//---------------------------------------------------------------------------------
class DFA
{
public:
    DFA() {}
    DFA(set<char> A, set<int> I, set<int> F) : alpha(A), init_states(I), fin_states(F)
    {
        Reset();
    }

    void Reset()
    {
        status = START;
        current_state = *init_states.begin();
        accepted = false;
        lexeme.clear();
    }

    void AddTransition(int src, int dst, char sym)
    { 
        Dtran[src][sym] = dst; 
    }

    string Move(string token)
    {
        string temp = token;
        for (char c : token) {
            // Check if c is a valid transition
            if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
                current_state = Dtran[current_state][c];
                lexeme += c;
                temp.erase(temp.find(c), 1); // Corrected erasure

                // Check if current state is a final state
                if (fin_states.count(current_state) > 0) {
                    status = ACCEPT;
                    accepted = true;
                    accepted_lexeme = lexeme;
                }
                else {
                    status = POTENTIAL;
                }
            }
            else {
                status = FAIL;
                return temp; // Return the remaining unprocessed part
            }
        }
        return temp;
    }

    DFAstatus GetStatus() {
        return status; 
    }

    string GetAcceptedLexeme() {
        return accepted_lexeme; 
    }
    bool GetAccepted() {
        return accepted; 
    }

    void Print()
    {
        cout << "DFA Transitions:\n";
        for (const auto& dfa_row : Dtran)
        {
            cout << dfa_row.first << ":\t";
            for (const auto& transition : dfa_row.second)
                cout << transition.first << ": " << transition.second << " ";
            cout << endl;
        }
    }


    map< int, map<char, int> > Dtran;
    DFAstatus status;
    int current_state;
    bool accepted;
    string lexeme;
    string accepted_lexeme;

    set<char> alpha;      // set of input symbols in the alphabet
    char epsilon = '#';   // epsilon is represented by '_'
    set<int> init_states; // initial state of the DFA
    set<int> fin_states;  // final states of the DFA
    string name = "";
};

#endif