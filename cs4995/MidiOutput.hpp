#ifndef MIDI_OUTPUT_HPP_
#define MIDI_OUTPUT_HPP_

#include <string>
#include <vector>
#include "Key.hpp"
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

    void transpose(int delta) {
        for (Track &trk : tracks) {
            trk.transpose(delta);
        }
    }

    void modulate(const Scale &src, const Scale &dest) {
        for (Track &trk : tracks) {
            trk.modulate(src, dest);
        }
    }

    void write(string filename) {
        MidiFile outputFile;
        outputFile.absoluteTicks();
        outputFile.setTicksPerQuarterNote(tpq);
        outputFile.addTracks(tracks.size());
        for (int track_num = 0; track_num < tracks.size(); track_num++) {
            int actionTime = 0;
            Track trk = tracks[track_num];
            for (Chord c : trk.getChords()) {
                if (c.isRest()) {
                    // simply skip for the duration of the note
                    actionTime += tpq * c.getLength();
                } else if (c.isNote()){
                    // NOTE_ON event
                    // TODO: add overflow checking since we're using ints
                    vector<uchar> midievent = {
                        NOTE_ON,
                        static_cast<uint8_t>(c.getPitches()[0].toInt()),
                        static_cast<uint8_t>(trk.getVelocity())
                    };
                    outputFile.addEvent(track_num + 1, actionTime, midievent);
                    actionTime += tpq * c.getLength();
                    // NOTE_OFF event
                    midievent[0] = NOTE_OFF;
                    outputFile.addEvent(track_num + 1, actionTime, midievent);
                } else {
			// TODO handle chord writing
		}
            }
        }
        outputFile.sortTracks();
        outputFile.write(filename);
    }
};

} // namespace smf

#endif
