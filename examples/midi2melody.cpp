//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Jun 30 11:52:06 PDT 2001
// Last Modified: Sat Jun 30 11:52:09 PDT 2001
// Filename:      ...sig/examples/all/midi2melody.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/humdrum/midi2melody.cpp
// Syntax:        C++; museinfo
//
// Description:   Converts a single melody MIDI file/track into an ASCII text 
//	          format with starting time and pitch.
//

#include "MidiFile.h"
#include "Options.h"

class Melody {
   public:
      double time;
      double duration;
      int    pitch;
};

// user interface variables
Options options;
int     track = 0;          // used with the -t option

// function declarations:
void      checkOptions      (Options& opts, int argc, char** argv);
void      example           (void);
void      usage             (const char* command);
void      convertToMelody   (MidiFile& midifile, SigCollection<Melody>& melody);
void      printMelody       (SigCollection<Melody>& melody, int tpq);
void      sortMelody        (SigCollection<Melody>& melody);
int       notecompare       (const void* a, const void* b);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));

   SigCollection<Melody> melody;
   convertToMelody(midifile, melody);
   sortMelody(melody);
   printMelody(melody, midifile.getTicksPerQuarterNote());

   return 0;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// sortMelody -- 
//

void sortMelody(SigCollection<Melody>& melody) {
   qsort(melody.getBase(), melody.getSize(), sizeof(Melody), notecompare);
}



//////////////////////////////
//
// printMelody --
//   only print the highest voice if multiple notes played together.
//

void printMelody(SigCollection<Melody>& melody, int tpq) {
   int i;
   double delta = 0;

   if (melody.getSize() < 1) {
      return;
   }

   Melody temp;
   temp.time = melody[melody.getSize()-1].time +
               melody[melody.getSize()-1].duration;
   temp.pitch = 0;
   temp.duration = 0;
   melody.append(temp);

   for (i=0; i<melody.getSize()-1; i++) {
      delta = melody[i+1].time - melody[i].time;
      if (delta == 0) {
         continue;
      }
      
      cout << (double)melody[i].time/tpq
           << "\t" << melody[i].pitch
           // << "\t" << (double)melody[i].duration/tpq
           << "\n";
      if (delta > melody[i].duration) {
         cout << (melody[i+1].time - (delta - melody[i].duration))/(double)tpq
              << "\t" << 0
              << "\n";
      }
   }
   cout << (double)melody[melody.getSize()-1].time/tpq
        << "\t" << 0
        << "\n";
}



//////////////////////////////
//
// convertToMelody --
//

void convertToMelody(MidiFile& midifile, SigCollection<Melody>& melody) {
   midifile.absoluteTime();
   if (track < 0 || track >= midifile.getNumTracks()) {
      cout << "Invalid track: " << track << " Maximum track is: "
           << midifile.getNumTracks() - 1 << endl;
   }
   int numEvents = midifile.getNumEvents(track);

   Array<int> state(128);   // for keeping track of the note states
   
   int i;
   for (i=0; i<128; i++) {
      state[i] = -1;
   }

   melody.setSize(numEvents);
   melody.setSize(0);
   melody.allowGrowth(1);

   Melody mtemp;
   int command;
   int pitch;
   int velocity;

   for (i=0; i<numEvents; i++) {
      command = midifile.getEvent(track,i).data[0] & 0xf0;
      if (command == 0x90) {
         pitch = midifile.getEvent(track, i).data[1];
         velocity = midifile.getEvent(track, i).data[2];
         if (velocity == 0) {
            // note off
            goto noteoff;
         } else {
            // note on
            state[pitch] = midifile.getEvent(track, i).time;
         }
      } else if (command == 0x80) {
         // note off
         pitch = midifile.getEvent(track, i).data[1];
noteoff:
         if (state[pitch] == -1) {
            continue;
         }
         mtemp.time = state[pitch];
         mtemp.duration = midifile.getEvent(track, i).time - state[pitch];
         mtemp.pitch = pitch;
         melody.append(mtemp);
         state[pitch] = -1;
      }
   }
}



//////////////////////////////
//
// checkOptions -- 
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("t|track=i:0",  "Track from which to extract melody"); 

   opts.define("author=b",  "author of program"); 
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");   
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);
   
   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 30 June 2001" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: June 2001" << endl;
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

   track = opts.getInteger("track");
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



///////////////////////////////
//
// notecompare -- for sorting the times of Melody messages
//

int notecompare(const void* a, const void* b) {
   Melody& aa = *((Melody*)a);
   Melody& bb = *((Melody*)b);

   if (aa.time < bb.time) {
      return -1;
   } else if (aa.time > bb.time) {
      return 1;
   } else {
      // highest note comes first
      if (aa.pitch > bb.pitch) {
         return 1;
      } else if (aa.pitch < bb.pitch) {
         return -1;
      } else {
         return 0;
      }
   }
}


    
// md5sum: d8201865177b80aa72106e3198962cdc midi2melody.cpp [20050403]
