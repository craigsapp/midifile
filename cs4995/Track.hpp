#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include <sstream>
#include "Note.hpp"

namespace smf {

using std::string;
using std::vector;

uint8_t DEFAULT_VELOCITY = 64;

vector<string> tokenize(string str, char delimiter);

class Track {
private:
    vector<Note> notes;
    uint8_t velocity;

public:
    Track(uint8_t velocity = DEFAULT_VELOCITY) : velocity(velocity) {}

    const vector<Note>& getNotes() {
        return notes;
    }

    const Note& operator[](int index) const {
        return notes[index];
    }

    Note& operator[](int index) {
        return notes[index];
    }

    uint8_t getVelocity() {
        return velocity;
    }

    friend void operator<<(Track &trk, Note n);
    friend void operator<<(Track &trk, const vector<Note> &n);
    friend void operator<<(Track &trk, string s);
};

void operator<<(Track &trk, Note n) {
    trk.notes.push_back(n);
}

void operator<<(Track &trk, const vector<Note> &nv) {
    trk.notes.reserve(trk.notes.size() + nv.size());
    for (Note n : nv) {
        trk.notes.push_back(n);
    }
}

void operator<<(Track &trk, string s) {
    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return;
    }

    // reserve space >= the amount needed
    trk.notes.reserve(trk.notes.size() + tokens.size());

    auto it = tokens.begin();
    while (it < tokens.end()) {
        Note n; // default: quarter rest
        if (it->compare(REST) != 0) {
            n = Note{toPitch(*it)};
        }
        while(++it < tokens.end() && it->compare(EXTEND) == 0) {
            n.incrementLength();
        }
        trk << n;
    }
    return;
}

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

} // namespace smf

#endif
