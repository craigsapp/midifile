//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb 16 20:00:41 PST 2016
// Last Modified: Tue Feb 16 20:16:17 PST 2016
// Filename:      midifile/src-programs/createmidifile2.cpp
// Syntax:        C++11
//
// Description:   Demonstration of how to create a Multi-track MIDI file
//                with convenience functions.
//

#include "MidiFile.h"
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   MidiFile midifile;
   midifile.addTracks(2);    // Add another two tracks to the MIDI file
   int tpq = 120;            // ticks per quarter note
   midifile.setTicksPerQuarterNote(tpq);

   // melody to write to MIDI track 1: (60 = middle C)
                  // C5   C  G  G  A  A  G- F  F  E  E  D  D  C-
   int melody[50]  = {72,72,79,79,81,81,79,77,77,76,76,74,74,72,-1};
   int mrhythm[50] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2,-1};

                  // C3 C4  E  C  F  C  E  C  D B3 C4 A3  F  G  C-
   int bass[50] =   {48,60,64,60,65,60,64,60,62,59,60,57,53,55,48,-1};
   int brhythm[50]= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,-1};

   int actiontick = 0;
   int track      = 0;

   // Add some expression track (track 0) messages:
   midifile.addTrackName(track, actiontick, "Twinkle, Twinkle Little Star");
   midifile.addTempo(track, actiontick, 104.0);

   // Store melody line in track 1
   int i = 0;
   int velocity   = 64;
   int channel    = 0;
   track          = 1;

   midifile.addTrackName(track, actiontick, "Melody");
   midifile.addPatchChange(track, actiontick, channel, 40); // 40=violin

   while (melody[i] >= 0) {
      midifile.addNoteOn(track, actiontick, channel, melody[i], velocity);
      actiontick += tpq * mrhythm[i];
      midifile.addNoteOff(track, actiontick, channel, melody[i], velocity);
      i++;
   }

   // Store a base line in track 2 on channel 1 (indexed from 0):
   i = 0;
   actiontick   = 0;
   velocity     = 90;
   channel      = 1;
   track        = 2;

   midifile.addTrackName(track, actiontick, "Bass line");
   midifile.addPatchChange(track, actiontick, channel, 58); // 58=tuba

   while (bass[i] >= 0) {
      midifile.addNoteOn(track, actiontick, channel, bass[i], velocity);
      actiontick += tpq * brhythm[i];
      midifile.addNoteOff(track, actiontick, channel, bass[i], velocity);
      i++;
   }

   midifile.sortTracks();         // ensure tick times are in correct order
   midifile.write("twinkle.mid"); // write Standard MIDI File twinkle.mid
   return 0;
}


