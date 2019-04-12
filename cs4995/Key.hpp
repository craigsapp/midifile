#ifndef KEY_HPP_
#define KEY_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

vector<int> MAJOR = { 0, 2, 4, 5, 7, 9, 11 };
vector<int> MINOR = { 0, 2, 3, 5, 7, 8, 11 };
vector<int> HARMONIC_MINOR = MINOR;
vector<int> NATURAL_MINOR = { 0, 2, 3, 5, 7, 8, 10 };

template <typename KeyType, typename ValueType>
map<KeyType, ValueType> generateMap(
    vector<KeyType> keys, vector<ValueType> values)
{
    if (keys.size() != values.size()) {
        std::cerr << "Different number of keys and values in generateMap().\n";
        exit(1);
    }
    map<KeyType, ValueType> m;
    for (int i = 0; i < keys.size(); i++) {
        if (values[i] != 0) {
            m[keys[i]] = values[i];
        }
    }
    return m;
}

class Scale {
private:
    vector<int> scale_degrees;

    // each scale degree should be higher than the next.
    void makeAscending() {
        auto prev = scale_degrees.begin();
        if (prev == scale_degrees.end()) {
            return;
        }

        auto it = prev + 1;
        while(it != scale_degrees.end()) {
            while (*it < *prev) {
                *it += OCTAVE_WIDTH;
            }
            it++;
        }
    }

public:
    Scale(const vector<Pitch> &pitches) {
        for (Pitch p : pitches) {
            scale_degrees.push_back(p.toInt() % OCTAVE_WIDTH);
        }
        makeAscending();
    }

    Scale(const string &input) {
        vector<string> tokens = tokenize(input, ' ');
        for (string tok : tokens) {
            BasePitch base = charPitchMap[tok[0]];
            if (tok.length() == 2) {
                scale_degrees.push_back(base + charAccidentalMap[tok[1]]);
            } else {
                scale_degrees.push_back(base);
            }
        }
        makeAscending();
    }

    Scale(int key, const vector<int> &intervals) {
        for (int i : intervals) {
            scale_degrees.push_back(key + i);
        }
        makeAscending();
    }

    map<int, int> getDifferences(const Scale &s) const {
        if (scale_degrees.size() != s.scale_degrees.size()) {
            std::cerr <<
                "Different number of notes in scales in getDifference.\n";
            exit(1);
        }
        vector<int> diffs;
        for (int i = 0; i < scale_degrees.size(); i++) {
            diffs.push_back((int) s.scale_degrees[i] - (int) scale_degrees[i]);
        }
        // need to round down scale degrees to their common denomicator to be
        // used for transformations
        vector<int> base_degrees;
        for (int deg : scale_degrees) {
            base_degrees.push_back(deg % OCTAVE_WIDTH);
        }
        return generateMap(base_degrees, diffs);
    }
};

} // namespace smf

#endif
