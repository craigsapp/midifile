#include <iostream>
#include <string>
#include <vector>
#include "StringProcessing.hpp"

using namespace std;

int main() {
    string input_1 = "A/C/E";
    string input_2 = "ACE";
    string input_3 = "A#CbF";

    cerr << "\nstarting...\n";
    vector<string> tokens_1 = tokenize_chordstr(input_1);
    cerr << "finished 1\n";
    vector<string> tokens_2 = tokenize_chordstr(input_2);
    cerr << "finished 2\n";
    vector<string> tokens_3 = tokenize_chordstr(input_3);
    cerr << "finished 3\n\n";

    cout << "tokens_1:\n";
    for (auto s : tokens_1) {
        cout << s << ", ";
    }
    cout << "\nshould be '" << "A/ C/ E" << "'\n\n";

    cout << "tokens_2:\n";
    for (auto s : tokens_2) {
        cout << s << ", ";
    }

    cout << "\nshould be '" << "A C E" << "'\n\n";

    cout << "tokens_3:\n";
    for (auto s : tokens_3) {
        cout << s << ", ";
    }

    cout << "\nshould be '" << "A# Cb F" << "'\n\n";


}
