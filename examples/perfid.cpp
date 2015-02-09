//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jun 18 12:14:03 PDT 2002
// Last Modified: Tue Jun 18 12:14:06 PDT 2002
// Filename:      ...sig/examples/all/perfid.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/perfid.cpp
// Syntax:        C++; museinfo
//
// Description:   Determine if a MIDI file is a live performance or if
//                it is step edit.
//

#include "MidiFile.h"
#include "Options.h"

#ifndef OLDCPP
   #include <iomanip>
   using namespace std;
#else
   #include <iomanip.h>
#endif

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
void      getNoteOnDeltas       (Array<int>& noteondeltas, 
                                 MidiFile& midifile);
void      addNoteOnEvents       (Array<int>& noteondeltas, MidiFile& midifile,
                                 int track);
void      usage                 (const char* command);
int       intcompare            (const void* a, const void* b);
void      sortArray             (Array<int>& noteondeltas);
void      printDeltas           (Array<int>& noteondeltas);
void      printID               (Array<int>& noteondeltas);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile;
   midifile.read(options.getArg(1));
   midifile.absoluteTime();
   Array<int> noteondeltas;
   noteondeltas.setSize(maxcount);
   noteondeltas.setSize(0);
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

void printDeltas(Array<int>& noteondeltas) {
   int i;
   int count = 1;
   if (noteondeltas.getSize() == 0) {
      return;
   }
   for (i=1; i<noteondeltas.getSize(); i++) {
      if (noteondeltas[i] != noteondeltas[i-1]) {
         cout << count << "\t" << noteondeltas[i-1] << "\n";
         count = 1;
      } else {
         count++;
      }
   }
   cout << count << "\t" << noteondeltas.last() << "\n";
}



//////////////////////////////
//
// printID --
//

void printID(Array<int>& noteondeltas) {
   if (fileQ) {
      cout << options.getArg(1) << "\t";
   }
   int i;
   int count = 1;
   if (noteondeltas.getSize() == 0) {
      cout << "Empty" << endl;
      return;
   }
   Array<int> deltas;
   Array<int> hist;
   deltas.setSize(noteondeltas.getSize());
   hist.setSize(noteondeltas.getSize());
   deltas.setSize(0);
   hist.setSize(0);
   for (i=1; i<noteondeltas.getSize(); i++) {
      if (noteondeltas[i] != noteondeltas[i-1]) {
         deltas.append(noteondeltas[i-1]);
         hist.append(count);
         count = 1;
      } else {
         count++;
      }
   }
   deltas.append(noteondeltas.last());
   hist.append(count);
   int size = deltas.getSize();
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

void getNoteOnDeltas(Array<int>& noteondeltas, MidiFile& midifile) {
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

void sortArray(Array<int>& noteondeltas) {
   qsort(noteondeltas.getBase(), noteondeltas.getSize(), sizeof(int), intcompare);

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

void addNoteOnEvents(Array<int>& noteondeltas, MidiFile& midifile,
      int track) {
   int i;
   int lasttime = -1;
   MFEvent event;
   int delta = 0;
   
   for (i=0; i<midifile.getNumEvents(track); i++) {
      event = midifile.getEvent(track, i);
      if ((event.data[0] & 0xf0) == 0x90) {
         if (event.data[2] > 0) {
            if (lasttime < 0) {
               lasttime = event.time;
            } else {
               delta = event.time - lasttime;
               if (delta < cutoff) {
                  noteondeltas.append(delta);
               }
               lasttime = event.time;
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
      usage(opts.getCommand());
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
      usage(opts.getCommand());
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



// md5sum: 123fb815d981e38fc24a183785895361 perfid.cpp [20110711]
