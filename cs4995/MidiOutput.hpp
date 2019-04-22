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
    int ticks; // ticks per quarter
    vector<Track> tracks;

    // writes notes from current collection of notes to midifile
    // does not assume single notes are a special case
    // does not handle incrementing actionTime
    // TODO integer overflow checking
    void write_notes(
        MidiFile& outputFile, Chord c, Track trk, int track_num, int actionTime)
    {
        for (Pitch p : c.getPitches()) {
            vector<uchar> midievent = {
                NOTE_ON,
                static_cast<uint8_t>(p.toInt() + OCTAVE_WIDTH * trk.getOctave()),
                static_cast<uint8_t>(trk.getVelocity())
            };

            outputFile.addEvent(track_num + 1, actionTime, midievent);

            midievent[0] = NOTE_OFF;
            outputFile.addEvent(
                track_num + 1, actionTime + ticks * c.getLength(), midievent);
        }
    }

public:

    MidiOutput(int ticks = 120) : ticks(ticks) {}

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
        outputFile.setTicksPerQuarterNote(ticks);
        outputFile.addTracks(tracks.size());
        for (int track_num = 0; track_num < tracks.size(); track_num++) {
            int actionTime = 0;
            Track trk = tracks[track_num];

            // write all notes to midifile
            // if it's a rest, just increment actionTime
            for (Chord c : trk.getChords()) {
                if (!(c.isRest())) {
                    write_notes(outputFile, c, trk, track_num, actionTime);
                }
                actionTime += ticks * c.getLength();
            }
        }
        outputFile.sortTracks();
        outputFile.write(filename);
    }

};

} // namespace smf

#endif
