#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;

void doTimeMirror  (MidiFile& midifile);
void swapLinks     (MidiEvent& event);
void doPitchMirror (MidiFile& midifile, double pivot);
void setMirror     (vector<int>& mirror, double pivot);

int main(int argc, char** argv) {
   Options options;
   options.define("p|pitch|i|inversion=d:60.0", 
      "Do a pitch mirror, reflecting around middle C");
   options.define("r|reverse|retrograde=b", 
      "Do a time reversal of the MIDI file");
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

   if (options.getBoolean("pitch")) {
      doPitchMirror(midifile, options.getDouble("pitch"));
      if (options.getBoolean("retrograde")) {
         doTimeMirror(midifile);
      }
   } else {
      doTimeMirror(midifile);
   }

   midifile.write(options.getArg(2));
   return 0;
}


//////////////////////////////
//
// doTimeMirror -- Reverse the timeline of the MIDI file.  Note-ons
//   and their matching Note-offs are switched.
//

void doTimeMirror(MidiFile& midifile) {
   midifile.linkNotePairs();
   midifile.joinTracks();
   int maxtick = midifile[0].back().tick;
   int i;
   for (i=0; i<midifile[0].size(); i++) {
      midifile[0][i].tick = maxtick - midifile[0][i].tick;
   }
   for (i=0; i<midifile[0].size(); i++) {
      if (!midifile[0][i].isNoteOn()) {
         continue;
      }
      if (midifile[0][i].isLinked()) {
         swapLinks(midifile[0][i]);
      }
   }
   midifile.splitTracks();
   midifile.sortTracks();
}


//////////////////////////////
//
// doPitchMirror -- Mirror the pitch around a particular note. 
//   If the pivot point has a fractional part, then use the space
//   between notes as the pivot rather than a partcular note.  For 
//   example if the pivot is 60, then 60->60, 61->59, 62->58, etc.
//   If the pivot is 60.5 (or anything than 60.0 up to 61.0),  then the
//   pivot will be between 60 and 61: 60->61, 61->60, 62->59, etc.
//   If a note goes out of range, it will be mirrored again off of
//   the limits of the range.

void doPitchMirror(MidiFile& midifile, double pivot) {
   vector<int> mirror;
   setMirror(mirror, pivot);
   midifile.linkNotePairs();
   
   for (int i=0; i<midifile.size(); i++) {
      for (int j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isNote()) {
            continue;
         }
         midifile[i][j].setKeyNumber(mirror[midifile[i][j][1]]);
      }
   }
}


//////////////////////////////
//
// swapLinks -- Reverse the time order of two linked events.
//

void swapLinks(MidiEvent& event) {
   MidiEvent* thisnote = &event; 
   MidiEvent* linknote = event.getLinkedEvent();
   if (linknote == NULL) {
      return;
   }
   int temptick = thisnote->tick;
   thisnote->tick = linknote->tick;
   linknote->tick = temptick;
}


//////////////////////////////
//
// setMirror -- Set the mapping from a pitch to its mirrored pitch.
//

void setMirror(vector<int>& mirror, double pivot) {
   mirror.resize(128);
   double fraction = pivot - (int)pivot;
   for (int i=0; i<mirror.size(); i++) {
      if (fraction > 0.0) {
         mirror[i] = (int)(4 * (int)pivot - 2 * i)/2;
      } else {
         mirror[i] = 2 * pivot - i;
      }
      // check for out of bounds (but only one cycle on each side).
      if (mirror[i] < 0) {
         mirror[i] = -mirror[i];
      }
      if (mirror[i] > 127) {
         mirror[i] = 127 - mirror[i];
      }
   }
}

