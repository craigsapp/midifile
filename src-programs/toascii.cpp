//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb 18 20:42:18 PST 2015
// Last Modified: Wed Feb 18 22:25:17 PST 2015
// Filename:      midifile/src-programs/toascii.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Convert a Standard MIDI file into binasc format
//                (ASCII version of the MIDI file which can be converted
//                back into the binary format without information loss).
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

// Global variables for command-line options.
Options  options;

// function declarations:
void     checkOptions        (Options& opts);
void     example             (void);
void     usage               (const string& command);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   options.setOptions(argc, argv);
   checkOptions(options);

   int status;
   MidiFile midifile;
   if (options.getArgCount()) {
      status = midifile.read(options.getArg(1));
   } else {
      status = midifile.read(cin);
   }
   if (status == 0) {
      cerr << "Error: could not read MIDI file" << endl;
      exit(1);
   }
   status = midifile.writeBinasc(cout);
   if (status == 0) {
      cerr << "Error: could not write MIDI file" << endl;
      exit(1);
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts) {
   opts.define("author=b",   "Author of the program");
   opts.define("version=b",  "Version of the program");
   opts.define("example=b",  "Example useage of the program");
   opts.define("h|help=b",   "List of options for the program");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
              "craig@ccrma.stanford.edu, February 2015" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << "toascii, version 1.0 (18 Feb 2015)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (opts.getArgCount() > 1) {
      cerr << "Error: One input file or standard input is required." << endl;
      exit(1);
   }

}



//////////////////////////////
//
// example -- shows various command-line option calls to program.
//

void example(void) {
   cout <<
   "\n"
   << endl;
}



//////////////////////////////
//
// usage -- how to run this program from the command-line.
//

void usage(const string& command) {
   cout <<
   "\n"
   << endl;
}



