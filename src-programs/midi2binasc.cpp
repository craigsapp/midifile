//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Apr  3 23:43:56 PDT 2013
// Last Modified: Thu Apr  4 01:39:09 PDT 2013
// Last Modified: Mon Feb  9 21:26:32 PST 2015 Updated for C++11.
// Filename:      ...sig/examples/all/midi2binasc.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/midi2binasc.cpp
// Syntax:        C++; museinfo
//
// Description:   Converts a MIDI file into an ASCII format which can be
//                converted back into a MIDI file with the binasc program:
//                   http://binasc.googlecode.com
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;

typedef unsigned char uchar;

void    convertMidiFile         (MidiFile& midifile);
void    printMidiHeader         (MidiFile& midifile);
void    checkOptions            (Options& opts, int argc, char* argv[]);
void    printTrack              (MidiFile& midifile, int track);
int     getVlvSize              (int value);
int     getTrackByteCount       (MidiFile& midifile, int track);
void    printDecByte            (int value);
void    printMidiEvent          (MidiEvent& event);
void    printHexByte            (int value);
void    usage                   (const char* command);
void    example                 (void);


// user interface variables
Options options;
int     debugQ   = 0;           // use with --debug option
int     type0Q   = 0;           // force MIDI file to type 0 (single track)

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   convertMidiFile(midifile);
   return 0;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// convertMidiFile --
//

void convertMidiFile(MidiFile& midifile) {
   if (type0Q) {
      midifile.joinTracks();
   }
   midifile.deltaTicks();

   printMidiHeader(midifile);
   int trackcount = midifile.getTrackCount();
   for (int i=0; i<trackcount; i++) {
      printTrack(midifile, i);
   }
}



//////////////////////////////
//
// printTrack --
//

void printTrack(MidiFile& midifile, int track) {
   cout << endl;

   // first print track header
   cout << "+M +T +r +k\t\t\t; Track chunk marker" << endl;
   // print number of bytes in track
   int trackbytes = getTrackByteCount(midifile, track);
   cout << "4'" << trackbytes << "\t\t\t\t; number of bytes to follow in track"
        << endl;

   cout << endl;

   // print the list of events in the track
   MidiEvent event;
   int eventcount = midifile.getEventCount(track);
   for (int i=0; i<eventcount; i++) {
      event = midifile.getEvent(track, i);
      printMidiEvent(event);
   }
}



//////////////////////////////
//
// printMidiEvent -- print a time/MIDI message pair.
//

void printMidiEvent(MidiEvent& event) {
   // print the time:
   cout << "v" << event.tick << "\t";

   // print the command byte in hex format (two digits):
   int commandbyte = event[0];
   printHexByte(commandbyte);
   int i;

   switch (commandbyte & 0xf0) {
   case 0x90:
   case 0x80:
      for (i=1; i<(int)event.size(); i++) {
         cout << " ";
         printDecByte(event[i]);
      }
      break;
   default:
      for (i=1; i<(int)event.size(); i++) {
         cout << " ";
         printHexByte(event[i]);
      }
   }

   cout << endl;
}



//////////////////////////////
//
// printDecByte --
//

void printDecByte(int value) {
   cout << "'" << value;
}



//////////////////////////////
//
// printHexByte --
//

void printHexByte(int value) {
   if (value < 16) {
      cout << "0";
   } else if (value > 255) {
      cerr << "ERROR: value is too large: " << value << endl;
      exit(1);
   }
   cout << hex << value << dec;
}



//////////////////////////////
//
// getTrackByteCount -- number of by the track after the track header.
//    Might need to keep track of End-of-track meta message.
//

int getTrackByteCount(MidiFile& midifile, int track) {
   int sum = 0;
   int i;
   int eventcount = midifile.getEventCount(track);
   MidiEvent event;

   for (i=0; i<eventcount; i++) {
      event = midifile.getEvent(track, i);
      sum += getVlvSize(event.tick);
      sum += event.size();
   }
   return sum;
}



//////////////////////////////
//
// getVlvSize -- return the number of bytes in the VLV format for the
//    integer.
//

int getVlvSize(int value) {
   if (value < 0x80) {
      return 1;
   } if (value < 0x4000) {
      return 2;
   } else if (value < 0x200000) {
      return 3;
   } else if (value < 0x10000000) {
      return 4;
   } else {
      return 5;
   }
}



//////////////////////////////
//
// printMidiHeader --
//
// Example header:
//
// +M +T +h +d             ; MIDI file header chunk marker
// 4'6                     ; bytes in header to follow
// 2'0                     ; format: Type-0 (single track)
// 2'1                     ; track count: 1 track
// '-25 '40                ; divisions: SMPTE: 25 frames/sec 40 subframes/frame
//                         ;    in other words: 25 * 40 = 1000 ticks per second.
//

void printMidiHeader(MidiFile& midifile) {
   // print MIDI file header marker
   cout << "+M +T +h +d\t\t\t; MIDI file header chunk marker" << endl;
   // print the number of bytes in the MIDI file to follow (always 6):
   cout << "4'6\t\t\t\t; bytes in header to follow" << endl;
   // print the format (0 = single track, 1 = multitrack)
   // The MidiFile class does not exactly keep track of this value.
   // It will presume that a single track file is a type-0 MIDI file
   // (type-1 MIDI files can theoretically have a single track, but not
   // usually).
   if (midifile.getTrackCount() == 0) {
      cout << "2'0\t\t\t\t; format: Type-0 (single track)";
   } else {
      cout << "2'1\t\t\t\t; format: Type-1 (multi-track)";
   }
   cout << endl;
   // print track count
   cout << "2'" << midifile.getTrackCount();
   cout << "\t\t\t\t; track count: ";
   cout << midifile.getTrackCount() << " track";
   if (midifile.getTrackCount() != 1) {
      cout << "s";
   }
   cout << endl;

   // print the ticks per quarter note.  The ticks per quarter note
   // can be SMPTE or regular.  Assuming regular at the moment.
   int ticks = midifile.getTicksPerQuarterNote();
   cout << "2'" << ticks << "\t\t\t\t; ticks per quarter note" << endl;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("debug=b",  "debug mode to find errors in input file");

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

   debugQ   = opts.getBoolean("debug");

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



