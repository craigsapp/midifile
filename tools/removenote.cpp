//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 15 10:43:19 PDT 2018
// Last Modified: Sun Apr 15 11:01:11 PDT 2018
// Filename:      midifile/tools/removenote.cpp
// Syntax:        C++11
//
// Description:   Demonstration of how to remove a MIDI message.
//

#include "MidiFile.h"
#include <iostream>

using namespace std;
using namespace smf;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   MidiFile midifile;
   int track   = 0;
   int channel = 0;
   midifile.addNoteOn(track,  1, channel, 60, 64);
   midifile.addNoteOn(track,  2, channel, 60,  0);
   midifile.addNoteOn(track,  3, channel, 61, 64);
   midifile.addNoteOn(track,  4, channel, 61,  0);
   midifile.addNoteOn(track,  5, channel, 62, 64);
   midifile.addNoteOn(track,  6, channel, 62,  0);
   midifile.addNoteOn(track,  7, channel, 63, 64);
   midifile.addNoteOn(track,  8, channel, 63,  0);
   midifile.addNoteOn(track,  9, channel, 64, 64);
   midifile.addNoteOn(track, 10, channel, 64,  0);
   midifile.addNoteOn(track, 11, channel, 65, 64);
   midifile.addNoteOn(track, 12, channel, 65,  0);

   // Delete any MIDI message which is a note message on key 63:
   int removekey = 63;
   for (int i=0; i<midifile[0].getSize(); i++) {
      if (!midifile[0][i].isNote()) {
         continue;
      }
      if (midifile[0][i].getP1() == removekey) {
         midifile[0][i].clear();
      }
   }

   // calling MidiFile::removeEmpties() is optional, since
   // writing the MidiFile will automatically skip over any
   // MIDI events with no message content:
   midifile.removeEmpties();

   cout << midifile;
}



