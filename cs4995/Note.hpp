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

int OCTAVE_WIDTH = 12;
int DEFAULT_OCTAVE = 5;
string EXTEND = "-";
string REST = ".";

enum BasePitch {
    C = 0, D = 2, E = 4, F = 5, G = 7, A = 9, B = 11
};

map<char, BasePitch> charPitchMap {
    {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'A', A}, {'B', B}
};

map<char, int> charAccidentalMap {
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

int toAccidental(char c) {
    return charAccidentalMap[c];
}

bool isOctave(char c) {
    return '0' <= c && c <= '9';
}

int toOctave(char c) {
    return (int) (c - '0');
}

/*
 * A Pitch is composed of a base note, an optional accidental, and an octave.
 * For example, middle C is { C, 0, 5 }.
 */
class Pitch {
private:
    BasePitch base;
    int accidental; // e.g. sharp or flat
    int octave;

public:
    Pitch(BasePitch base = C, int accidental = 0,
          int octave = DEFAULT_OCTAVE) :
        base(base), accidental(accidental), octave(octave) {}

    /*
     * Currently, a Pitch can be represented by a string with base pitch and
     * optionally, accidental and octave. "C", "C#", "C3", "C#3" are all examples
     * of valid notes.
     */
    Pitch(string s) {
        if (s.length() == 0) {
            std::cerr << "Invalid conversion from empty string to Pitch.\n";
            exit(1);
        }
        base = toBasePitch(s[0]);
        accidental = 0;
        octave = DEFAULT_OCTAVE;
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
    }

    // MIDI pitch representation to Pitch object
    Pitch(int p) {
        octave = p / OCTAVE_WIDTH;
        switch(p % OCTAVE_WIDTH) {
            case 0:  base = C; accidental = 0;  break;
            case 1:  base = C; accidental = 1;  break;
            case 2:  base = D; accidental = 0;  break;
            case 3:  base = E; accidental = -1; break;
            case 4:  base = E; accidental = 0;  break;
            case 5:  base = F; accidental = 0;  break;
            case 6:  base = F; accidental = 1;  break;
            case 7:  base = G; accidental = 0;  break;
            case 8:  base = A; accidental = -1; break;
            case 9:  base = A; accidental = 0;  break;
            case 10: base = B; accidental = -1; break;
            case 11: base = B; accidental = 0;  break;
            default: break;
        }
    }

    BasePitch getBasePitch() {
        return base;
    }

    int toInt() {
        return base + accidental + OCTAVE_WIDTH * octave;
    }

    void transform(int delta) {
        Pitch result{toInt() + delta};
        base = result.base;
        accidental = result.accidental;
        octave = result.octave;
    }

    void transform(const map<int, int> &deltas) {
        auto val = deltas.find(base + accidental);
        if (val != deltas.end()) {
            Pitch result{toInt() + val->second};
            base = result.base;
            accidental = result.accidental;
            octave = result.octave;
        }
    }

    friend std::ostream& operator<<(std::ostream &os, const Pitch &p);
};

std::ostream& operator<<(std::ostream &os, const Pitch &p) {
    os << p.base <<
        (p.accidental == 1 ? "#" : p.accidental == -1 ? "b" : "") << p.octave;
    return os;
}

class Chord {
private:
    vector<Pitch> pitches;
    float length; // quarter note == 1.0
    bool is_rest;

public:
    
    // constructs a chord out of a collection of pitches
    Chord(vector<Pitch> pitches, float length = 1.0) :
        pitches(pitches), length(length), is_rest(false) {}

    // individual notes are not a special case, just chords
    // with only one pitch
    Chord(Pitch pitch, float length = 1.0) :
	pitches(), length(length), is_rest(false) {
	    pitches.push_back(pitch);	
    }

    Chord(float length = 1.0) : length(length), is_rest(true) {}

    vector<Pitch> getPitches() {
        return pitches;
    }

    void transformPitch(int delta) {
        for (Pitch p: pitches){
	    p.transform(delta);
	}
    }

    void transformPitch(const map<int, int> &deltas) {
        for (Pitch p: pitches){
	    p.transform(deltas);
	}
    }

    float getLength() {
        return length;
    }

    bool isRest() {
        return is_rest;
    }

    bool isNote() {
	return pitches.size() == 1;
    }
    void incrementLength() {
        length++;
    }
};

} // namespace smf

#endif
