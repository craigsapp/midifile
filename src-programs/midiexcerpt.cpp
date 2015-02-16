//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jul 22 18:59:27 PDT 2010
// Last Modified: Thu Jul 22 18:59:30 PDT 2010
// Filename:      ...sig/doc/examples/all/midiexcerpt/midiexcerpt.cpp
// Syntax:        C++
//
// Description:   Extracts a time region from a MIDI file.  Notes
//                starting before the start time will be ignored.
//                Notes not ending before the end time of the file
//                will be turned off at the given end time.
//

#include "Options.h"
#include "MidiFile.h"
#include "PerlRegularExpression.h"
#include <stdlib.h>

using namespace std;

// function declarations:
void   checkOptions        (Options& opts);
void   example             (void);
void   usage               (const char* command);
double getTimeInSeconds    (const char* timestring);
void   extractMidi         (MidiFile& outputfile, MidiFile& inputfile,
                            double starttime, double endtime);
int    getStartIndex       (MidiFile& midifile, int starttick);
int    getStopIndex        (MidiFile& midifile, int startindex, int stoptick);

// user interface variables:
double starttime = 0.0;    // used with -s option
double endtime   = 0.0;    // used with -e option

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   int       status;
   MidiFile  inputfile;
   MidiFile  outputfile;
   Options   options(argc, argv);

   checkOptions(options);

   status = inputfile.read(options.getArg(1));
   if (status == 0) {
      cout << "Syntax error in file: " << options.getArg(1) << "\n";
   }

   extractMidi(outputfile, inputfile, starttime, endtime);
   outputfile.write(options.getArg(2));

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// extractMidi -- Extract a time range from a MIDI file.  If the
//      endtime is negative, then that means through the end of the
//      original MIDI file.
//
//

void extractMidi(MidiFile& outputfile, MidiFile& inputfile, double starttime,
     double endtime) {

   outputfile.absoluteTime();
   outputfile.setTicksPerQuarterNote(inputfile.getTicksPerQuarterNote());
   if (inputfile.getTrackCount() > 1) {
      outputfile.addTrack(inputfile.getTrackCount()-1);
   }
   // outputfile.joinTracks();

   int i, j;


   Array<Array<Array<int> > > notestates;
   notestates.setSize(inputfile.getTrackCountAsType1());
   for (i=0; i<notestates.getSize(); i++) {
      notestates[i].setSize(16);
      for (j=0; j<16; j++) {
         notestates[i][j].setSize(128);
         notestates[i][j].allowGrowth(0);
         notestates[i][j].setAll(0);
      }
   }

   int offtype80 = 0;
   int offtype90 = 0;

   int starttick = inputfile.getAbsoluteTickTime(starttime);
   int stoptick  = inputfile.getAbsoluteTickTime(endtime);

   int startindex = getStartIndex(inputfile, starttick);
   int stopindex  = getStopIndex(inputfile, startindex, stoptick);

   MFEvent eventcopy;
   int track;
   int pitch;
   int channel = 0;

   // insert active tempo setting, if any
   MFEvent *tempoptr = NULL;
   for (i=0; i<startindex; i++) {
      if (inputfile.getEvent(0, i).isTempo()) {
         tempoptr = &inputfile.getEvent(0, i);
      }
   }
   if (tempoptr != NULL) {
      eventcopy = *tempoptr;
      eventcopy.time = 0;
      outputfile.addEvent(eventcopy);
   }

   // insert active timbre settings, if any
   Array<Array<int> > timbres;
   timbres.setSize(notestates.getSize());
   for (i=0; i<timbres.getSize(); i++) {
      timbres[i].setSize(16);
      timbres[i].setAll(-1);
   }
   for (i=0; i<startindex; i++) {
      if (inputfile.isTimbre(0, i)) {
         int tam = inputfile.getEvent(0, i).data[1];
         track = inputfile.getTrack(0, i);
         channel = inputfile.getChannelNibble(0, i);
         timbres[track][channel] = tam;
      }
   }
   eventcopy.data.setSize(2);
   eventcopy.time = 0;
   for (track=0; track<timbres.getSize(); track++) {
      for (channel=0; channel<timbres.getSize(); channel++) {
         if (timbres[track][channel] >= 0) {
            eventcopy.track = track;
            eventcopy.data[0] = 0xc0 | channel;
            eventcopy.data[1] = timbres[track][channel];
            outputfile.addEvent(eventcopy);
         }
      }
   }

   MFEvent *ptr;
   for (i=startindex; i<stopindex; i++) {
      ptr = &inputfile.getEvent(0,i);
      if (ptr->isNoteOff()) {
         if (ptr->getCommandNibble() == 0x90) {
            offtype90++;
         } else if (ptr->getCommandNibble() == 0x80) {
            offtype80++;
         }

         track = ptr->track;
         channel = ptr->getChannelNibble();
         pitch = ptr->data[1];
         if (notestates[track][channel][pitch] > 0) {
            notestates[track][channel][pitch]--;
         } else {
            // ignore the note off, since it is from a note
            // which was turned on before the selected time region.
            continue;
         }
      } else if (ptr->isNoteOn()) {
         track = ptr->track;
         pitch = ptr->data[1];
         notestates[track][channel][pitch]++;
      }
      eventcopy = *ptr;
      eventcopy.time -= starttick;
      outputfile.addEvent(eventcopy);
   }

   // Turn off any notes which are still on...

   int k;
   eventcopy.data.setSize(3);
   eventcopy.time = stoptick - starttick;
   for (track=0; track<notestates.getSize(); track++) {
      for (channel=0; channel<16; channel++) {
         for (pitch=0; pitch<128; pitch++) {
            for (k=0; k<notestates[track][channel][pitch]; k++) {
               eventcopy.track = track;
               eventcopy.data[1] = pitch;
               if (offtype90 > offtype80) {
                  eventcopy.data[0] = (uchar)(0x90 | channel);
                  eventcopy.data[0] = 0;
               } else {
                  eventcopy.data[0] = (uchar)(0x80 | channel);
                  eventcopy.data[2] = 64;
               }
               outputfile.addEvent(eventcopy);
            }
         }
      }
   }

   outputfile.sortTracks();
}



//////////////////////////////
//
// getStartIndex --
//

int getStartIndex(MidiFile& midifile, int starttick) {
   int i;
   for (i=0; i<midifile.getNumEvents(0); i++) {
      if (starttick <= midifile.getEvent(0,i).time) {
         return i;
      }
   }

   // something bad happened
   cerr << "ERROR in getStartIndex" << endl;
   exit(1);
}



//////////////////////////////
//
// getStopIndex --
//

int getStopIndex(MidiFile& midifile, int startindex, int stoptick) {
   int i;
   for (i=startindex; i<midifile.getNumEvents(0); i++) {
      if (stoptick <= midifile.getEvent(0,i).time) {
         return i-1;
      }
   }

   // something bad happened
   cerr << "ERROR in getStartIndex" << endl;
   exit(1);
}



//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {
   opts.define("begin|start|b|s=s:0", "Excerpt start time in sec or min:sec");
   opts.define("duration|d=s:0", "Duration of the excerpt in sec or min:sec");
   opts.define("end|e=s:-1", "Ending time of the excerpt in sec or min:sec");

   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, July 2010" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "midiextract version 1.0" << endl;
      cout << "compiled: " << __DATE__ << endl;
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   // can only have one output filename
   if (opts.getArgCount() != 2) {
      cout << "Error: need one input MIDI file and an output filename.";
      cout << endl;
      usage(opts.getCommand());
      exit(1);
   }

   starttime = getTimeInSeconds(opts.getString("begin"));
   if (opts.getBoolean("duration")) {
      double duration = getTimeInSeconds(opts.getString("duration"));
      if (duration <= 0.0) {
          cerr << "ERROR: duration must be positive" << endl;
          exit(1);
      }
      endtime = starttime + duration;
   } else {
      endtime = getTimeInSeconds(opts.getString("end"));
   }
}



//////////////////////////////
//
// getTimeInSeconds -- return the numeric value found in the string.
//    if the string contains a colon (:), then treate the number
//    on the left side of the colon as being in minutes, and the value
//    on the right as time in seconds.  Fractional values are allowed
//    on seconds.  Also allowed on minutes, but probably should not
//    be used...
//

double getTimeInSeconds(const char* timestring) {
   PerlRegularExpression pre;
   if (pre.search(timestring, ":", "")) {
      double minutes = 0.0;
      double seconds = 0.0;
      if (pre.search(timestring, "([\\d\\.\\+-]+):", "")) {
         minutes = strtod(pre.getSubmatch(1), NULL);
      }
      if (pre.search(timestring, ":([\\d\\.\\+-]+)", "")) {
         seconds = strtod(pre.getSubmatch(1), NULL);
      }
      return minutes * 60.0 + seconds;
   } else {
      if (pre.search(timestring, "([\\d+.+-]+)", "")) {
         return strtod(pre.getSubmatch(1), NULL);
      } else {
         return 0.0;
      }
   }
}



//////////////////////////////
//
// example -- gives example calls to the midiexcerpt program.
//

void example(void) {
   cout <<
   "# textmidi examples:                                                     \n"
   << endl;
}


//////////////////////////////
//
// usage -- how to run the midiexcerpt program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   << endl;
}



