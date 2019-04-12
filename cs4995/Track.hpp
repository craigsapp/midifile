#ifndef TRACK_HPP_
#define TRACK_HPP_

#include <string>
#include "Key.hpp"
#include "Note.hpp"
#include "StringProcessing.hpp"

namespace smf {

using std::string;
using std::vector;

int DEFAULT_VELOCITY = 64;

/* unimplemented
void transformLength(
    vector<Note> &notes, const map<float, float> &transformation);
    */

class Track {
private:
    vector<Note> notes;
    int velocity;

    void transformPitch(const map<int, int> &deltas) {
        for (Note &n : notes) {
            n.transformPitch(deltas);
        }
    }

public:
    Track(int velocity = DEFAULT_VELOCITY) : velocity(velocity) {}

    const vector<Note>& getNotes() {
        return notes;
    }

    const Note& operator[](int index) const {
        return notes[index];
    }

    Note& operator[](int index) {
        return notes[index];
    }

    int getVelocity() {
        return velocity;
    }

    void transpose(int delta) {
        for (Note &n : notes) {
            n.transformPitch(delta);
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        transformPitch(src.getDifferences(dest));
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
            n = Note{Pitch{*it}};
        }
        while(++it < tokens.end() && it->compare(EXTEND) == 0) {
            n.incrementLength();
        }
        trk << n;
    }
    return;
}

} // namespace smf

#endif
