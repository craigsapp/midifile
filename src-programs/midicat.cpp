//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Oct 16 07:34:30 PDT 2012
// Last Modified: Mon Feb  9 20:40:50 PST 2015 Updated for C++11.
// Filename:      ...sig/examples/all/midicat.cpp
// Web Address:   http://museinfo.sapp.org/examples/museinfo/midi/midicat.cpp
// Syntax:        C++; museinfo
//
// Description:   Concatenate multiple MIDI files into single type-0 MIDI file.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <vector>

using namespace std;

// user interface variables
Options options;
double seconds         = 2.0;  // used with -p option
int    binaryQ         = 1;    // used with -a option

// function declarations:
void      checkOptions      (Options& opts, int argc, char** argv);
void      example           (void);
void      usage             (const char* command);
void      appendMidi        (MidiFile& outfile, const char* filename,
                             double seconds, int initQ);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile outfile;
   outfile.joinTracks();
   outfile.deltaTicks();

   int i;
   int initQ = 0;
   for (i=1; i<=options.getArgCount(); i++) {
      appendMidi(outfile, options.getArg(i).data(), seconds, initQ++);
   }

   // insert an end-of track Meta Event
   int tpq = outfile.getTicksPerQuarterNote();
   MidiEvent mfevent;
   mfevent.tick = tpq;
   mfevent.track = 0;
   mfevent.resize(3);
   mfevent[0] = 0xff;
   mfevent[1] = 0x2f;
   mfevent[2] = 0;
   outfile.addEvent(mfevent);

   if (binaryQ) {
      outfile.write(cout);
   } else {
      cout << outfile;
   }

   return 0;
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// appendMidi --
//

void appendMidi(MidiFile& outfile, const char* filename,
      double seconds, int initQ) {
   MidiFile infile(filename);
   infile.joinTracks();
   infile.deltaTicks();
   int i;
   int tpq;
   int count;

   MidiEvent anevent;
   if (initQ == 0) {
      outfile.joinTracks();
      count = infile.getEventCount(0);
      // don't include end-of-track meta event
      count--;
      tpq = infile.getTicksPerQuarterNote();
      outfile.setTicksPerQuarterNote(tpq);
      for (i=0; i<count; i++) {
         anevent = infile.getEvent(0,i);
         // storing as a type-0 file, so remove track information
         anevent.track = 0;
         outfile.addEvent(anevent);
      }
      return;
   }

   // presuming constant tpq for different files.
   tpq = outfile.getTicksPerQuarterNote();

   if (seconds > 0.0) {
      // insert a tempo marking of 120, and then a pause related to how
      // long in second to wait until next MIDI file contents.
      // micro-seconds per quarter note is last three bytes of meta message
      // If quarter note is 120 bpm, that is 0.5 seconds or 500000 usec.
      // In hex 500000 is 07 a1 20
      // Tempo meta event:  ff 51 03 07 a1 20
      vector<uchar> bpm120;
      bpm120.resize(3);
      bpm120[0] = 0x07;
      bpm120[1] = 0xa1;
      bpm120[2] = 0x20;

      outfile.addMetaEvent(0, 0, 0x51, bpm120);
      infile.getEvent(0,0).tick = int(seconds * 2 * tpq + 0.5);
   }

   count = infile.getEventCount(0);
   // don't include end-of-track meta event
   count--;
   for (i=0; i<count; i++) {
      anevent = infile.getEvent(0,i);
      anevent.track = 0;
      outfile.addEvent(anevent);
   }
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("p|pause=d:2.0",  "Pause given number of secs after each file");
   opts.define("a|ascii=b",  "Display MIDI output as ASCII text");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 16 October 2012" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: October 2012" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (opts.getArgCount() <= 1) {
      usage(opts.getCommand().data());
      exit(1);
   }

   seconds     =  opts.getDouble("pause");
   binaryQ     = !opts.getBoolean("ascii");
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

}



