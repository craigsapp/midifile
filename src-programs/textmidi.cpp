//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Nov 27 11:43:31 PST 1999
// Last Modified: Mon Nov 29 14:01:34 PST 1999
// Last Modified: Mon Feb  9 21:26:32 PST 2015 Updated for C++11.
// Filename:      ...sig/doc/examples/all/textmidi/textmidi.cpp
// Syntax:        C++
//
// Description:   Reads a MIDI file and converts data to/from ASCII text.
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

#define STYLE_TIME_DELTA      'd'
#define STYLE_TIME_ABSOLUTE   'a'

// global variables:
int timestyle = STYLE_TIME_DELTA;    // command-line style options (-a | -d)

// function declarations:
void  checkOptions    (Options& opts);
void  example         (void);
void  usage           (const char* command);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   int       status;
   MidiFile  inputfile;
   Options   options(argc, argv);

   checkOptions(options);
   for (int i=1; i<=options.getArgCount(); i++) {
      status = inputfile.read(options.getArg(i));
      if (options.getArgCount() > 1) {
         cout << "\n\n\n+++ FILE " << i << "++++++++++++++++++++++++++++\n\n";
      }
      if (status != 0) {
         switch (timestyle) {
            case 'd': inputfile.deltaTicks();    break;
            case 'a': inputfile.absoluteTicks(); break;
         }
         cout << inputfile;
      } else {
         cout << "Syntax error in file: " << options.getArg(i) << "\n";
      }
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {
   opts.define("a|abs|absolute=b");
   opts.define("d|delta=b");
   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, Nov 1999" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "textmidi version 1.0" << endl;
      cout << "compiled: " << __DATE__ << endl;
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   // can only have one output filename
   if (opts.getArgCount() == 0) {
      cout << "Error: need one input MIDI file." << endl;
      usage(opts.getCommand().data());
      exit(1);
   }

   if (opts.getBoolean("absolute")) {
      timestyle = STYLE_TIME_ABSOLUTE;
   }

}



//////////////////////////////
//
// example -- gives example calls to the textmidi program.
//

void example(void) {
   cout <<
   "# textmidi examples:                                                     \n"
   "       textmidi -a midifile.mid | more                                   \n"
   << endl;
}



//////////////////////////////
//
// usage -- how to run the textmidi program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   "Creates a printable form of the data in a MIDI File.                     \n"
   "                                                                         \n"
   "Usage: " << command << " [-d|-a] midifile[s]                             \n"
   "                                                                         \n"
   "Options:                                                                 \n"
   "   -a = set time values to absolute time                                 \n"
   "   -d = set time values to delta time                                    \n"
   "   --options = list of all options, aliases and default values.          \n"
   "                                                                         \n"
   "                                                                         \n"
   << endl;
}



