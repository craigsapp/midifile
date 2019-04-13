//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 15 12:11:36 PDT 2018
// Last Modified: Sun Apr 15 12:11:40 PDT 2018
// Filename:      midifile/tools/readstatus.cpp
// Syntax:        C++11
//
// Description:   Convert a binary standard MIDI file (or binasc MIDI file,
//                or hex-byte MIDI file) into a hex byte-code MIDI file.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.define("o|output=s", "output file name");
   options.define("w|width=i:25", "hex bytes per text line");
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) {
      midifile.read(cin);
   } else if (options.getArgCount() == 1) {
      midifile.read(options.getArg(1));
   } else {
      cerr << "Only one file input allowed" << endl;
      exit(1);
   }

   string filename = options.getString("output");
   if (filename.empty()) {
      midifile.writeHex(cout, options.getInteger("width"));
   } else {
      midifile.writeHex(filename, options.getInteger("width"));
   }

   return 0;
}

/* test data save to a file:

"MThd"
4'6
2'0
2'1
2'100

"MTRk"
4'13
v0   90 '60 '80
v200 90 '60 '80
v0   FF 2F 00


output data:

4d 54 68 64 00 00 00 06 00 00 00 01 00 78 4d 54 72 6b 00 00 00 04 00 ff 2f
00

*/


