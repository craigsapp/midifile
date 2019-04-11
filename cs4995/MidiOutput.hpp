#ifndef MIDIOUTPUT_HPP_
#define MIDIOUTPUT_HPP_

#include <string>
#include "MidiFile.h"
#include "Note.hpp"

namespace smf {

uint8_t DEFAULT_VELOCITY = 64;
uint8_t NOTE_ON = 0x90;
uint8_t NOTE_OFF = 0x80;

class MidiOutput {
private:
    int tpq;
    uint8_t velocity;
    vector< vector<Note> > tracks;

public:
    MidiOutput(int tpq = 120, uint8_t velocity = DEFAULT_VELOCITY) :
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
                    vector<uchar> midievent = {
                        NOTE_ON, n.getPitch().toInt(), velocity
                    };
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

} // namespace smf

#endif
