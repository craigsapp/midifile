//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Apr  6 13:54:09 PDT 2009
// Last Modified: Mon Apr  6 13:54:12 PDT 2009
// Filename:      peep2midi.cpp
// Syntax:        C++; museinfo
//
// Description:   Convert Performance Expression Extraction Program
//                output data into MIDI data.
//

#include "MidiFile.h"
#include "humdrum.h"
#include "Options.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char uchar;

// user interface variables
Options options;
int     debugQ = 0;             // use with --debug option
int     maxcount = 100000;
int     mindyn = 30;            // use with -r option
int     maxdyn = 120;           // use with -r option
double  duration = 0.1;         // use with -d option
const char* filename = "";      // use with -o option


// function declarations:
void      checkOptions          (Options& opts, int argc, char** argv);
void      createMidiFile        (MidiFile& midifile, HumdrumFile& infile);
int       getMIDIKeyNum         (const char* string);
int       getTrackNumber        (const char* string);
void      example               (void);
void      usage                 (const char* command);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   HumdrumFile infile;
   infile.read(options.getArg(1));
   MidiFile midifile;
   createMidiFile(midifile, infile);

   if (strcmp(filename, "") == 0) {
      cout << midifile;
   } else {
      midifile.write(filename);
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// createMidiFile --
//   input humdrum file data should be in simple columns in this order:
//
//  **data	**data	**data	**data	**data
//  1.667	18.8	F3	-2	1
//  1.666	19	C4	-1	1
//  1.667	10.7	a4	-2	1
//  1.667	19.2	c5	-2	1
//  2.009	24	F3	+10	1
//  2.01	24.2	C4	+10	1
//
// Column one is the time in seconds at which the note is started
// Column two is a dynamic value (the range of which will be normalized)
// Column three contains the pitch information.
//

void createMidiFile(MidiFile& midifile, HumdrumFile& infile) {

   Array<int>    millitimes;
   Array<double> velocities;
   Array<int>    keynum;
   Array<int>    track;

   millitimes.setSize(infile.getNumLines());
   velocities.setSize(infile.getNumLines());
   keynum.setSize(infile.getNumLines());
   track.setSize(infile.getNumLines());

   millitimes.setSize(0);
   velocities.setSize(0);
   keynum.setSize(0);
   track.setSize(0);

   int    intval;
   double floatval;
   double dmax = -100000;
   double dmin = +100000;

   int i;
   for (i=0; i<infile.getNumLines(); i++) {
      if (!infile[i].isData()) {
         continue;
      }
      sscanf(infile[i][0], "%lf", &floatval);
      intval = int(floatval * 1000.0 + 0.5);
      millitimes.append(intval);

      sscanf(infile[i][1], "%lf", &floatval);
      velocities.append(floatval);
      if (floatval < dmin) { dmin = floatval; }
      if (floatval > dmax) { dmax = floatval; }

      intval = getMIDIKeyNum(infile[i][2]);
      keynum.append(intval);

      intval = getTrackNumber(infile[i][2]);
      track.append(intval);
   }
   millitimes.allowGrowth(0);
   velocities.allowGrowth(0);
   keynum.allowGrowth(0);
   track.allowGrowth(0);

   // normalize the dynamics data into the range from 0 to 1
   double diff = dmax - dmin;
   for (i=0; i<velocities.getSize(); i++) {
      if (diff > 0.0) {
         velocities[i] = (velocities[i] - dmin) / diff;
      } else {
         velocities[i] = 0.5;
      }
   }


   // now ready to write the data to the MIDI file:

   midifile.setMillisecondDelta();   // SMPTE 25 frames & 40 subframes
   midifile.absoluteTime();          // Time values inserted are absolute
   midifile.addTrack(2);             // Right and Left hands

   Array<uchar> event;
   event.setSize(3);

   int intdur  = int(duration * 1000.0 + 0.5);
   int lasttime = 0;
   int dyndiff = maxdyn - mindyn;
   int vel;
   for (i=0; i<millitimes.getSize(); i++) {
      if ((keynum[i] <= 10) || (keynum[i] > 127)) {
         continue;
      }
      vel = int(velocities[i] * dyndiff + mindyn + 0.5);
      if (vel < 1) { vel = 1; }
      if (vel > 127) { vel = 127; }

      event[0] = 0x90; // note-on
      event[1] = keynum[i];
      event[2] = vel;
      midifile.addEvent(track[i], millitimes[i], event);
      event[2] = 0;
      lasttime = millitimes[i] + intdur;
      midifile.addEvent(track[i], lasttime, event);
   }

   // write the end of track marker
   event[0] = 0xff;
   event[1] = 0x2f;
   event[2] = 0;
   for (i=0; i<midifile.getTrackCount(); i++) {
	
      if (i>0) {
         // have to lengthen the last note in track due to bugs
         // in various MIDI playback programs which clip
         // the last chord of a file
         midifile.getEvent(i, midifile.getNumEvents(i)-1).time += 1500;
      }
      midifile.addEvent(i, lasttime+2000, event);
   }

   // add comments from header
   for (i=0; i<infile.getNumLines() && i<lasttime; i++) {
      if (infile[i].isBibliographic() || infile[i].isGlobalComment()) {
         // 0x01 is a text event
         midifile.addMetaEvent(0, i, 0x01, infile[i].getLine());
      }
   }

   // sort the ontimes and offtimes so they are in correct time order:
   midifile.sortTracks();

}



//////////////////////////////
//
// getTrackNumber -- lowercase pitch name = right hand; uppercase = left hand
//

int getTrackNumber(const char* string) {
   if (islower(string[0])) {
      return 1;
   } else {
      return 2;
   }
}



//////////////////////////////
//
// getMIDIKeyNum --
//

int getMIDIKeyNum(const char* string) {
   int accid = 0;
   int octave = -1;
   int len = strlen(string);
   int diatonic = -1;

   switch (tolower(string[0])) {
      case 'c': diatonic = 0;  break;
      case 'd': diatonic = 2;  break;
      case 'e': diatonic = 4;  break;
      case 'f': diatonic = 5;  break;
      case 'g': diatonic = 7;  break;
      case 'a': diatonic = 9;  break;
      case 'b': diatonic = 11; break;
   }

   if (diatonic < 0) {
      return -1;
   }

   int i;
   for (i=0; i<len; i++) {
      if (string[i] == '#') {
         accid++;
      } else if (string[i] == '-') {
         accid--;
      }
      if (isdigit(string[i]) && (octave < 0)) {
         sscanf(&(string[i]), "%d", &octave);
      }
   }

   if (octave < 0) {
      return -1;
   }

   return diatonic + octave * 12 + accid;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("o|output=s",  "output midi file name");
   opts.define("r|range=s",   "dynamics range");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");

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

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand());
      exit(1);
   }

   filename = opts.getString("output");
   int count;
   if (opts.getBoolean("range")) {
      count = sscanf(opts.getString("range"), "%d-%d", &mindyn, &maxdyn);
      if (count != 2) {
         count = sscanf(opts.getString("range"), "%d:%d", &mindyn, &maxdyn);
      }
      if (count != 2) {
         count = sscanf(opts.getString("range"), "%d,%d", &mindyn, &maxdyn);
      }
      if (count != 2) {
         count = sscanf(opts.getString("range"), "%d, %d", &mindyn, &maxdyn);
      }
      if (count != 2) {
         count = sscanf(opts.getString("range"), "%d %d", &mindyn, &maxdyn);
      }
   }

   if (mindyn > maxdyn) {
      int temp = mindyn;
      mindyn = maxdyn;
      maxdyn = temp;
   }

   if (mindyn == maxdyn) {
      mindyn = 20;
      maxdyn = 120;
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



