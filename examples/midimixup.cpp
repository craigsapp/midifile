//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu> 
// Creation Date: Thu Dec  2 12:45:43 PST 1999
// Last Modified: Thu Dec  2 13:53:31 PST 1999
// Filename:      midimixup.cpp
// Syntax:        C++
// 
// Description:   Reads a standard MIDI file, adjusts the timing of
//                the events in a random manner, then writes the file
//                back out to a standard MIDI file.
//

#include "MidiFile.h"
#include "Options.h"

#ifndef OLDCPP
   #include <iostream>
#else
   #include <iostream.h>
#endif


// function declarations:
void adjustTime(int deviation, int& aTime);
void checkOptions(Options& opts);
void example(void);
void usage(const char* command);

// Global variables:
MidiFile midifile;            // performance interface

// command-line variables
int maxdev  = 10;             // maximum unit deviation


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   Options options(argc, argv);
   checkOptions(options);

   MidiFile midifile;
   midifile.read(options.getArg(1));

   // note: when a MIDI file is read in by the read() function,
   // the delta timings are converted to absolute timings.
   // The next piece of code will add random timing shifts
   // to all events in the file according to the -m command line
   // option.

   int track, event;
   for (track=0; track<midifile.getNumTracks(); track++) {
      for (event=0; event<midifile.getNumEvents(track); event++) {
         adjustTime(maxdev, midifile.getEvent(track, event).time);
      }
   }

   // the timing values are not sorted any longer, so they
   // have to be resorted in time.
   midifile.sortTracks();

   // you can view an ASCII version of the MIDI file with the
   // following code:
   //  cout << midifile;

   // now ready to write out the new midi file to the second argument
   midifile.write(options.getArg(2));

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// adjustTime -- adjust an absolute MIDI time.  If the time
//    becomes negative, then the time will be set to zero.
//

void adjustTime(int deviation, int& aTime) {
   // if you are using a UNIX computer, you might want to try
   // the lrand48() function which gives more random values.
   int sign = rand() % 2 ? 1 : -1;
   aTime += sign * (rand() % deviation + 1);
}



//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {
   opts.define("m|max|max-deviation=i:10");
   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.process();              

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 2 December 1999" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "midimixup version 1.0" << endl;
      cout << "compiled: " << __DATE__ << endl;
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }               

   // must have two filenames on the command-line
   if (opts.getArgCount() == 1) {
      cout << "Need an output file specified." << endl;
      usage(opts.getCommand());
      exit(1);
   }
   if (opts.getArgCount() != 2) {
      cout << "Error: need one input and one output MIDI filename." << endl;
      usage(opts.getCommand());
      exit(1);
   } 

   maxdev = opts.getInteger("max-deviation");
}



//////////////////////////////
//
// example -- gives example calls to the midiplay program.
//

void example(void) {
   cout <<
   "# midimixup examples:                                                    \n"
   "       midimixup -m 10 fileinput.mid fileoutput.mid                      \n"
   << endl;
}

 

//////////////////////////////
//
// usage -- how to run the textmidi program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   "Applies a random variation to the timing of notes in a MIDI file.        \n"
   "                                                                         \n"
   "Usage: " << command << " [-m amt] input-midifile output-midifile         \n"
   "                                                                         \n"
   "Options:                                                                 \n"
   "   -m amt    = adjust each timing value randomly up to +/- amt           \n"
   "   --options = list of all options, aliases and default values.          \n"
   "                                                                         \n"
   "                                                                         \n"
   << endl;               
}


// md5sum: cf2e5559e7e3e8b36b93c0f569425bf9 midimixup.cpp [20050403]
