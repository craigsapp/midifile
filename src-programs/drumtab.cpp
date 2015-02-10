//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu> 
// Creation Date: Sat May 17 19:34:13 PDT 2003
// Last Modified: Sat May 17 19:34:15 PDT 2003
// Filename:      ...sig/doc/examples/all/drumtab/drumtab.cpp
// Syntax:        C++
// 
// Description:   Demonstration of how to convert a drum tab into
//                a Multi-track MIDI file with one track for each
//                drum part.
//

#include "MidiFile.h"

#ifndef OLDCPP
   #include <iostream>
   using namespace std;
#else
   #include <iostream.h>
#endif


#define HIGH_HAT    59
#define SNARE       38
#define BASS_DRUM   41

#define QUARTER   120        /* ticks per quarter note */
#define SIXTEENTH 30         /* ticks per sixteenth note */

typedef unsigned char uchar;
void AddDrumTrack(MidiFile& midifile, int* data, int instrument, int ticks);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   MidiFile outputfile;        // create an empty MIDI file with one track
   outputfile.absoluteTime();  // time information stored as absolute time
                               // (will be coverted to delta time when written)
   outputfile.setTicksPerQuarterNote(QUARTER);

   int hhdata[50] = {'x', '-', 'x', '-', 'x', '-', 'x', '-', -1};
   int sndata[50] = {'-', '-', 'x', '-', '-', '-', 'x', '-', -1};
   int bsdata[50] = {'x', '-', '-', '-', 'x', '-', '-', '-', -1};

   AddDrumTrack(outputfile, hhdata, HIGH_HAT,  SIXTEENTH);
   AddDrumTrack(outputfile, sndata, SNARE,     SIXTEENTH);
   AddDrumTrack(outputfile, bsdata, BASS_DRUM, SIXTEENTH);

   outputfile.sortTracks();         // make sure data is in correct order
   outputfile.write("rhythm.mid");  // write Standard MIDI File twinkle.mid

   return 0;
}



//////////////////////////////
//
// AddDrumTrack -- add a new track to the MIDI file which contains
//    a single percussion instrument.  Tablature data is ended by a -1
//

void AddDrumTrack(MidiFile& midifile, int* data, int instrument, int ticks) {
   Array<uchar> midievent;   // temporary storage for MIDI events 
   midievent.setSize(3);     // set the size of the array to 3 bytes
   midievent[2] = 64;        // set the loudness to a constant value
   int notestate = 0;        // 0 = off, 1 = on
   int i         = 0;
   int actiontime;
   int track = midifile.addTrack();      // Add a track to the MIDI file

   while (data[i] >= 0) {
      switch (data[i]) {
         case 'x': case 'X':
            if (notestate) {
               // turn off previous note
               midievent[0] = 0x89;
               midievent[1] = instrument;
               actiontime = ticks * i - 1;
               midifile.addEvent(track, actiontime, midievent);
            }
            // turn on current note
            midievent[0] = 0x99;
            midievent[1] = instrument;
            actiontime = ticks * i;
            midifile.addEvent(track, actiontime, midievent);
            notestate = 1;
            break;
         case '0': case 'o': case 'O':
            // turn off previous note
            if (notestate) {
               midievent[0] = 0x89;
               midievent[1] = instrument;
               actiontime = ticks * i - 1;
               midifile.addEvent(track, actiontime, midievent);
               notestate = 0;
            }
         break;
      }
      i++;
   }

   if (notestate) {
      // turn off last note
      midievent[0] = 0x89;
      midievent[1] = instrument;
      actiontime = ticks * i;
      midifile.addEvent(track, actiontime, midievent);
   }

}


// md5sum: 3c36ff696d281d9010f16e0ee50ae1ce drumtab.cpp [20050403]
