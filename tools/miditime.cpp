//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jul 26 13:10:22 PDT 2010
// Last Modified: Mon Feb  9 20:34:40 PST 2015 Updated for C++11.
// Filename:      ...sig/doc/examples/all/miditime/miditime.cpp
// Syntax:        C++
//
// Description:   Displays the absolute tick time and absolute time
//                in seconds for MIDI events in a MIDI file, along with
//                the track information.

#include "midifile/Options.h"
#include "midifile/MidiFile.h"
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void   checkOptions        (Options& opts);
void   example             (void);
void   usage               (const char* command);
void   processMidiFile     (MidiFile& midifile);

// user interface variables:
double starttime = 0.0;    // used with -s option
double endtime   = 0.0;    // used with -e option
int    onsetQ    = 0;      // used with -o option

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   int       status;
   MidiFile  inputfile;
   Options   options(argc, argv);

   checkOptions(options);

   status = inputfile.read(options.getArg(1));
   if (status == 0) {
      cout << "Syntax error in file: " << options.getArg(1) << "\n";
   }

   processMidiFile(inputfile);
   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// processMidiFile -- extract track and timing information from the MIDI file.
//

void processMidiFile(MidiFile& midifile) {
   midifile.absoluteTicks();
   midifile.joinTracks();
   int eventcount = midifile.getEventCount(0);
   MidiEvent *ptr;
   for (int i=0; i<eventcount; i++) {
      ptr = &(midifile[0][i]);
      int track       = ptr->track;
      int timeinticks = ptr->tick;
      double timeinsecs  = midifile.getTimeInSeconds(0, i);
      int attack = ((*ptr)[0] & 0xf0) == 0x90;
      if (onsetQ && !attack) {
         continue;
      }
      if (onsetQ && attack) {
         if ((*ptr)[2] == 0) {
            continue;
         }
      }
      
      cout << timeinticks << "\t";
      cout << timeinsecs << "\t";
      cout << track << "\t";
      cout << i << "\t";
      for (int j=0; j<(int)ptr->size(); j++) {
         if (j == 0) {
            cout << "0x" << hex << (int)(*ptr)[j] << dec << " ";
         } else {
            cout << (int)(*ptr)[j] << " ";
         }
      }
      cout << endl;
   }
}



//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {

   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.define("o|on|onset=b");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, July 2010" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "midiextract version 1.0" << endl;
      cout << "compiled: " << __DATE__ << endl;
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand().c_str());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   onsetQ = opts.getBoolean("onset");
}



//////////////////////////////
//
// example -- gives example calls to the midiexcerpt program.
//

void example(void) {
   cout <<
   "#                                                                        \n"
   << endl;
}



//////////////////////////////
//
// usage -- how to run the midiexcerpt program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   << endl;
}



