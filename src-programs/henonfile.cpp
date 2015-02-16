//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jan  9 14:29:54 PST 2002
// Last Modified: Thu Jan 10 23:19:05 PST 2002
// Last Modified: Thu Apr  3 10:23:41 PST 2008 (added Humdrum output)
// Filename:      ...sig/examples/all/henonfile.cpp
// Web Address:   http://sig.sapp.org/examples/improv/improv/henonfile.cpp
// Syntax:        C++; museinfo
//
// Description:   Creates a fractal melodic line based on the
//                Henon Map.  Output can be either a MIDI file or
//                plain text.
//

#include "MidiFile.h"
#include "Convert.h"
#include "Options.h"
#include "CircularBuffer.h"
#include <string.h>
#include <stdio.h>

using namespace std;


// function declarations:
void        checkOptions        (Options& opts, int argc, char** argv);
void        example             (void);
void        usage               (const char* command);
void        createHenon         (double alpha, double beta, double x0,
                                 double y0, int maxcount, MidiFile& midifile);
int         checkTermination    (int key);
void        storeInMidiFile     (MidiFile& midifile, int key);
void        printGuidoNotation  (void);
void        printHumdrumNotation(void);
char*       convertMidiToGuido  (char* buffer, Array<char> notelist, int index);
void        printLeftHand       (void);
void        printRightHand      (void);

// User interface variables:
Options     options;
int         maxcount  = 10000;  // used with the -n option
double      alpha = -1.56693;   // used with the -a option
double      beta  = -0.011811;  // used with the -b option
double      x0 = 0.0;           // x-axis starting point
double      y0e = 0.0;          // y-axis starting point
int         textQ = 0;          // used with the --text option
int         guidoQ = 0;         // used with the -g option
int         humdrumQ = 0;       // used with the -u option
int         repeatQ = 0;        // used with the -r option
const char* filename = "test.mid"; // filename to write MIDI file
int         tpq = 96;           // ticks per quarter note in MIDI file
int         divisions = 4;      // number of notes per quarter note
int         instrument = 0;     // used with the -i option
double      tempo = 120.0;      // used with the -t option
Array<char> notelist;           // for printing in Guido Music Notation
int         minNote = 30;       // minimum note to play
int         maxNote = 100;      // maximum note to play

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   checkOptions(options, argc, argv); // process the command-line options

   MidiFile midifile;
   midifile.setTicksPerQuarterNote(tpq);
   midifile.allocateEvents(0, 2 * maxcount + 500);  // pre allocate space for
                                                    // max expected MIDI events
   notelist.setSize(maxcount+10);
   notelist.setSize(0);
   notelist.allowGrowth();

   midifile.absoluteTime();

   Array<uchar> mididata(2);
   mididata[0] = 0xc0;       // patch change on MIDI channel 1
   mididata[1] = (uchar) instrument;  // user input instrument
   midifile.addEvent(0, 0, mididata);

   // write the tempo to the midifile
   mididata.setSize(6);
   mididata[0] = 0xff;      // meta message
   mididata[1] = 0x51;      // tempo change
   mididata[2] = 0x03;      // three bytes to follow
   int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
   mididata[3] = (microseconds >> 16) & 0xff;
   mididata[4] = (microseconds >> 8)  & 0xff;
   mididata[5] = (microseconds >> 0)  & 0xff;
   midifile.addEvent(0, 0, mididata);

   createHenon(alpha, beta, x0, y0e, maxcount, midifile);
   if (guidoQ) {
      printGuidoNotation();
   } else if (humdrumQ) {
      printHumdrumNotation();
   } else {
      midifile.write(filename);
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// createHenon --
//

void createHenon(double alpha, double beta, double x0, double y0e,
   int maxcount, MidiFile& midifile) {

   double x = x0;
   double y = y0e;
   double newx;
   double newy;
   int key;
   int termination = 0;
   int i;

   for (i=0; i<maxcount; i++) {
      newx = 1 + alpha * x * x + beta * y;
      newy = x;
      x = newx;
      y = newy;

      key = (int)((x + 1.0)/2.0 * 127.0 + 0.5);
      if (key < minNote) {
         key = 0;
      }
      if (key > maxNote) {
         key = 0;
      }
      if (repeatQ) {
         termination = 0;
      } else {
         termination = checkTermination(key);
      }
      if (textQ) {
         cout << key << "\n";
         if (termination != 0) {
            cout << "REPEAT" << termination << endl;
            exit(0);
         }
      } else {
         storeInMidiFile(midifile, key);
         if (termination != 0) {
            midifile.write(filename);
            exit(0);
         }
      }
   }

}



//////////////////////////////
//
// storeInMidiFile --
//

void storeInMidiFile(MidiFile& midifile, int key) {
   static int timer = tpq;   // start after one beat (for patch change)
   char note = (char)key;

   // don't store extreme notes -- this gives interesting rhythms sometimes.
   if (key < minNote || key > maxNote) {
      note = 0;
      notelist.append(note);
      timer += tpq/divisions;
      return;
   }
   notelist.append(note);       // store note for displaying Guido Notation
   Array<uchar> midinote(3);
   midinote[0] = 0x90;
   midinote[1] = key;
   midinote[2] = 64;
   midifile.addEvent(0, timer, midinote);
   midinote[0] = 0x80;
   timer += tpq/divisions;
   midifile.addEvent(0, timer, midinote);
}



//////////////////////////////
//
// checkTermination --
//

int checkTermination(int key) {
   static CircularBuffer<int> memory;
   static int init = 0;
   if (init == 0) {
      init = 1;
      memory.setSize(1000);
      memory.reset();
   }
   memory.insert(key);
   if (memory.getCount() < 40 + 10) {
      return 0;
   }

   int j;
   int i;
   int cycleQ;
   for (j=1; j<20; j++) {
      cycleQ = 1;
      for (i=0; i<40; i++) {
         if (memory[i] != memory[i+j]) {
            cycleQ = 0;
            break;
         }
      }
      if (cycleQ == 1) {
         return j;
      }
   }

   // no 1-9 period cycles detected
   return 0;

}




//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("n|max-number=i:10000", "Maximum number of notes to generate");
   opts.define("a|alpha=d:-1.56693",   "alpha factor");
   opts.define("b|beta=d:-0.011811",   "beta factor");
   opts.define("x|x0=d:0.0",           "initial x value");
   opts.define("y|y0=d:0.0",           "initial y value");
   opts.define("max|max-note=i:100",   "maximum note to play; higher is rest");
   opts.define("min|min-note=i:30",    "minimum note to play; lower is rest");
   opts.define("text=b",               "display output as text only");
   opts.define("g|guido=b",            "Guido Music Notation output");
   opts.define("u|humdrum=b",          "Humdrum data file output");
   opts.define("i|instrument=i:0",     "General MIDI instrument number");
   opts.define("t|tempo=d:120",        "Tempo");
   opts.define("d|divisions=i:4",      "Number of notes per quarter note");
   opts.define("r|allow-repeats=b",    "Do not stop at cyclical patterns");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, Jan 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 3 Apr 2008" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   maxcount   = opts.getInteger("max-number");
   alpha      = opts.getDouble("alpha");
   beta       = opts.getDouble("beta");
   x0         = opts.getDouble("x0");
   y0e        = opts.getDouble("y0");
   textQ      = opts.getBoolean("text");
   guidoQ     = opts.getBoolean("guido");
   humdrumQ   = opts.getBoolean("humdrum");
   if (textQ == 1) {
      guidoQ   = 0;
      humdrumQ = 0;
   }
   repeatQ    = opts.getBoolean("allow-repeats");
   tempo      = opts.getDouble("tempo");
   divisions  = opts.getInteger("divisions");
   instrument = opts.getInteger("instrument");

   maxNote = opts.getInteger("max-note");
   minNote = opts.getInteger("min-note");
   if (minNote < 0)   minNote = 0;
   if (minNote > 126) minNote = 126;
   if (maxNote < 0)   maxNote = 0;
   if (maxNote > 126) maxNote = 126;
   if (minNote > maxNote) {
      int temp = minNote;
      minNote = maxNote;
      maxNote = temp;
   }

   if (instrument < 0) {
      instrument = 0;
   } else if (instrument > 127) {
      instrument = 127;
   }

   if (tempo < 4) {
      tempo = 4;
   } else if (tempo > 1000) {
      tempo = 1000;
   }

   if (humdrumQ == 0 && guidoQ == 0 && textQ == 0 && opts.getArgCount() != 1) {
      usage(opts.getCommand());
      exit(1);
   }
   if (humdrumQ == 0 && textQ == 0 && guidoQ == 0) {
      filename = opts.getArg(1);
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
   cout << "Usage: " << command << " outputfile" << endl;

}



//////////////////////////////
//
// printGuidoNotation --
//

void printGuidoNotation(void) {
   cout << "{ ";
   printRightHand();
   cout << ", ";
   printLeftHand();
   cout << " }\n";
}



//////////////////////////////
//
// printHumdrumNotation --
//

void printHumdrumNotation(void) {
   char buffer[1024] = {0};
   cout << "!!!alpha:\t" << alpha << "\n";
   cout << "!!!beta:\t" << beta << "\n";
   cout << "!!!start:\t(" << x0 << ", " << y0e << ")\n";
   cout << "**kern\n";
   int i;
   for (i=0; i<notelist.getSize(); i++) {
      cout << "16";
      if (notelist[i] <= 0) {
         cout << "r";
      } else {
         cout << Convert::base12ToKern(buffer, notelist[i]);
      }
      cout << "\n";
      if ((i+1) % 32 == 0) {
         cout << "=" << (i / 32) + 2 << "\n";
      }

   }
   cout << "*-" << endl;
}



//////////////////////////////
//
// printRightHand --
//

void printRightHand(void) {
   char buffer[128] = {0};
   cout << "[";
   int i;
   cout << "\\meter<\"2/4\">\n";
   for (i=0; i<notelist.getSize(); i++) {
      if (notelist[i] >= 60) {
         cout << convertMidiToGuido(buffer, notelist, i) << " ";
      } else {
         cout << "_/16" << " ";
      }
      if ((i+1)%16 == 0) {
         cout << "\n\t";
      }
   }
   cout << "]\n";
}



//////////////////////////////
//
// printLeftHand --
//

void printLeftHand(void) {
   char buffer[128] = {0};
   cout << "[";
   int i;
   cout << "\\meter<\"2/4\">\n";
   for (i=0; i<notelist.getSize(); i++) {
      if (notelist[i] < 60) {
         cout << convertMidiToGuido(buffer, notelist, i) << " ";
      } else {
         cout << "_/16" << " ";
      }
      if ((i+1)%16 == 0) {
         cout << "\n\t";
      }
   }
   cout << "]\n";
}



//////////////////////////////
//
// convertMidiToGuido -- convert from MIDI note number to Guido Music
//     Notation note name.
//

char* convertMidiToGuido(char* buffer, Array<char> notelist, int index) {
   if (notelist[index] == 0) {
      strcpy(buffer, "_/16");
      return buffer;
   }
   int octave = (int)notelist[index] / 12 - 4;
   if (octave > 2) octave--;

   char octbuf[32] = {0};
   sprintf(octbuf, "%d", octave);

   int pc = (int)notelist[index] % 12;
   switch (pc) {
      case 0:  strcpy(buffer, "c");  break;
      case 1:  strcpy(buffer, "c#");  break;
      case 2:  strcpy(buffer, "d");   break;
      case 3:  strcpy(buffer, "e&");  break;
      case 4:  strcpy(buffer, "e");   break;
      case 5:  strcpy(buffer, "f");   break;
      case 6:  strcpy(buffer, "f#");  break;
      case 7:  strcpy(buffer, "g");   break;
      case 8:  strcpy(buffer, "a&");  break;
      case 9:  strcpy(buffer, "a");   break;
      case 10: strcpy(buffer, "b&");  break;
      case 11: strcpy(buffer, "b");   break;
   }
   strcat(buffer, octbuf);
   strcat(buffer, "/16");
   return buffer;
}



