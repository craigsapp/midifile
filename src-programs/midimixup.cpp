//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Dec  2 12:45:43 PST 1999
// Last Modified: Fri Feb 20 20:34:39 PST 2015
// Filename:      midimixup.cpp
// Syntax:        C++11
//
// Description:   Reads a standard MIDI file, move the pitches around
//                into a random order.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>

using namespace std;

// function declarations:
void checkOptions         (Options& opts);
void example              (void);
void usage                (const char* command);
void randomizeNotes       (vector<MidiEvent*>& notes);
void reverseNotes         (vector<MidiEvent*>& notes);
void swapNotes            (vector<MidiEvent*>& notes, int index1, int index2);

class pairing {
   public:
      int index;
      double value;
};

// variables related to command-line options
int     reverseQ = 0;         // used with -r option: reverse notes

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   Options options(argc, argv);
   checkOptions(options);

   MidiFile midifile;
   if (options.getArgCount() == 2) {
      midifile.read(options.getArg(1));
   } else if (options.getArgCount() == 1) {
      midifile.read(options.getArg(1));
   } else {
      cerr << "Need one optional MIDI file (or standard input), ";
      cerr << "and one output" << endl;
      exit(1);
   }

   midifile.linkNotePairs();
   vector<MidiEvent*> notes;
   notes.reserve(123456);
   int track, event;
   for (track=0; track<midifile.getNumTracks(); track++) {
      for (event=0; event<midifile.getNumEvents(track); event++) {
         if (midifile[track][event].isNoteOn()) {
            if (midifile[track][event].isLinked()) {
               notes.push_back(&midifile[track][event]);
            }
         }
      }
   }

   if (reverseQ) {
      reverseNotes(notes);
   } else {
      randomizeNotes(notes);
   }

   if (options.getArgCount() == 2) {
      midifile.write(options.getArg(2));
   } else {
      midifile.write(options.getArg(1));
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// reverseNotes -- Reverse the order of the notes in the file.
//

void reverseNotes(vector<MidiEvent*>& notes) {
   int count = notes.size();
   for (int i=0; i<count/2; i++) {
      swapNotes(notes, i, count-1-i);
   }
}



//////////////////////////////
//
// randomizeNotes -- adjust an absolute MIDI time.  If the time
//    becomes negative, then the time will be set to zero.
//

void randomizeNotes(vector<MidiEvent*>& notes) {
   random_device rd;
   mt19937 md(rd());
   uniform_real_distribution<double> dist(0, 100);
   int count = notes.size();
   vector<pairing> neworder;
   neworder.resize(notes.size());
   int i;
   for (i=0; i<(int)neworder.size(); i++) {
      neworder[i].index = i;
      neworder[i].value = dist(md);
   }
   sort(neworder.begin(), neworder.end(),
      [](const pairing& a, const pairing& b) {
         return a.value > b.value;
      });

   for (i=0; i<count; i++) {
      swapNotes(notes, i, neworder[i].index);
   }
}



//////////////////////////////
//
// swapNotes -- move a note from one location to another.
//

void swapNotes(vector<MidiEvent*>& notes, int index1, int index2) {
   MidiEvent* noteon1  = notes[index1];
   MidiEvent* noteon2  = notes[index2];
   MidiEvent* noteoff1 = notes[index1]->getLinkedEvent();
   MidiEvent* noteoff2 = notes[index2]->getLinkedEvent();
   if (noteon1  == NULL) { return; }
   if (noteon2  == NULL) { return; }
   if (noteoff1 == NULL) { return; }
   if (noteoff2 == NULL) { return; }
   int pitch1 = noteon1->getKeyNumber();
   int pitch2 = noteon2->getKeyNumber();
   if (pitch1 == pitch2) { return; }
   if (pitch1 < 0) { return; }
   if (pitch2 < 0) { return; }

   noteon1->setKeyNumber(pitch2);
   noteoff1->setKeyNumber(pitch2);
   noteon2->setKeyNumber(pitch1);
   noteoff2->setKeyNumber(pitch1);
}



//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {
   opts.define("r|reverse=b", "Reverse the order of notes");

   opts.define("author=b",    "Author of the program");
   opts.define("version=b",   "Print version of the program");
   opts.define("example=b",   "Display example use of the program");
   opts.define("help=b",      "Dispay help for the program");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 2 December 1999" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "midimixup version 2.0" << endl;
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

   reverseQ = opts.getBoolean("reverse");
}



//////////////////////////////
//
// example -- gives example calls to the midiplay program.
//

void example(void) {
   cout <<
   "                                                                         \n"
   << endl;
}



//////////////////////////////
//
// usage -- how to run the textmidi program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   << endl;
}



