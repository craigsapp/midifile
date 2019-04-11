#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include "MidiFile.h"

namespace smf {

using std::map;
using std::string;
using std::vector;

int DEFAULT_OCTAVE = 5;
int DEFAULT_VELOCITY = 64;
int NOTE_ON = 0x90;
int NOTE_OFF = 0x80;

string EXTEND = "-";
string REST = ".";

enum BasePitch {
    C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
};

map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

map<char, int> charAccidentalMap { {'#', 1}, {'b', -1} };

bool isBasePitch(char c) {
    return charPitchMap.find(c) != charPitchMap.end();
}

BasePitch toBasePitch(char c) {
    return charPitchMap[c];
}

bool isAccidental(char c) {
    return charAccidentalMap.find(c) != charAccidentalMap.end();
}

int toAccidental(char c) {
    return charAccidentalMap[c];
}

bool isOctave(char c) {
    return '0' <= c && c <= '9';
}

unsigned int toOctave(char c) {
    return (int) (c - '0');
}

class Pitch {
private:
    BasePitch base;
    int accidental; // sharp = 1, flat = -1
    unsigned int octave;

public:
    Pitch(BasePitch base = C, int accidental = 0,
          unsigned int octave = DEFAULT_OCTAVE) :
        base(base), accidental(accidental), octave(octave) {}

    BasePitch getBasePitch() {
        return base;
    }

    int toInt() {
        return base + accidental + 12 * octave;
    }

    friend std::ostream& operator<<(std::ostream &os, const Pitch &p);
};

std::ostream& operator<<(std::ostream &os, const Pitch &p) {
    os << p.base <<
        (p.accidental == 1 ? "#" : p.accidental == -1 ? "b" : "") << p.octave;
    return os;
}

class Note {
private:
    Pitch pitch;
    float length; // quarter note == 1.0
    bool is_rest;

public:
    Note(Pitch pitch, float length = 1.0) :
        pitch(pitch), length(length), is_rest(false) {}

    Note(float length = 1.0) : length(length), is_rest(true) {}

    Pitch getPitch() {
        return pitch;
    }

    float getLength() {
        return length;
    }

    bool isRest() {
        return is_rest;
    }

    void incrementLength() {
        length++;
    }
};

class MidiOutput {
private:
    int tpq;
    int velocity;
    vector< vector<Note> > tracks;

public:
    MidiOutput(int tpq = 120, int velocity = DEFAULT_VELOCITY) :
        tpq(tpq), velocity(velocity) {}

    void addTrack(vector<Note> trk) {
        tracks.push_back(trk);
    }

    void write(string filename) {
        MidiFile outputFile;
        outputFile.absoluteTicks();
        outputFile.setTicksPerQuarterNote(tpq);
        outputFile.addTracks(tracks.size());
        for (int track_num = 0; track_num < tracks.size(); track_num++) {
            int actionTime = 0;
            for (Note n : tracks[track_num]) {
                if (n.isRest()) {
                    actionTime += tpq * n.getLength();
                } else {
                    vector<uchar> midievent;
                    midievent.resize(3);
                    midievent[0] = NOTE_ON;
                    midievent[1] = n.getPitch().toInt();
                    midievent[2] = velocity;
                    outputFile.addEvent(track_num + 1, actionTime, midievent);
                    actionTime += tpq * n.getLength();
                    midievent[0] = NOTE_OFF;
                    outputFile.addEvent(track_num + 1, actionTime, midievent);
                }
            }
        }
        outputFile.sortTracks();
        outputFile.write(filename);
    }
};

Pitch toPitch(string s) {
    if (s.length() == 0) {
        std::cerr << "Invalid conversion from empty string to Pitch.\n";
        exit(1);
    }
    BasePitch base = toBasePitch(s[0]);
    int accidental = 0;
    unsigned int octave = DEFAULT_OCTAVE;
    if (s.length() == 2) {
        if (isAccidental(s[1])) {
            accidental = toAccidental(s[1]);
        } else if (isOctave(s[1])) {
            octave = toOctave(s[1]);
        }
    } else if (s.length() == 3) {
        accidental = toAccidental(s[1]);
        octave = toOctave(s[2]);
    }
    return Pitch{ base, accidental, octave };
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

vector<Note> toMelody(string s) {
    vector<Note> melody;
    vector<string> tokens = tokenize(s, ' ');
    if (tokens.size() == 0) {
        return melody;
    }

    auto it = tokens.begin();
    while (it < tokens.end()) {
        Note n; // default: quarter rest
        if (it->compare(REST) != 0) {
            n = Note{toPitch(*it)};
        }
        while(++it < tokens.end() && it->compare(EXTEND) == 0) {
            n.incrementLength();
        }
        melody.push_back(n);
    }
    return melody;
}

} // namespace smf
