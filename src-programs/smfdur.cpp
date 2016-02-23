//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb 23 05:34:17 PST 2016
// Last Modified: Tue Feb 23 05:34:20 PST 2016
// Filename:      midifile/src-programs/smfdur.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/src-programs/smfdur.cpp
// Syntax:        C++11
//
// Description:   Calcualte the total duration of a MIDI file.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;

// User interface variables:
Options options;

// Function declarations:
void      checkOptions          (Options& opts, int argc, char* argv[]);
void      usage                 (const char* command);
void      example               (void);
double    getTotalDuration      (MidiFile& midifile);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);


   MidiFile midifile;

   int numinputs = options.getArgCount();
   for (int i=0; i < numinputs || i==0; i++) {
      midifile.clear();
      if (options.getArgCount() < 1) {
         midifile.read(cin);
      } else {
         midifile.read(options.getArg(i+1));
      }
      if (options.getArgCount() > 1) {
         cout << options.getArg(i+1) << "\t";
      }
      cout << getTotalDuration(midifile);
      cout << endl;
   }
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// getTotalDuration --
//

double getTotalDuration(MidiFile& midifile) {
   midifile.doTimeAnalysis();
   midifile.joinTracks();
   return midifile[0].last().seconds;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 23 February 2016" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 23 February 2016" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

}



//////////////////////////////
//
// example --
//

void example(void) {

}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {
   cout << "Usage: " << command << " input(s)" << endl;
}



