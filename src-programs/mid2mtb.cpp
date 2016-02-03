//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb  2 15:06:12 PST 2016
// Last Modified: Tue Feb  2 15:06:15 PST 2016
// Filename:      ...sig/examples/all/mid2mtb.cpp
// Web Address:   http://midifile.sapp.org/src-programs//mid2mtb.cpp
// Syntax:        C++; museinfo
//
// Description:   Converts a MIDI file into a MIDI Toolbox compatible
//                text file.
//

#include "MidiFile.h"
#include "Options.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;

typedef unsigned char uchar;

// user interface variables
Options options;
vector<vector<double> > matlabarray;

// function declarations:
void      convertMidiFile       (MidiFile& midifile,
                                 vector<vector<double> >& matlab);
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      usage                 (const char* command);
void      printEvent            (vector<double>& event);
void      printLegend           (MidiFile& midifile);
void      printMatlabArray      (MidiFile& midifile,
                                 vector<vector<double> >& matlab);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   matlabarray.reserve(100000);
   matlabarray.clear();
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   convertMidiFile(midifile, matlabarray);
   printMatlabArray(midifile, matlabarray);
   return 0;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// convertMidiFile --
//

void convertMidiFile(MidiFile& midifile, vector<vector<double> >& matlab) {
   midifile.absoluteTicks();
   midifile.linkNotePairs();
   midifile.joinTracks();
   midifile.doTimeAnalysis();

   vector<double> parameters(8);
   // 1: beat on time
   // 2: beat duration
   // 3: channel
   // 4: pitch #
   // 5: velocity
   // 6: start time (seconds)
   // 7: duration (seconds)
   // 8: track

   double tpq = midifile.getTicksPerQuarterNote();
   double beatstart;
   double beatdur;
   double starttime;
   double duration;
   double channel;
   double key;
   double velocity;
   double track;
   for (int i=0; i<midifile[0].size(); i++) {
      if (!midifile[0][i].isNoteOn()) {
         continue;
      }
      beatstart = midifile[0][i].tick / tpq;
      beatdur = midifile[0][i].getTickDuration() / tpq;
      starttime = midifile.getTimeInSeconds(0, i);
      duration = midifile[0][i].getDurationInSeconds();
      channel = midifile[0][i].getChannelNibble() + 1;
      key = midifile[0][i][1];
      velocity = midifile[0][i][2];
      track = midifile[0][i].track + 1;
      parameters[0] = beatstart;
      parameters[1] = beatdur;
      parameters[2] = channel;
      parameters[3] = key;
      parameters[4] = velocity;
      parameters[5] = starttime;
      parameters[6] = duration;
      parameters[7] = track;
      matlab.push_back(parameters);
   }
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

   opts.define("debug=b",  "debug mode to find errors in input file");
   opts.define("max=i:100000", "maximum number of notes expected in input");

   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 22 Jan 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 12 Nov 2003" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().data());
      exit(1);
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
   cout << "Usage: " << command << " midifile" << endl;
}



void printLegend(MidiFile& midifile) {
   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   cout << "%% DATA LEGEND                                               %%\n";
   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   cout << "%%Filename: " << midifile.getFilename() << endl;
   cout << "%%Ticks per quarter note: " << midifile.getTicksPerQuarterNote()
        << "\n";
   cout << "%%" << endl;
   cout << "%% Meaning of columns:" << endl;
   cout << "%%(1) note start in beats (quarter notes)." << endl;
   cout << "%%(2) note duration in beats (quarter notes)." << endl;
   cout << "%%(3) MIDI channel (indexed from 1)." << endl;
   cout << "%%(4) MIDI pitch (60 = C4)" << endl;
   cout << "%%(5) MIDI velocity (60 = C4)" << endl;
   cout << "%%(6) note start in seconds." << endl;
   cout << "%%(7) note duration in seconds." << endl;
   cout << "%%(8) MIDI file track." << endl;
   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
}



//////////////////////////////
//
// printMatlabArray -- print the Matlab array representing the MIDI file.
//    If not in time order, see sourcecode for mid2mat.cpp for sorting function.
//

void printMatlabArray(MidiFile& midifile, vector<vector<double> >& matlab) {
   int i;
   printLegend(midifile);
   for (i=0; i<(int)matlab.size(); i++) {
      printEvent(matlab[i]);
   }
}



//////////////////////////////
//
// printEvent -- print the event
//

void printEvent(vector<double>& event) {
   for (int i=0; i<(int)event.size(); i++) {
      cout << event[i];
      if (i<(int)event.size()-1) {
         cout << "\t";
      }
   }
   cout << "\n";
}



