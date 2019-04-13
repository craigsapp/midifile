//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 15 19:09:11 PDT 2018
// Last Modified: Sun Apr 15 19:09:22 PDT 2018
// Filename:      midifile/tools/extractlyrics.cpp
// Syntax:        C++11
//
// Description:   Demonstration of how to extract lyrics from a MIDI file.
//                Used the -s option to display the timestamp in seconds
//                that the lyric occurs at.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

void extractLyrics(MidiFile& midifile, int seconds);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   Options options;
   options.define("s|seconds|timestamp=b", "display timestamp for lyrics");
   options.process(argc, argv);

   MidiFile midifile;
   if (options.getArgCount() == 0) {
      midifile.read(cin);
   } else if (options.getArgCount() == 1) {
      midifile.read(options.getArg(1));
   } else {
      cerr << "Too many filenames" << endl;
      exit(1);
   }

   extractLyrics(midifile, options.getBoolean("seconds"));
   return 0;
}


//////////////////////////////
//
// extractLyrics -- Extract lyrics from MIDI file, one lyric per output line.
//

void extractLyrics(MidiFile& midifile, int seconds) {
   if (seconds) {
      midifile.doTimeAnalysis();
   }
   for (int i=0; i<midifile.size(); i++) {
      for (int j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isLyricText()) {
            continue;
         }
         string lyric = midifile[i][j].getMetaContent();
         if (seconds) {
            cout << midifile[i][j].seconds << "\t";
         }
         cout << lyric << endl;
      }
   }
}


/* Test data: 

"MThd"
4'6
2'0
2'1
2'100

"MTrk"
4'126
v0 FF 51 03 t85  ; tempo: 85 quarter note per minute

v0 FF 05 05 "Some-"
v0   90 '65 '100
v200 90 '65 '0

v0 FF 05 06 "-where"
v0   90 '77 '100
v200 90 '77 '0

v0 FF 05 02 "o-"
v0   90 '76 '100
v100 90 '76 '0

v0 FF 05 04 "-ver"
v0   90 '72 '100
v50  90 '72 '0

v0 FF 05 03 "the"
v0   90 '74 '100
v50  90 '74 '0

v0 FF 05 05 "rain-"
v0   90 '76 '100
v100 90 '76 '0

v0 FF 05 04 "-bow"
v0   90 '77 '100
v100 90 '77 '0

v0   FF 2F 00


Example output using -s to show time in seconds for lyrics:

$ bin/extractlyrics lyric.txt  -s
0	Some-
1.41176	-where
2.82353	o-
3.52941	-ver
3.88235	the
4.23529	rain-

*/


