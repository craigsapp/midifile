//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb 10 21:57:42 PST 2015
// Last Modified: Tue Feb 10 21:57:45 PST 2015
// Filename:      midifile/src-programs/shutak.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Convert lines of MIDI note numbers into MIDI files.
//                Multiple lines will be placed in multiple tracks.
//                Each note has the duration of one second (quarter notes
//                at MM60).
//
// Each row of input is a set of numbers for MIDI pitches that are
// played one per second:
//                77      74      81      69      81      62
//
// If there are more than one row, each row is played in parallel
// with the other:
//                66      81      63      79      64      62
//                75      68      66      76      81      66
//                63      64      75      68      77      73
// In this case the first three notes that are played together are:
//     (66, 75, 63)
//

#include "Options.h"
#include "MidiFile.h"
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Global variables for command-line options.
Options  options;            // for command-line processing

// function declarations:
void     checkOptions        (Options& opts);
void     createMidiFile      (const char* filename,
                              vector<vector<int> >& sequence);
void     processFile         (const string& filename);
void     getData             (vector<vector<int> >& sequence,
                              const string& filename);
void     example             (void);
void     usage               (const string& command);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   options.setOptions(argc, argv);
   checkOptions(options);
   string line;
   for (int i=1; i<=options.getArgCount(); i++) {
      processFile(options.getArg(i));
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// processFile -- Convert file into a MIDI file.
//

void processFile(const string& filename) {
   char buffer[1024] = {0};
   strncpy(buffer, filename.data(),1000);
   char* ptr = strrchr(buffer, '.');
   if (ptr != NULL) {
      *ptr = '\0';
   }
   strcat(buffer, ".mid");

   vector<vector<int> > sequence;
   getData(sequence, filename);

   createMidiFile(buffer, sequence);
}



//////////////////////////////
//
// createMidiFile --
//

void createMidiFile(const char* filename, vector<vector<int> >& sequence) {
   MidiFile midifile;
   midifile.absoluteTicks();
   midifile.addTrack(1);
   int tpq = 120;
   double beat = 0.0;
   midifile.setTicksPerQuarterNote(tpq);


   MidiEvent tempo;
   tempo.setMetaTempo(60.0);
   tempo.track = 0;
   tempo.tick = 0;
   midifile.addEvent(tempo);

   int maxlen = 0;
   int i, j;
   for (i=0; i<(int)sequence.size(); i++) {
      if ((int)sequence[i].size() > maxlen) {
         maxlen = sequence[i].size();
      }
   }

   vector<int> notelist;
   MidiEvent noteon(0x90, 0, 64);
   MidiEvent noteoff(0x80, 0, 64);
   noteon.track  = 1;
   noteoff.track = 1;

   for (i=0; i<maxlen; i++) {
      notelist.clear();
      for (j=0; j<(int)sequence.size(); j++) {
         if (i<(int)sequence[j].size()) {
            notelist.push_back(sequence[j][i]);
         }
      }
      for (j=0; j<(int)notelist.size(); j++) {
         noteon[1]  = 0x7f & notelist[j];
         noteoff[1] = 0x7f & notelist[j];
         noteon.tick  = (int)(beat * tpq + 0.5);
         noteoff.tick = (int)(beat * tpq + 1 * tpq + 0.5);
         midifile.addEvent(noteon);
         midifile.addEvent(noteoff);
      }
      beat += 1.0;
   }

   midifile.sortTracks();
   midifile.write(filename);
}



//////////////////////////////
//
// getData -- Extract MIDI key numbers from input file.
//

void getData(vector<vector<int> >& sequence, const string& filename) {
   ifstream infile;
   infile.open(filename.data());
   string line;
   int key;
   stringstream sss;
   sequence.clear();
   vector<int> list;
   while (getline(infile, line)) {
      cout << "LINE = " << line << "\n";
      sss.clear();
      list.clear();
      sss << line;
      while (sss >> key) {
         list.push_back(key);
      }
      if (list.size() == 0) {
         continue;
      }
      sequence.push_back(list);
   }
   infile.close();
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
      cout << "80off, version 1.0 (10 Feb 2015)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (opts.getArgCount() == 0) {
      cerr << "Error: One or more input file is required." << endl;
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



