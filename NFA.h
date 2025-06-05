
//---------------------------------------------------------------------------------
// Example code for an NFA class and an epsilon-closure function
//
// g++ -std=c++11 recitation1.cpp
//
//---------------------------------------------------------------------------------
#include <iostream>
#include <set>
#include <map>
#include <string>


using namespace std;

//---------------------------------------------------------------------------------
// class NFA
// (S, Σ, δ, s0, F)
//---------------------------------------------------------------------------------
class NFA 
{
public:
    NFA(set<char> A, int I, set<int> F) 
    {
        alphabet  = A;
        init_state= I;
        fin_states= F;
    }
    void AddTransition( int src, set<int> dst, char sym)
    {
        Ntran[src][sym] = dst;
    }
    set<int> EpsilonClosure(const set<int>& states); 
    void Print();

    map< int, map<char, set<int>> > Ntran;

    set<char> alphabet;   // set of input symbols in the alphabet
    int init_state;       // initial state of the NFA
    set<int> fin_states;  // final states of the NFA
    string name = "";

    
};



//---------------------------------------------------------------------------------
// print the NFA 
//---------------------------------------------------------------------------------
void NFA::Print() 
{
    cout << "NFA Transitions:\n";
    for (const auto& nfa_row : Ntran) 
    {
        cout << nfa_row.first << ":\t";

        for (const auto& transition : nfa_row.second) 
        {
            cout << transition.first << ": { ";
            for (int state : transition.second) 
                cout << state << " ";
            cout << "} ";
        }
        cout << endl;
    }
}

//---------------------------------------------------------------------------------
// epsilon closure of a set of states in the NFA
//
// a dfs traversal of epsilon transitions
//
//---------------------------------------------------------------------------------
set<int> NFA::EpsilonClosure(const set<int>& states) 
{
    stack<int> dfs_stack;
    set<int> epsilon_closure_set = states;

    for (int state : states)
        dfs_stack.push(state);

    while (!dfs_stack.empty()) 
    {
        int state = dfs_stack.top(); dfs_stack.pop();

        // Check all epsilon transitions from this state
        if (Ntran[state].find('#') != Ntran[state].end())
            for (int next_state : Ntran[state]['#'])
                if (epsilon_closure_set.find(next_state) == epsilon_closure_set.end()) 
                {
                    epsilon_closure_set.insert(next_state);
                    dfs_stack.push(next_state);
                }
    }

    return epsilon_closure_set;
}




