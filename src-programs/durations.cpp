//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Feb 19 20:43:49 PST 2015
// Last Modified: Thu Feb 19 20:43:52 PST 2015
// Filename:      midifile/src-programs/durations.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Print note durations in a MidiFile.  Note-ons with no
//                no matching note-offs (such as for rhythm-channel
//                events) will have a duration of 0.
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;

// Global variables for command-line options.
Options  options;
int      quarterQ = 0;  // used with -q option: time units in quarter notes.
int      joinQ    = 0;  // used with -j option: join tracks before printing.
int      secondsQ = 0;  // used with -s option: print times in seconds.

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

   int tpq = midifile.getTicksPerQuarterNote();
   midifile.linkNotePairs();
   if (joinQ) {
      midifile.joinTracks();
   }
   MidiEvent* mev;
   double duration;

   if (secondsQ) {
      midifile.doTimeAnalysis();
   }

   if (secondsQ) {
      cout << "SEC\tDUR\tTRACK\tNOTE\n";
   } else if (quarterQ) {
      cout << "QTRS\tDUR\tTRACK\tNOTE\n";
   } else {
      cout << "TICKS\tDUR\tTRACK\tNOTE\n";
   }
   cout << "============================\n";

   for (int track=0; track < midifile.getTrackCount(); track++) {
      for (int i=0; i<midifile[track].size(); i++) {
         mev = &midifile[track][i];
         if (!mev->isNoteOn()) {
            continue;
         }
         if (secondsQ) {
            duration = mev->getDurationInSeconds();
         } else {
            duration = mev->getTickDuration();
         }

         if (secondsQ) {
            cout << mev->seconds << '\t';
            cout << duration << '\t';
         } else if (quarterQ) {
            cout << mev->tick/tpq << '\t';
            cout << duration/tpq << '\t';
         } else {
            cout << mev->tick << '\t';
            cout << duration << '\t';
         }
         cout << mev->track << '\t';
         cout << mev->getKeyNumber();
         cout << endl;
      }
      if (midifile.getTrackCount() > 1) {
         cout << endl;
      }
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts) {
   opts.define("q|quarter=b", "Display ticks in quarter note units");
   opts.define("j|join=b",    "Join tracks before printing");
   opts.define("s|seconds=b", "Display times in seconds");

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
      cout << "duration, version 1.0 (19 Feb 2015)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   quarterQ  = opts.getBoolean("quarter");
   joinQ     = opts.getBoolean("join");
   secondsQ  = opts.getBoolean("seconds");
   if (secondsQ) {
      quarterQ = 0;
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



