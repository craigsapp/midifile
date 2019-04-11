#ifndef NOTE_HPP_
#define NOTE_HPP_

#include <map>
#include <iostream>
#include <string>
#include <sstream>

namespace smf {

using std::map;
using std::string;
using std::vector;

uint8_t DEFAULT_OCTAVE = 5;
string EXTEND = "-";
string REST = ".";

enum BasePitch {
    C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
};

map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

map<char, int8_t> charAccidentalMap {
    {'#', 1} /* sharp */, {'b', -1} /* flat */
};

bool isBasePitch(char c) {
    return charPitchMap.find(c) != charPitchMap.end();
}

BasePitch toBasePitch(char c) {
    return charPitchMap[c];
}

bool isAccidental(char c) {
    return charAccidentalMap.find(c) != charAccidentalMap.end();
}

int8_t toAccidental(char c) {
    return charAccidentalMap[c];
}

bool isOctave(char c) {
    return '0' <= c && c <= '9';
}

uint8_t toOctave(char c) {
    return (uint8_t) (c - '0');
}

/*
 * A Pitch is composed of a base note, an optional accidental, and an octave.
 * For example, middle C is { C, 0, 5 }.
 */
class Pitch {
private:
    BasePitch base;
    int8_t accidental; // e.g. sharp or flat
    uint8_t octave;

public:
    Pitch(BasePitch base = C, int8_t accidental = 0,
          uint8_t octave = DEFAULT_OCTAVE) :
        base(base), accidental(accidental), octave(octave) {}

    BasePitch getBasePitch() {
        return base;
    }

    uint8_t toInt() {
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

/*
 * Currently, a Pitch can be represented by a string with base pitch and
 * optionally, accidental and octave. "C", "C#", "C3", "C#3" are all examples
 * of valid notes.
 */
Pitch toPitch(string s) {
    if (s.length() == 0) {
        std::cerr << "Invalid conversion from empty string to Pitch.\n";
        exit(1);
    }
    BasePitch base = toBasePitch(s[0]);
    int8_t accidental = 0;
    uint8_t octave = DEFAULT_OCTAVE;
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

} // namespace smf

#endif
