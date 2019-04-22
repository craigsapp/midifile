#ifndef STRING_PROCESSING_HPP_
#define STRING_PROCESSING_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using std::vector;
using std::string;

vector<string> tokenize(string str, char delimiter) {
    vector<string> tokens;
    std::stringstream stream(str);
    string intermediate;
    while(getline(stream, intermediate, delimiter)) {
        if (intermediate.length() > 0) {
            tokens.push_back(intermediate);
        }
    }
    return tokens;
}

// returns true if c is one of
// {'A','B','C','D','E','F','G'}
bool is_note_char(char& c){
    return (c >= 'A' && c <= 'G');
}

// assumes valid input
// creates new token every time it encounters one of
// {'A','B','C','D','E','F','G'}
vector<string> tokenize_chordstr(string str) {
    vector<string> tokens;
    unsigned int start, end;
    start = end = 0;

    while (end < str.size()){
        while ( !(is_note_char(str[end])) && end < str.size() ) {
            end++;
        }
        if (start != end){
            tokens.push_back(str.substr(start, end-(start)));
        }
        start = end;
        end++;
    }

    tokens.push_back(str.substr(start));
    return tokens;
}
#endif
