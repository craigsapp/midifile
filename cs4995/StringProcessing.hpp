#ifndef STRING_PROCESSING_HPP_
#define STRING_PROCESSING_HPP_

#include <string>
#include <sstream>
#include <vector>

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

#endif
