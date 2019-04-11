#ifndef MIDIOUTPUT_HPP_
#define MIDIOUTPUT_HPP_

#include <string>
#include "MidiFile.h"
#include "Note.hpp"
#include "Track.hpp"

namespace smf {

uint8_t NOTE_ON = 0x90;
uint8_t NOTE_OFF = 0x80;

/*
 * MIDI representation that can be written into an output file.
 */
class MidiOutput {
private:
    int tpq;
    vector<Track> tracks;

public:
    MidiOutput(int tpq = 120) : tpq(tpq) {}

    void addTrack(Track trk) {
        tracks.push_back(trk);
    }

    void write(string filename) {
        MidiFile outputFile;
        outputFile.absoluteTicks();
        outputFile.setTicksPerQuarterNote(tpq);
        outputFile.addTracks(tracks.size());
        for (int track_num = 0; track_num < tracks.size(); track_num++) {
            int actionTime = 0;
            Track trk = tracks[track_num];
            for (Note n : trk.getNotes()) {
                if (n.isRest()) {
                    // simply skip for the duration of the note
                    actionTime += tpq * n.getLength();
                } else {
                    // NOTE_ON event
                    vector<uchar> midievent = {
                        NOTE_ON, n.getPitch().toInt(), trk.getVelocity()
                    };
                    outputFile.addEvent(track_num + 1, actionTime, midievent);
                    actionTime += tpq * n.getLength();
                    // NOTE_OFF event
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
