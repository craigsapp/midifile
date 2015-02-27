//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jan 22 16:46:19 PST 2002
// Last Modified: Fri Dec 13 22:27:44 PST 2002 (added channel option)
// Last Modified: Mon Feb  9 21:26:32 PST 2015 Updated for C++11.
// Filename:      ...sig/examples/all/text2midi.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/text2midi.cpp
// Syntax:        C++; museinfo
//
// Description:   Description: Converts a text based notelist into a
//                MIDI file.
//

#include "MidiFile.h"
#include "Options.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

typedef unsigned char uchar;

// user interface variables
Options options;
int     tpq = 480;              // ticks per quarter note
int     debugQ = 0;             // use with --debug option
int     maxcount = 100000;      // maxiumum number of notes expected
double  tempo = 120.0;          // time units will be in seconds
int     channel = 0;            // default channel

// function declarations:
void      convertTextToMidiFile (istream& textfile, MidiFile& midifile);
void      adjustbuffer          (char* buffer);
void      readvalues            (char* buffer, int& eventtype, double& start,
                                 double& dur, int& note, int& vel);
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      usage                 (const char* command);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);

   fstream textfile(options.getArg(1).data(), ios::in);
   if (!textfile.is_open()) {
      cout << "Error: cannot read input text file." << endl;
      usage(options.getCommand().data());
      exit(1);
   }
   MidiFile midifile;
   convertTextToMidiFile(textfile, midifile);


   midifile.sortTracks();
   midifile.write(options.getArg(2));

   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// convertTextToMidiFile --
//

void convertTextToMidiFile(istream& textfile, MidiFile& midifile) {
   vector<uchar> mididata;
   midifile.setTicksPerQuarterNote(tpq);
   midifile.absoluteTicks();
   midifile.allocateEvents(0, 2 * maxcount + 500);  // pre allocate space for
                                                    // max expected MIDI events

   // write the tempo to the midifile
   mididata.resize(6);
   mididata[0] = 0xff;      // meta message
   mididata[1] = 0x51;      // tempo change
   mididata[2] = 0x03;      // three bytes to follow
   int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
   mididata[3] = (microseconds >> 16) & 0xff;
   mididata[4] = (microseconds >> 8)  & 0xff;
   mididata[5] = (microseconds >> 0)  & 0xff;
   midifile.addEvent(0, 0, mididata);

   char buffer[1024] = {0};
   int line = 1;
   double start = 0.0;
   double dur = 0.0;
   int ontick = 0;
   int offtick = 0;
   int note = 0;
   int vel = 0;
   int eventtype = 0;

   while (!textfile.eof()) {
      textfile.getline(buffer, 1000, '\n');
      if (textfile.eof()) {
         break;
      }
      adjustbuffer(buffer);
      if (debugQ) {
         cout << "line " << line << ":\t" << buffer << endl;
      }
      readvalues(buffer, eventtype, start, dur, note, vel);
      if (eventtype != 1) {
         continue;
      }

      // have a good note, so store it in the MIDI file
      ontick  = (int)(start * tpq * 2.0 + 0.5);
      offtick = (int)((start + dur) * tpq * 2.0 + 0.5);
      if (offtick <= ontick) {
         offtick = ontick + 1;
      }
      if (debugQ) {
         cout << "Note ontick=" << ontick << "\tofftick=" << offtick
              << "\tnote=" << note << "\tvel=" << vel << endl;
      }

      mididata.resize(3);
      mididata[0] = 0x90 | channel;       // note on command
      mididata[1] = (uchar)(note & 0x7f);
      mididata[2] = (uchar)(vel & 0x7f);
      midifile.addEvent(0, ontick, mididata);
      mididata[0] = 0x80 | channel;       // note off command
      midifile.addEvent(0, offtick, mididata);

      line++;
   }
}



//////////////////////////////
//
// readvalues -- read parameter values from the input dataline.
//     returns 0 if no parameters were readable.
//

void readvalues(char* buffer, int& eventtype, double& start, double& dur,
   int& note, int& vel) {
   char *ptr = NULL;
   ptr = strtok(buffer, " \t\n");
   if (ptr == NULL) {
      eventtype = 0;
      return;
   }

   if (strcmp(ptr, "note") != 0) {
      eventtype = 0;
      return;
   } else {
      eventtype = 1;
   }

   // read the starttime
   ptr = strtok(NULL, " \t\n");
   if (ptr == NULL) {
      eventtype = 0;
      return;
   }
   start = atof(ptr);

   // read the duration
   ptr = strtok(NULL, " \t\n");
   if (ptr == NULL) {
      eventtype = 0;
      return;
   }
   dur = atof(ptr);

   // read the note number
   ptr = strtok(NULL, " \t\n");
   if (ptr == NULL) {
      eventtype = 0;
      return;
   }
   note = strtol(ptr, NULL, 10);
   if (note < 0 || note > 127) {
      eventtype = 0;
      return;
   }

   // read the starttime
   ptr = strtok(NULL, " \t\n");
   if (ptr == NULL) {
      eventtype = 0;
      return;
   }
   vel = strtol(ptr, NULL, 10);
   if (vel < 0 || vel > 127) {
      eventtype = 0;
      return;
   }

   eventtype = 1;

}



//////////////////////////////
//
// adjustbuffer -- remove comments and make lower characters
//

void adjustbuffer(char* buffer) {
   int i = 0;
   while (buffer[i] != '\0') {
      buffer[i] = tolower(buffer[i]);
      if (buffer[i] == ';') {
         buffer[i] = '\0';
         return;
      }
      i++;
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

   opts.define("c|channel=i:0","MIDI Channel to play notes on (offset from 0)");
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
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   debugQ   = opts.getBoolean("debug");
   maxcount = opts.getInteger("max");
   channel  = opts.getInteger("channel");
   if (channel < 0) {
      channel = 0;
   } if (channel > 15) {
      channel = 15;
   }

   if (opts.getArgCount() != 2) {
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
   cout << "Usage: " << command << " textfile midifile" << endl;
}




