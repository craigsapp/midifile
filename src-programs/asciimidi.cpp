//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat May 28 09:56:21 PDT 2016
// Last Modified: Sat May 28 10:34:30 PDT 2016
// Filename:      midifile/src-programs/asciimidi.cpp
// Syntax:        C++11
//
// Description:   Print MIDI file in binasc format, allowing for
//                reconversion into a binary Standard MIDI File.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;

void checkOptions  (Options& opts, int argc, char* argv[]);
void printAscii    (MidiFile& file, Options& options);


int main(int argc, char** argv) {
   MidiFile midifile;
   Options options;
   checkOptions(options, argc, argv);
   int filecount = options.getArgCount();
   if (filecount == 0) {
         midifile.read(cin);
         printAscii(midifile, options);
   } else {
      for (int i=1; i<=filecount; i++) {
         midifile.read(options.getArg(i));
         printAscii(midifile, options);
         if (i < filecount) {
            cout << "\n\n\n";
         }
      }
   }

   return 0;
}



//////////////////////////////
//
// printAscii -- Print a MIDI file in ASCII format (annotated
//     hex bytes).
//

void printAscii(MidiFile& file, Options& options) {
   if (options.getBoolean("hex")) {
      file.writeHex(cout);
   } else if (options.getBoolean("no-comments")) {
      file.writeBinasc(cout);
   } else {
      cout << file;
   }
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("x|hex=b", "Print as ASCII hex bytes");
   opts.define("C|no-comments=b", "Print in binasc format with no comments");
   opts.process(argc, argv);
}



