#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <String>
using namespace std;

vector<pair<string, string>> divider( string& input) {
    vector<pair<string, string>> sections;
    stringstream ss(input);
    string segment;

    while (getline(ss, segment, ',')) {  // Split by ','
        stringstream ss2(segment);
        string first, second;

        if (ss2 >> first >> second) { // Extract two parts
            if (second.back() == '#') {
                second.pop_back(); // Remove '#' from the last section
            }
            sections.emplace_back(first, second);
        }
    }
    return sections;
}
vector<string> dividerTokens( string& input) {

    vector< string> sections;
    stringstream ss(input);
    string segment;

    while (getline(ss, segment, ' ')) {  // Split by ' '
        stringstream ss2(segment);
        string first;
        
        if (ss2 >> first) {     // Extract two parts      
            sections.push_back(first);
        }
    }
    return sections;
}

