//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Feb  9 16:51:58 PST 2015
// Last Modified: Mon Feb  9 21:26:32 PST 2015 Updated for C++11.
// Filename:      midifile/src-programs/80off.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Converts any note-off messages in the for "9? ?? 00" to
//                "8? ?? 00".
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

// Global variables for command-line options.
Options  options;              // for command-line processing


// function declarations:
void     checkOptions        (Options& opts);
void     convertFile         (const string& inputfilename,
                              const string& outputfilename);
void     example             (void);
void     usage               (const string& command);


///////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
   options.setOptions(argc, argv);
   checkOptions(options);
   convertFile(options.getArg(1), options.getArg(2));
   return 0;
}

///////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// convertFile -- Convert "9? ?? 00" MIDI messages into "8? ?? 00" messages.
//

void convertFile(const string& inputfilename, const string& outputfilename) {
   MidiFile midifile(inputfilename);
   for (int i=0; i<midifile.getTrackCount(); i++) {
      for (int j=0; j<midifile.getEventCount(i); j++) {
         if (!midifile[i][j].isNoteOff()) {
            continue;
         }
         midifile[i][j].setCommandNibble(0x80);
      }
   }
   midifile.write(outputfilename);
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts) {
   opts.define("author=b",   "author of the program");
   opts.define("version=b",  "version of the program");
   opts.define("example=b",  "example useage of the program");
   opts.define("h|help=b",   "list of options for the program");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
              "craig@ccrma.stanford.edu, February 2015" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << "80off, version 1.0 (9 Feb 2015)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (options.getArgCount() > 2) {
      cerr << "Error: too many filenames, expecting only two" << endl;
      exit(1);
   }
   if (options.getArgCount() < 2) {
      cerr << "Error: too few filenames, expecting two:" << endl;
      cerr << "(1) input file to process, (2) name of output file." << endl;
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



