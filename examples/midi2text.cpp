//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jan 22 22:09:46 PST 2002
// Last Modified: Tue Jan 22 22:09:48 PST 2002
// Filename:      ...sig/examples/all/midi2text.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/midi2text.cpp
// Syntax:        C++; museinfo
//
// Description:   Description: Converts a MIDI file into a text based notelist.
//

#include "MidiFile.h"
#include "Options.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char uchar;


// user interface variables
Options options;
int     debugQ = 0;             // use with --debug option
int     maxcount = 100000;
double  tempo = 60.0;

// function declarations:
void      convertMidiFileToText (MidiFile& midifile);
void      setTempo              (MidiFile& midifile, int index, double& tempo);
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      usage                 (const char* command);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   convertMidiFileToText(midifile);
   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// convertMidiFileToText --
//

void convertMidiFileToText(MidiFile& midifile) {
   midifile.absoluteTime();
   midifile.joinTracks();

   Array<double> ontimes(128);
   Array<int> onvelocities(128);
   int i;
   for (i=0; i<128; i++) {
      ontimes[i] = -1.0;
      onvelocities[i] = -1;
   }

   double offtime = 0.0;

   int key = 0;
   int vel = 0;
   int command = 0;

   for (i=0; i<midifile.getNumEvents(0); i++) {
      command = midifile.getEvent(0, i).data[0] & 0xf0;
      if (command == 0x90 && midifile.getEvent(0, i).data[2] != 0) {
         // store note-on velocity and time
         key = midifile.getEvent(0, i).data[1];
         vel = midifile.getEvent(0, i).data[2];
         ontimes[key] = midifile.getEvent(0, i).time * 60.0 / tempo / 
               midifile.getTicksPerQuarterNote();
         onvelocities[key] = vel;
      } else if (command == 0x90 || command == 0x80) {
         // note off command write to output
         key = midifile.getEvent(0, i).data[1];
         offtime = midifile.getEvent(0, i).time * 60.0 /
               midifile.getTicksPerQuarterNote() / tempo;
         cout << "note\t" << ontimes[key] 
              << "\t" << offtime - ontimes[key]
              << "\t" << key << "\t" << onvelocities[key] << endl;
         onvelocities[key] = -1;
         ontimes[key] = -1.0;
      } 

      // check for tempo indication
      if (midifile.getEvent(0, i).data[0] == 0xff &&
                 midifile.getEvent(0, i).data[1] == 0x51) {
         setTempo(midifile, i, tempo);
      }
   }

}


//////////////////////////////
//
// setTempo -- set the current tempo
//

void setTempo(MidiFile& midifile, int index, double& tempo) {
   double newtempo = 0.0;
   static int count = 0;
   count++;

   MFEvent& mididata = midifile.getEvent(0, index);

   int microseconds = 0;
   microseconds = microseconds | (mididata.data[3] << 16);
   microseconds = microseconds | (mididata.data[4] << 8);
   microseconds = microseconds | (mididata.data[5] << 0);

   newtempo = 60.0 / microseconds * 1000000.0;
   if (count <= 1) {
      tempo = newtempo;
   } else if (tempo != newtempo) {
      cout << "; WARNING: change of tempo from " << tempo 
           << " to " << newtempo << " ignored" << endl;
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
      cout << argv[0] << ", version: 22 Jan 2002" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   debugQ = opts.getBoolean("debug");
   maxcount = opts.getInteger("max"); 

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand());
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



// md5sum: 6228e3df0855cfa017d06dd9a0c498e4 midi2text.cpp [20110711]
