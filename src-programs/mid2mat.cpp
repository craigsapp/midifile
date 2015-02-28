//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jan 22 22:09:46 PST 2002
// Last Modified: Thu Nov 20 17:09:08 PST 2003
// Last Modified: Mon Sep  6 23:21:17 PDT 2004 Changed pow(2) to pow(2.0).
// Last Modified: Fri Jul 23 12:32:31 PDT 2010 Generalized tempo parsing.
// Last Modified: Mon Feb  9 21:17:36 PST 2015 Updated for C++11.
// Filename:      ...sig/examples/all/mid2mat.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/mid2mat.cpp
// Syntax:        C++; museinfo
//
// Description:   Converts a MIDI file into a text-based note matrix.
//
// directives:
//    note    = note data
//    tempo   = tempo change
//    control = continuous controller
//    tpq     = ticks per quarter note
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

// Four types of event time display:
#define TICK 1            /* time units are MIDI file ticks (absolute)     */
#define BEAT 2            /* time units are Beats (quarter note, absolute) */
#define SEC  3            /* time units are seconds (absolute)             */
#define MSEC 4            /* time units are millisecodns (absolute)        */

// Data which can be found in the final data array:
#define OP_NOTE    1000   /* Note Event                  */
#define OP_CONTROL 2000   /* Continuous-Controller Event */
#define OP_INSTR   3000   /* Instrument Change Event     */
#define OP_TEMPO   4000   /* Tempo Meta Event            */
#define OP_METER   5000   /* Meter Meta Event            */
#define OP_KEYSIG  6000   /* Key Signature Event         */

#define OP_NOTE_NAME    "NOTE"
#define OP_CONTROL_NAME "CONT"
#define OP_CONT_NAME    "CONT"
#define OP_INSTR_NAME   "INSTR"
#define OP_TEMPO_NAME   "TEMPO"
#define OP_METER_NAME   "METER"
#define OP_KEYSIG_NAME  "KEYSIG"


// General MIDI instrument names:
const char *GMinstrument[128] = {
   "acpiano",   "britepno",  "synpiano",  "honkytonk", "epiano1",   "epiano2",
   "hrpschrd",  "clavinet",  "celeste",   "glocken",   "musicbox",  "vibes",
   "marimba",   "xylophon",  "tubebell",  "santur",    "homeorg",   "percorg",
   "rockorg",   "churchorg", "reedorg",   "accordn",   "harmonica", "concrtna",
   "nyguitar",  "acguitar",  "jazzgtr",   "cleangtr",  "mutegtr",   "odguitar",
   "distgtr",   "gtrharm",   "acbass",    "fngrbass",  "pickbass",  "fretless",
   "slapbas1",  "slapbas2",  "synbass1",  "synbass2",  "violin",    "viola",
   "cello",     "contraba",  "marcato",   "pizzcato",  "harp",      "timpani",
   "marcato",   "slowstr",   "synstr1",   "synstr2",   "choir",     "doo",
   "voices",    "orchhit",   "trumpet",   "trombone",  "tuba",      "mutetrum",
   "frenchorn", "hitbrass",  "synbras1",  "synbras2",  "sprnosax",  "altosax",
   "tenorsax",  "barisax",   "oboe",      "englhorn",  "bassoon",   "clarinet",
   "piccolo",   "flute",     "recorder",  "woodflut",  "bottle",    "shakazul",
   "whistle",   "ocarina",   "sqrwave",   "sawwave",   "calliope",  "chiflead",
   "charang",   "voxlead",   "lead5th",   "basslead",  "fantasia",  "warmpad",
   "polysyn",   "ghostie",   "bowglass",  "metalpad",  "halopad",   "sweeper",
   "aurora",    "soundtrk",  "crystal",   "atmosphr",  "freshair",  "unicorn",
   "sweeper",   "startrak",  "sitar",     "banjo",     "shamisen",  "koto",
   "kalimba",   "bagpipes",  "fiddle",    "shannai",   "carillon",  "agogo",
   "steeldrum", "woodblock", "taiko",     "toms",      "syntom",    "revcymb",
   "fx-fret",   "fx-blow",   "seashore",  "jungle",    "telephone", "helicptr",
   "applause",  "ringwhsl"
};

vector<int> legend_instr;
vector<int> legend_opcode;
vector<int> legend_controller;

typedef unsigned char uchar;

// user interface variables
Options options;
int     debugQ   = 0;           // use with --debug option
int     verboseQ = 0;           // used with -v option
int     tickQ    = 0;           // used with -t option
int     beatQ    = 0;           // used with -b option
int     secQ     = 0;           // used with -s option
int     msecQ    = 0;           // used with -m option
double  unused   = -1000.0;     // used with -u option
char    arrayname[1024] = {0};  // used with -n option
int     timetype = SEC;
int     numQ     = 0;
double  tempo    = 60.0;
int     maxcount = 100000;
vector<vector<double> > matlabarray;

// function declarations:
void      convertMidiFile       (MidiFile& midifile,
                                 vector<vector<double> >& matlab);
//void    setTempo              (MidiFile& midifile, int index, double& tempo);
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      usage                 (const char* command);
double    getTime               (int ticks, MidiFile& midifile);
void      processMetaEvent      (MidiFile& midifile, int i,
                                 vector<double>& event);
void      printEvent            (vector<double>& event);
void      printLegend           (MidiFile& midifile);
void      printMatlabArray      (MidiFile& midifile,
                                 vector<vector<double> >& matlab);
void      sortArray             (vector<vector<double> >& matlab);
int       eventcmp              (const void* a, const void* b);
void      printOpcodeVariables  (vector<int> opcodes);
void      printOpName           (int code);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   matlabarray.reserve(100000);
   matlabarray.clear();

   legend_instr.resize(128);
   legend_opcode.resize(128);
   legend_controller.resize(128);

   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));

   convertMidiFile(midifile, matlabarray);
   if (!verboseQ) {
      printMatlabArray(midifile, matlabarray);
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// printOpcodeVariables -- create variables for each type of
//  data found in the following array (for readability).
//

void printOpcodeVariables(vector<int> opcodes) {
   int i;
   for (i=0; i<(int)opcodes.size(); i++) {
      if (!opcodes[i]) {
         continue;
      }
      switch (i) {
         case 1:
            cout << OP_NOTE_NAME << " = "    << OP_NOTE    << ";" << endl;
            break;
         case 2:
            cout << OP_CONTROL_NAME << " = " << OP_CONTROL << ";" << endl;
            break;
         case 3:
            cout << OP_INSTR_NAME << " = "   << OP_INSTR   << ";" << endl;
            break;
         case 4:
            cout << OP_TEMPO_NAME << " = "   << OP_TEMPO   << ";" << endl;
            break;
         case 5:
            cout << OP_METER_NAME << " = "   << OP_METER   << ";" << endl;
            break;
         case 6:
            cout << OP_KEYSIG_NAME << " = "  << OP_KEYSIG  << ";" << endl;
            break;
      }
   }
}



//////////////////////////////
//
// convertMidiFile --
//

void convertMidiFile(MidiFile& midifile, vector<vector<double> >& matlab) {
   midifile.absoluteTicks();
   midifile.joinTracks();
   if (secQ || msecQ) {
      midifile.doTimeAnalysis();
   }
   vector<double> event(7);
   vector<double> ontimes(128);
   vector<int> onvelocities(128);
   int i;
   for (i=0; i<128; i++) {
      ontimes[i] = -1.0;
      onvelocities[i] = -1;
   }

   double offtime = 0.0;

   int key = 0;
   int vel = 0;
   int command = 0;

   if (verboseQ) {
      cout << "-1\ttpq\t" << midifile.getTicksPerQuarterNote() << endl;
   }

   for (i=0; i<midifile.getNumEvents(0); i++) {
      event.assign(event.size(), unused);
      command = midifile[0][i][0] & 0xf0;
      if (command == 0xf0) {
         command = midifile[0][i][0];
      }
      if (command == 0x90 && midifile[0][i][2] != 0) {
         // store note-on velocity and time
         key = midifile[0][i][1];
         vel = midifile[0][i][2];
         ontimes[key] = getTime(midifile[0][i].tick, midifile);

         onvelocities[key] = vel;
      } else if (command == 0x90 || command == 0x80) {
         // note off command write to output
         key = midifile[0][i][1];
         offtime = getTime(midifile[0][i].tick, midifile);
         legend_opcode[OP_NOTE/1000] = 1;

         if (verboseQ) {
            cout
              << ontimes[key]
              << "\tnote"
              << "\tdur=" << offtime - ontimes[key]
              << "\tpch=" << key
              << "\tvel=" << onvelocities[key]
              << "\tch="  << (midifile[0][i][0] & 0x0f)
              << "\ttrack=" << midifile[0][i].track
              << endl;
         } else {
            event[0] = ontimes[key];
            event[1] = OP_NOTE;
            event[2] = offtime - ontimes[key];
            event[3] = key;
            event[4] = onvelocities[key];
            event[5] = (midifile[0][i][0] & 0x0f);
            event[6] = midifile[0][i].track;
         }
      } else if (command == 0xb0) {
         legend_controller[midifile[0][i][1]] = 1;
         legend_opcode[OP_CONTROL/1000] = 1;

         if (verboseQ) {
            cout << getTime(midifile[0][i].tick, midifile)
                 << "\tcontrol"
                 << "\ttype="  << (int)midifile[0][i][1]
                 << "\tval="   << (int)midifile[0][i][2]
                 << "\tch="    << (midifile[0][i][0] & 0x0f)
                 << "\ttrack=" << midifile[0][i].track
                 << "\n";
         } else {
            event[0] = getTime(midifile[0][i].tick, midifile);
            event[1] = OP_CONTROL;
            event[2] = (int)midifile[0][i][1];
            event[3] = (int)midifile[0][i][2];
            event[5] = (midifile[0][i][0] & 0x0f);
            event[6] = midifile[0][i].track;
         }
      } else if (command == 0xc0) {
         legend_instr[midifile[0][i][1]] = 1;
         legend_opcode[OP_INSTR/1000] = 1;

         if (verboseQ) {
         cout << getTime(midifile[0][i].tick, midifile)
              << "\tinstr"
              << "\tname="  << GMinstrument[midifile[0][i][1]]
              << "\tnum="   << (int)midifile[0][i][1]
              << "\tch="    << (midifile[0][i][0] & 0x0f)
              << "\ttrack=" << midifile[0][i].track
              << "\n";
         } else {
            event[0] = getTime(midifile[0][i].tick, midifile);
            event[1] = OP_INSTR;
            event[2] = (int)midifile[0][i][1];
            event[5] = (midifile[0][i][0] & 0x0f);
            event[6] = midifile[0][i].track;
         }
      } else if (command == 0xff) {
         if (verboseQ) {
            cout << getTime(midifile[0][i].tick, midifile)
                 << "\t";
         } else {
            event[0] = getTime(midifile[0][i].tick, midifile);
         }
         processMetaEvent(midifile, i, event);
         if (verboseQ) {
            cout << "\n";
         }
      }

      /* no longer needed
      // check for tempo indication
      if (midifile[0][i][0] == 0xff &&
                 midifile[0][i][1] == 0x51) {
         setTempo(midifile, i, tempo);

      }
      */

      if (event[1] != unused) {
         matlab.push_back(event);
      }
   }

}



//////////////////////////////
//
// processMetaEvent -- Handle meta events.
//

void processMetaEvent(MidiFile& midifile, int i, vector<double>& event) {
   MidiEvent& mfevent = midifile[0][i];

   switch (mfevent[1]) {
      case 0x51:  // tempo change
         legend_opcode[OP_TEMPO/1000] = 1;
         event[1] = OP_TEMPO;
         event[2] = mfevent.getTempoBPM();
         break;

      case 0x58:  // time signature
         // 58 04 nn dd cc bb
         //  nn=numerator of time sig.
         //  dd=denominator of time sig. 2=quarter
         //  3=eighth, etc.
         //  cc=number of ticks in metronome click
         //  bb=number of 32nd notes to the quarter note
         if (verboseQ) {
            cout << "%meter\t" << (int)mfevent[2] << "/" << pow(2.0, mfevent[3]);
         } else {
            legend_opcode[OP_METER/1000] = 1;
            event[1] = OP_METER;
            event[2] = (int)mfevent[2];
            event[3] = pow(2.0, mfevent[3]);
         }
         break;

      case 0x59:  // key signature
         // 59 02 sf mi
         // sf=sharps/flats (-7=7 flats, 0=key of C, 7=7 sharps)
         // mi=major/minor (0=major, 1=minor)
         if (verboseQ) {
            cout << "%keysig\t";
            if (mfevent[3]==0) {
               switch (mfevent[2]) {
                  case 0: cout << "C-major"; break;
                  case 1: cout << "G-major"; break;
                  case 2: cout << "D-major"; break;
                  case 3: cout << "A-major"; break;
                  case 4: cout << "E-major"; break;
                  case 5: cout << "B-major"; break;
                  case 6: cout << "F-sharp-major"; break;
                  case 7: cout << "C-sharp-major"; break;
               }
            } else {
               switch (mfevent[2]) {
                  case 0: cout << "A-minor"; break;
                  case 1: cout << "E-minor"; break;
                  case 2: cout << "B-minor"; break;
                  case 3: cout << "F-minor"; break;
                  case 4: cout << "C-sharp-minor"; break;
                  case 5: cout << "G-sharp-minor"; break;
                  case 6: cout << "D-sharp-minor"; break;
                  case 7: cout << "A-sharp-minor"; break;
               }
            }
         } else {
            legend_opcode[OP_KEYSIG/1000] = 1;
            event[1] = OP_KEYSIG;
            event[2] = (int)mfevent[2];
            event[3] = (int)mfevent[3];
         }
         break;
      default:
         if (verboseQ) {
            cout << "%meta\t0x" << hex << (int)mfevent[1] << dec;
         }
   }
}



//////////////////////////////
//
// getTime -- return the time in command-line specified time unit
//

double getTime(int ticks, MidiFile& midifile) {
   int tpq = midifile.getTicksPerQuarterNote();
   switch (timetype) {
      case TICK:
         return ticks;
      case BEAT:
         return (double)ticks/tpq;
      case SEC:
         return midifile.getTimeInSeconds(ticks);
      case MSEC:
         return 1000 * midifile.getTimeInSeconds(ticks);
   }
   return 0.0;
}



/* obsolete function
//////////////////////////////
//
// setTempo -- set the current tempo
//

void setTempo(MidiFile& midifile, int index, double& tempo) {
   double newtempo = 0.0;
   static int count = 0;
   count++;
   vector<double> event;
   event.assign(7, unused);

   MidiEvent& mididata = midifile[0][index];

   int microseconds = 0;
   microseconds = microseconds | (mididata.data[3] << 16);
   microseconds = microseconds | (mididata.data[4] << 8);
   microseconds = microseconds | (mididata.data[5] << 0);

   newtempo = 60.0 / microseconds * 1000000.0;
   if (count <= 1) {
      tempo = newtempo;
   } else if (tempo != newtempo) {
      if (verboseQ) {
         cout << getTime(midifile[0][index].tick, midifile);
              << "\t"
              << "tempo\t" << newtempo << endl;
      } else {
         legend_opcode[OP_TEMPO/1000] = 1;
         event[0] = getTime(midifile[0][index].tick, midifile);
         event[1] = OP_TEMPO;
         event[2] = newtempo;
         matlabarray.push_back(event);
      }
   }
   tempo = newtempo;
}
*/



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("u|unused=d:-1000.0",             "unused parameter indicator");
   opts.define("n|name=s:data",                     "name for data array");
   opts.define("t|ticks|tick=b",                    "display time in ticks");
   opts.define("s|sec|second|seconds=b",            "display time in seconds");
   opts.define("m|msec|millisecond|milliseconds=b", "display time in msec");
   opts.define("b|beat|beats=b",                    "display time in beats");
   opts.define("num=b",                        "display opcodes as numbers");
   opts.define("v|verbose=b",                       "display verbose data");

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

   unused   = opts.getDouble("unused");
   debugQ   = opts.getBoolean("debug");
   maxcount = opts.getInteger("max");
   numQ     = opts.getBoolean("num");

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().data());
      exit(1);
   }

   tickQ = opts.getBoolean("ticks");
   secQ  = opts.getBoolean("seconds");
   msecQ = opts.getBoolean("milliseconds");
   beatQ = opts.getBoolean("beats");
   strcpy(arrayname, opts.getString("name").data());

   if (tickQ) {
      timetype = TICK;
      if (verboseQ) {
         cout << "-1\ttunit\tticks\n";
      }
   } else if (beatQ) {
      timetype = BEAT;
      if (verboseQ) {
         cout << "-1\ttunit\tbeat\n";
      }
   } else if (secQ) {
      timetype = SEC;
      if (verboseQ) {
         cout << "-1\ttunit\tseconds\n";
      }
   } else if (msecQ) {
      timetype = MSEC;
      if (verboseQ) {
         cout << "-1\ttunit\tmilliseconds\n";
      }
   } else {
      timetype = BEAT;
      if (verboseQ) {
         cout << "-1\ttunit\tseconds\n";
      }
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



//////////////////////////////
//
// printLegend -- print a legend of the codes in the data.
//

const char *GMcontrollers[128] = {
   "  0   Bank Select (coarse)               0..127",
   "  1   Modulation Wheel (coarse)          0..127",
   "  2   Breath Control (coarse)            0..127",
   "  3   Continuous controller #3           0..127",
   "  4   Foot Controller (coarse)           0..127",
   "  5   Portamento Time (coarse)           0..127",
   "  6   Data Entry Slider (coarse)         0..127",
   "  7   Main Volume (coarse)               0..127",
   "  8   Stereo Balance (coarse)            0..127",
   "  9   Continuous controller #9           0..127",
   " 10   Pan (coarse)                       0=left 127=right",
   " 11   Expression (sub-Volume) (coarse)   0..127",
   " 12   Effect Control 1 (coarse)          0..127",
   " 13   Effect Control 2 (coarse)          0..127",
   " 14   Continuous controller #14          0..127",
   " 15   Continuous controller #15          0..127",
   " 16   General Purpose Slider 1           0..127",
   " 17   General Purpose Slider 2           0..127",
   " 18   General Purpose Slider 3           0..127",
   " 19   General Purpose Slider 4           0..127",
   " 20   Continuous controller #20          0..127",
   " 21   Continuous controller #21          0..127",
   " 22   Continuous controller #22          0..127",
   " 23   Continuous controller #23          0..127",
   " 24   Continuous controller #24          0..127",
   " 25   Continuous controller #25          0..127",
   " 26   Continuous controller #26          0..127",
   " 27   Continuous controller #27          0..127",
   " 28   Continuous controller #28          0..127",
   " 29   Continuous controller #29          0..127",
   " 30   Continuous controller #30          0..127",
   " 31   Continuous controller #31          0..127",
   " 32   Bank Select (fine)                 0..127 usu.ignored",
   " 33   Modulation Wheel (fine)            0..127",
   " 34   Breath Control (fine)              0..127",
   " 35   Continuous controller #3 (fine)    0..127",
   " 36   Foot Controller (fine)             0..127",
   " 37   Portamento Time (fine)             0..127",
   " 38   Data Entry Slider (fine)           0..127",
   " 39   Main Volume (fine)                 0..127 usu. ignored",
   " 40   Stereo Balance (fine)              0..127",
   " 41   Continuous controller #9 (fine)    0..127",
   " 42   Pan (fine)                         0..127 usu. ignored",
   " 43   Expression (sub-Volume) (fine)     0..127 usu. ignored",
   " 44   Effect Control 1 (fine)            0..127",
   " 45   Effect Control 2 (fine)            0..127",
   " 46   Continuous controller #14 (fine)   0..127",
   " 47   Continuous controller #15 (fine)   0..127",
   " 48   Continuous controller #16          0..127",
   " 49   Continuous controller #17          0..127",
   " 50   Continuous controller #18          0..127",
   " 51   Continuous controller #19          0..127",
   " 52   Continuous controller #20 (fine)   0..127",
   " 53   Continuous controller #21 (fine)   0..127",
   " 54   Continuous controller #22 (fine)   0..127",
   " 55   Continuous controller #23 (fine)   0..127",
   " 56   Continuous controller #24 (fine)   0..127",
   " 57   Continuous controller #25 (fine)   0..127",
   " 58   Continuous controller #26 (fine)   0..127",
   " 59   Continuous controller #27 (fine)   0..127",
   " 60   Continuous controller #28 (fine)   0..127",
   " 61   Continuous controller #29 (fine)   0..127",
   " 62   Continuous controller #30 (fine)   0..127",
   " 63   Continuous controller #31 (fine)   0..127",
   " 64   Hold pedal (Sustain) on/off        0..63=off  64..127=on",
   " 65   Portamento on/off                  0..63=off  64..127=on",
   " 66   Sustenuto Pedal on/off             0..63=off  64..127=on",
   " 67   Soft Pedal on/off                  0..63=off  64..127=on",
   " 68   Legato Pedal on/off                0..63=off  64..127=on",
   " 69   Hold Pedal 2 on/off                0..63=off  64..127=on",
   " 70   Sound Variation                    0..127",
   " 71   Sound Timbre                       0..127",
   " 72   Sound Release Time                 0..127",
   " 73   Sound Attack Time                  0..127",
   " 74   Sound Brighness                    0..127",
   " 75   Sound Control 6                    0..127",
   " 76   Sound Control 7                    0..127",
   " 77   Sound Control 8                    0..127",
   " 78   Sound Control 9                    0..127",
   " 79   Sound Control 10                   0..127",
   " 80   General Purpose Button             0..63=off 64..127=on",
   " 81   General Purpose Button             0..63=off 64..127=on",
   " 82   General Purpose Button             0..63=off 64..127=on",
   " 83   General Purpose Button             0..63=off 64..127=on",
   " 84   Undefined on/off                   0..63=off 64..127=on",
   " 85   Undefined on/off                   0..63=off 64..127=on",
   " 86   Undefined on/off                   0..63=off 64..127=on",
   " 87   Undefined on/off                   0..63=off 64..127=on",
   " 88   Undefined on/off                   0..63=off 64..127=on",
   " 89   Undefined on/off                   0..63=off 64..127=on",
   " 90   Undefined on/off                   0..63=off 64..127=on",
   " 91   Effects Level                      0..127",
   " 92   Tremulo Level                      0..127",
   " 93   Chorus Level                       0..127",
   " 94   Celeste (Detune) Level             0..127",
   " 95   Phaser Level                       0..127",
   " 96   Data entry +1                      ignored",
   " 97   Data entry -1                      ignored",
   " 98   Non-Registered Parameter Number (coarse)0..127",
   " 99   Non-Registered Parameter Number (fine)  0..127",
   "100   Registered Parameter Number (coarse)    0..127",
   "101   Registered Parameter Number (fine) 0..127",
   "102   Undefined                          ?",
   "103   Undefined                          ?",
   "104   Undefined                          ?",
   "105   Undefined                          ?",
   "106   Undefined                          ?",
   "107   Undefined                          ?",
   "108   Undefined                          ?",
   "109   Undefined                          ?",
   "110   Undefined                          ?",
   "111   Undefined                          ?",
   "112   Undefined                          ?",
   "113   Undefined                          ?",
   "114   Undefined                          ?",
   "115   Undefined                          ?",
   "116   Undefined                          ?",
   "117   Undefined                          ?",
   "118   Undefined                          ?",
   "119   Undefined                          ?",
   "120   All Sound Off                      ignored",
   "121   All Controllers Off                ignored",
   "122   Local Keyboard On/Off              0..63=off 64..127=on",
   "123   All Notes Off                      ignored",
   "124   Omni Mode Off                      ignored",
   "125   Omni Mode On                       ignored",
   "126   Monophonic Mode On                 **",
   "127   Polyphonic Mode On (mono=off)      ignored"
};


void printLegend(MidiFile& midifile) {
   int sum = 0;
   int i;

   cout << "\n";
   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   cout << "%% DATA LEGEND                                               %%\n";
   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   cout << "%%Filename: " << midifile.getFilename() << endl;
   cout << "%%Ticks per quarter note: " << midifile.getTicksPerQuarterNote()
        << "\n";
   cout << "%%Time units used in column 1: ";
   switch (timetype) {
      case TICK: cout << "ticks\n";         break;
      case BEAT: cout << "beat\n";          break;
      case SEC:  cout << "seconds\n";       break;
      case MSEC: cout << "milliseconds\n";  break;
      default:   cout << "unknown\n";

   }
   // cout << "% unused parameter marker: " << unused << "\n";
   // check for opcodes used in data:
   sum = 0;
   for (i=0; i<(int)legend_opcode.size(); i++) {
      sum += legend_opcode[i];
   }
   if (sum > 0) {
      cout << "% " << sum << " opcodes are present in the data:\n";
      for (i=0; i<(int)legend_opcode.size(); i++) {
         if (legend_opcode[i]) {
            switch (i*1000) {
               case OP_NOTE:
                  cout << "%\topcode " << OP_NOTE << "\t= note\n";
                  cout << "%\t   column 1 = start time of note\n";
                  cout << "%\t   column 2 = opcode for note\n";
                  cout << "%\t   column 3 = duration of note\n";
                  cout << "%\t   column 4 = MIDI key number\n";
                  cout << "%\t   column 5 = MIDI attack velocity\n";
                  cout << "%\t   column 6 = MIDI channel\n";
                  cout << "%\t   column 7 = MIDI-file track number\n";
                  break;
               case OP_TEMPO:
                  cout << "%\topcode " << OP_TEMPO << "\t= tempo change\n";
                  cout << "%\t   column 1 = start time of tempo\n";
                  cout << "%\t   column 2 = opcode for tempo\n";
                  cout << "%\t   column 3 = number of beats per minute\n";
                  cout << "%\t   column 4-7 = unused\n";
                  break;
               case OP_CONTROL:
                  cout << "%\topcode " << OP_CONTROL
                       << "\t= continuous controller\n";
                  cout << "%\t   column 1 = action time of controller\n";
                  cout << "%\t   column 2 = opcode for controller\n";
                  cout << "%\t   column 3 = controller number\n";
                  cout << "%\t   column 4 = controller value\n";
                  cout << "%\t   column 5 = unused\n";
                  cout << "%\t   column 6 = MIDI channel\n";
                  cout << "%\t   column 7 = MIDI-file track number\n";
                  break;
               case OP_INSTR:
                  cout << "%\topcode " << OP_INSTR << "\t= instrument\n";
                  cout << "%\t   column 1 = start time of instrument on channel\n";
                  cout << "%\t   column 2 = opcode for instrument\n";
                  cout << "%\t   column 3 = instrument number\n";
                  cout << "%\t   column 4-5 = unused\n";
                  cout << "%\t   column 6 = MIDI channel\n";
                  cout << "%\t   column 7 = MIDI-file track number\n";
                  break;
               case OP_METER:
                  cout << "%\topcode " << OP_METER << "\t= meter signature\n";
                  cout << "%\t   column 1 = start time of meter signature\n";
                  cout << "%\t   column 2 = opcode for meter\n";
                  cout << "%\t   column 3 = numerator of meter\n";
                  cout << "%\t   column 4 = denominator of meter\n";
                  cout << "%\t   column 5-7 = unused\n";
                  break;
               case OP_KEYSIG:
                  cout << "%\topcode " << OP_KEYSIG << "\t= key signature\n";
                  cout << "%\t   column 1 = start time of key signature\n";
                  cout << "%\t   column 2 = opcode for key signature\n";
                  cout << "%\t   column 3 = number of sharps (positive) or flats (negative)\n";
                  cout << "%\t   column 4 = mode (0=major, 1=minor)\n";
                  cout << "%\t   column 5-7 = unused\n";
                  break;
               default:
                  cout << "%\topcode " << i*1000 << "\t= unknown\n";
            }
         }
      }
   }


   // check for instruments to list:
   sum = 0;
   for (i=0; i<(int)legend_instr.size(); i++) {
      sum += legend_instr[i];
   }
   if (sum > 0) {
      if (sum == 1) {
         cout << "% " << sum << " instrument timbre is present in the data:\n";
      } else {
         cout << "% " << sum << " instrument timbres are present in the data:\n";
      }
      for (i=0; i<(int)legend_instr.size(); i++) {
         if (legend_instr[i]) {
            cout << "%\tinstrument number " << i << "\t= "
                 << GMinstrument[i] << "\n";
         }
      }
   }

   // check for controllers to list:
   sum = 0;
   for (i=0; i<(int)legend_controller.size(); i++) {
      sum += legend_controller[i];
   }
   if (sum > 0) {
      if (sum == 1) {
      cout << "% " << sum << " type of controller is present in the data:\n";
      } else {
      cout << "% " << sum << " types of controllers are present in the data:\n";
      }
      for (i=0; i<(int)legend_controller.size(); i++) {
         if (legend_controller[i]) {
            cout << "%\tcontroller " << GMcontrollers[i] << "\n";
         }
      }
   }

   cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   cout << endl;

}



//////////////////////////////
//
// printMatlabArray -- print the Matlab array representing the MIDI file.
//

void printMatlabArray(MidiFile& midifile, vector<vector<double> >& matlab) {
   int i;
   sortArray(matlab);
   printLegend(midifile);
   if (!numQ) {
      printOpcodeVariables(legend_opcode);
   }
   cout << arrayname << " = [\n";
   for (i=0; i<(int)matlab.size(); i++) {
      printEvent(matlab[i]);
   }
   cout << "];\n";
}



//////////////////////////////
//
// sortArray -- sort the input file into time order because
//   the notes may be slightly out of time order.
//

void sortArray(vector<vector<double> >& matlab) {
   qsort(matlab.data(), matlab.size(), sizeof(vector<double>), eventcmp);
}



//////////////////////////////
//
// eventcmp -- compare two events and determine which should come
//    first in time.
//

int eventcmp(const void* a, const void* b) {
   vector<double>& A = *((vector<double>*)a);
   vector<double>& B = *((vector<double>*)b);

   if (A[0] < B[0]) {
      return -1;
   } else if (A[0] > B[0]) {
      return 1;
   } else {
      return 0;   // there is a tie if they come at the same time.
   }
}



//////////////////////////////
//
// printOpName -- print the OpCode's symbolic name for better
// readability.
//

void printOpName(int code) {
   switch (code) {

      case OP_NOTE:
         cout << OP_NOTE_NAME;
         break;
      case OP_CONTROL:
         cout << OP_CONT_NAME;
         break;
      case OP_INSTR:
         cout << OP_INSTR_NAME;
         break;
      case OP_TEMPO:
         cout << OP_TEMPO_NAME;
         break;
      case OP_METER:
         cout << OP_METER_NAME;
         break;
      case OP_KEYSIG:
         cout << OP_KEYSIG_NAME;
         break;
      default:
         cout << code;

   }
}



//////////////////////////////
//
// printEvent -- print the event
//

void printEvent(vector<double>& event) {
   int i;
   for (i=0; i<(int)event.size(); i++) {
      if ((i == 1) && (!numQ)) {
         printOpName(event[i]);
         cout << ",\t";
         continue;
      }
      cout << event[i];
      // if ((i==0) && (event[i] - (int)event[i] == 0.0)) {
      //    cout << ".0000";
      //    if (event[i] < 10) {
      //       cout << "0";
      //    }
      // }
      if (i<(int)event.size()-1) {
         cout << ",\t";
      }
   }
   cout << ";\n";
}



