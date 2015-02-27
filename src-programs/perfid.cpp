//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jun 18 12:14:03 PDT 2002
// Last Modified: Mon Feb  9 20:28:07 PST 2015 Updated for C++11.
// Filename:      midifile/src-programs/perfid.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/perfid.cpp
// Syntax:        C++; museinfo
//
// Description:   Determine if a MIDI file is a live performance or if
//                it is step edit.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

// user interface variables:
Options options;
int     track    = -1;         // track to extract from (starting from 0)
int     debugQ   = 0;          // use with --debug option
int     maxcount = 100000;     // maximum number of notes expected
int     rawQ     = 0;          // display raw data used to determine id
int     cutoff   = 1000000;    // maximum duration to consider
int     fileQ    = 0;          // print file name before id

// function declarations:
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      getNoteOnDeltas       (vector<int>& noteondeltas,
                                 MidiFile& midifile);
void      addNoteOnEvents       (vector<int>& noteondeltas, MidiFile& midifile,
                                 int track);
void      usage                 (const char* command);
int       intcompare            (const void* a, const void* b);
void      sortArray             (vector<int>& noteondeltas);
void      printDeltas           (vector<int>& noteondeltas);
void      printID               (vector<int>& noteondeltas);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile;
   midifile.read(options.getArg(1));
   midifile.absoluteTicks();
   vector<int> noteondeltas;
   noteondeltas.reserve(maxcount);
   noteondeltas.clear();
   midifile.joinTracks();
   getNoteOnDeltas(noteondeltas, midifile);
   if (rawQ) {
      cout << "// ";
      printID(noteondeltas);
      printDeltas(noteondeltas);
   } else {
      printID(noteondeltas);
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// printDeltas --
//

void printDeltas(vector<int>& noteondeltas) {
   int i;
   int count = 1;
   if (noteondeltas.size() == 0) {
      return;
   }
   for (i=1; i<(int)noteondeltas.size(); i++) {
      if (noteondeltas[i] != noteondeltas[i-1]) {
         cout << count << "\t" << noteondeltas[i-1] << "\n";
         count = 1;
      } else {
         count++;
      }
   }
   cout << count << "\t" << noteondeltas.back() << "\n";
}



//////////////////////////////
//
// printID --
//

void printID(vector<int>& noteondeltas) {
   if (fileQ) {
      cout << options.getArg(1) << "\t";
   }
   int i;
   int count = 1;
   if (noteondeltas.size() == 0) {
      cout << "Empty" << endl;
      return;
   }
   vector<int> deltas;
   vector<int> hist;
   deltas.reserve(noteondeltas.size());
   hist.reserve(noteondeltas.size());
   deltas.clear();
   hist.clear();
   for (i=1; i<(int)noteondeltas.size(); i++) {
      if (noteondeltas[i] != noteondeltas[i-1]) {
         deltas.push_back(noteondeltas[i-1]);
         hist.push_back(count);
         count = 1;
      } else {
         count++;
      }
   }
   deltas.push_back(noteondeltas.back());
   hist.push_back(count);
   int size = deltas.size();
   if (size > 2) {
      if (deltas[0] == 0 && hist[0] > 10 && deltas[1] >= 10) {
         cout << "Quantized" << endl;
         return;
      }
   }

   if (size > 3 && deltas[5] < 10) {
      if (hist[0] < hist[1] + hist[2] + hist[3] + hist[4]) {
         cout << "Performance" << endl;
         return;
      } else {
         cout << "Quantized" << endl;
         return;
      }
   }

   cout << "Unknown" << endl;
}



//////////////////////////////
//
// getNoteOnDeltas --
//

void getNoteOnDeltas(vector<int>& noteondeltas, MidiFile& midifile) {
   int i;
   for (i=0; i<midifile.getNumTracks(); i++) {
      addNoteOnEvents(noteondeltas, midifile, i);
   }

   // sort note ons here.
   sortArray(noteondeltas);
}


//////////////////////////////
//
// sortArray --
//

void sortArray(vector<int>& noteondeltas) {
   qsort(noteondeltas.data(), noteondeltas.size(), sizeof(int), intcompare);

}



//////////////////////////////
//
// intcompare -- compare two integers for ordering
//

int intcompare(const void* a, const void* b) {
   if (*((int*)a) < *((int*)b)) {
      return -1;
   } else if (*((int*)a) > *((int*)b)) {
      return 1;
   } else {
      return 0;
   }
}



//////////////////////////////
//
// addNoteOnEvents -- get the Note-On delta times from the specified
//                    track.
//

void addNoteOnEvents(vector<int>& noteondeltas, MidiFile& midifile,
      int track) {
   int i;
   int lasttime = -1;
   MidiEvent* event;
   int delta = 0;

   for (i=0; i<midifile.getNumEvents(track); i++) {
      event = &midifile[track][i];
      if (((*event)[0] & 0xf0) == 0x90) {
         if ((*event)[2] > 0) {
            if (lasttime < 0) {
               lasttime = event->tick;
            } else {
               delta = event->tick - lasttime;
               if (delta < cutoff) {
                  noteondeltas.push_back(delta);
               }
               lasttime = event->tick;
            }
         }
      }
   }
}



//////////////////////////////
//
// checkOptions -- process command-line options.
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");

   opts.define("t|track=i:-1", "which track to extract");
   opts.define("r|raw=b",      "print noteon deltas");
   opts.define("f|file=b",      "display filename");
   opts.define("max=i:100000", "maximum number of notes expected in input");
   opts.define("debug=b",  "debug mode to find errors in input file");

   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 18 Jun 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 18 Jun 2002" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   track    = opts.getInteger("track");
   debugQ   = opts.getBoolean("debug");
   maxcount = opts.getInteger("max");
   rawQ     = opts.getBoolean("raw");
   fileQ    = opts.getBoolean("file");

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().data());
      exit(1);
   }
}



//////////////////////////////
//
// example -- give example calls to the program.
//

void example(void) {
}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {
}



