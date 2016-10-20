//
// Programmer:    Ziemowit Laski <zlaski@ziemas.net>
// Creation Date: Sun, Jun 12, 2016 11:09:34 AM
// Last Modified: Sun, Jun 12, 2016 11:09:34 AM
// Filename:      stretch.cpp
// Syntax:        C++
//
// Description:   Stretches (or shrinks):
//                1. The position of bars (measures) in tracks,
//                   without affecting tempo, and/or
//                2. The tempo (BPM) itself.
//
//                This is useful when working with MIDI files
//                that were automatically generated from audio
//                (MP3, WAV) files.  Conversion software often
//                has difficulty in correct placement of
//                bars, and also sometimes does not preserve
//                the tempo of the original.
//   
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

void doStretch (MidiFile& midifile, double bars, double duration);

int main(int argc, char** argv) {
   Options options;
   options.define("b|bars|m|measures=d:1.0", 
      "Stretch width of bars (measures) by factor specified (WITHOUT affecting tempo)");
   options.define("d|duration|t|tempo=d:1.0",
      "Stretch duration of track(s) by factor specified");
   options.process(argc, argv);
   if (options.getArgCount() != 2) {
      cerr << "two MIDI filenames are required.\n";
      exit(1);
   }

   MidiFile midifile;
   midifile.read(options.getArg(1));
   if (!midifile.status()) {
      cerr << "Error reading MIDI file " << options.getArg(1) << endl;
      exit(1);
   }

   if (options.getBoolean("bars") || options.getBoolean("duration")) {
      double bars = options.getDouble("bars");
      double duration = options.getDouble("duration");
      if (bars < 0.1 || bars > 10.0 || duration < 0.1 || duration > 10.0) {
	  cerr << "stretch parameters must be between 0.1 and 10.\n";
	  exit(1);
      }
      doStretch(midifile, bars, duration);
   }

   midifile.write(options.getArg(2));
   return 0;
}


//////////////////////////////
//
// doStretch -- Stretch size of all bars (measures) in the file 
//   by a specified 'bars' factor. (Values lower than 1.0 will result
//   in shrinking).  Note that bars (measures) are not encoded in MIDI
//   directly as an event type.  Instead, they are computed from the
//   Pulse Per Quarter Note (PPQN) value in the file header; PPQN
//   multiplied by the number of quarter notes in a measure (as 
//   determined by the time signature contained in the FF 58
//   message) gives the number of ticks in each measure. The
//   'duration' parameter specifies the factor by which the
//   duration of the track(s) should be stretched (or shrunk,
//   if less than 1.0).

void doStretch(MidiFile& midifile, double bars, double duration) {
    int ppqn = midifile.getTicksPerQuarterNote();
    int new_ppqn = max( 24576, ppqn );
    midifile.setTicksPerQuarterNote( new_ppqn );
    double tick_mult = (double)new_ppqn / (double)ppqn;
    tick_mult /= bars;

    for (int t = 0; t < midifile.size(); ++t) {
	MidiEventList &track = midifile[t];
	for (int e = 0; e < track.size(); ++e) {
	    MidiEvent &event = track[e];
	    event.tick *= tick_mult;
	    if (event.getMetaType() == 0x51) {
		int tempo = event.getTempoMicroseconds();
		tempo *= bars;
		tempo *= duration;
		event.setTempoMicroseconds( tempo );
	    }
	}
    }
}
