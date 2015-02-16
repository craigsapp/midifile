//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Mar  5 22:49:55 PST 2004
// Last Modified: Sat Mar  6 11:28:05 PST 2004
// Last Modified: Thu Jan  6 03:41:05 PST 2011 (fixed array out-of-bounds err)
// Filename:      ...sig/examples/all/mid2hum.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/humdrum/mid2hum.cpp
// Syntax:        C++; museinfo
//
// Description:   Description: Converts a MIDI file into a Humdrum file.
//
// Recommendation: MIDI to notation (such as Humdrum) is a bit of an
//                open-ended problem.  I am not going to go through the
//                trouble of writing a robust converter.  For more complicated
//                MIDI to Humdrum conversions, I suggest the following method:
//
//                If you want to convert MIDI files into Humdrum files,
//                I would suggest that you use the xml2hum program:
//                   http://museinfo.sapp.org/examples/humdrum/xml2hum.cpp
//                First, import a MIDI file into Finale or Sibelius music
//                editing program.  Second, edit the music to your liking.
//                Third, export the music to MusicXML via the Dolet plugin
//                for either of the two editors:
//                   http://www.recordare.com/sibelius
//                   http://www.recordare.com/finale
//                Then finally, use the xml2hum program (also available
//                on the web at: http://kern.humdrum.net/program/xml2hum)
//                to convert the music into the Humdrum format.
//
// Todo:          tied notes
//                reading MIDI+ data from volume marker
//                lyrics
//                clean up rhythm on slightly shortened or lengthend notes.
// Done:
//                key signatures
//                barlines (only for constant meters)
//                time signatures
//                chord notes (partly done, but not robust)
//                adding rests to ends of tracks to make all tracks same length
//                adding rests to start of tracks to make all tracks same length
//
// Reference:     http://crystal.apana.org.au/ghansper/midi_introduction/midi_file_format.html
//

#include "MidiFile.h"
#include "Options.h"
#include "Convert.h"
#include "HumdrumFile.h"
#include <math.h>
#include <cctype>
#include <iostream>
#include <sstream>
#define SSTREAM stringstream
#define CSTRING str().c_str()

using namespace std;


class MidiInfo {
   public:
      int track;
      int state;
      int index;
      int tickdur;
      int starttick;
      int key;         // MIDI key number of note
      int chord;       // boolean for chord notes

      MidiInfo(void) { clear(); }
      void clear(void) { key = -1;
            chord = track = state = index = tickdur = starttick = 0; }
};

class MetaInfo {
   public:
      int type;
      int tempo;
      int starttick;
      int numerator;
      int denominator;
      int mode;
      int keysig;
      char text[512];
      int tsize;  // text size in bytes
      MetaInfo(void) { clear(); }
      void clear(void) { type = starttick = numerator = denominator = 0;
                         tempo = keysig = mode = 0;
                         text[0] = '\0'; tsize = 0;
                        }
};

// user interface variables
Options options;
int     extracttrack = -1;   // a single track to output
double  quantlevel = 0.25;   // quatization level for durations.
int     serialQ = 0;         // used with the -s option
int     reverseQ = 0;        // used with the -r option
int     measurenumQ = 1;     // used with the -M option
double  pickupbeat = 0.0;    // used with the -p option
double  timesigtop = 4.0;    // used to print barlines
double  timesigbottom = 4.0; // used to print barlines

// function declarations:
void      convertToHumdrum  (MidiFile& midifile);
void      getMidiData       (Array<Array<MidiInfo> >& mididata,
                             MidiFile& midifile);
void      storenote         (MidiInfo& info, Array<Array<MidiInfo> >& mididata,
                             int i, int currtick);
void      printKernData     (Array<Array<MidiInfo> >& mididata,
                             MidiFile& midifile, Array<MetaInfo>& metadata);
void      identifyChords    (Array<Array<MidiInfo> >& mididata);
void      correctdurations  (Array<Array<MidiInfo> >& mididata, int tpq);
int       MidiInfoCompare   (const void* a, const void* b);
void      printRestCorrection (ostream& out, int restcorr, int tqp);
void      processMetaMessage(MidiFile& midifile, int track, int event,
                             Array<MetaInfo>& metadata);
void      printMetaData     (ostream& out, Array<MetaInfo>& metadata,
                             int metaindex);
void      splitDataWithMeasure(ostream& out, HumdrumFile& hfile, int index,
                             int& measurenum, double firstdur);
void      printHumdrumFileWithBarlines(ostream& out, HumdrumFile& hfile);
void      checkOptions      (Options& opts, int argc, char** argv);
void      example           (void);
void      usage             (const char* command);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   convertToHumdrum(midifile);

   return 0;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// convertToHumdrum -- convert a MIDI file into Humdrum format.
//

void convertToHumdrum(MidiFile& midifile) {
   int ticksperquarter = midifile.getTicksPerQuarterNote();
   cout << "!! Converted from MIDI with mid2hum" << endl;
   cout << "!! Ticks Per Quarter Note = " << ticksperquarter << endl;
   cout << "!! Track count: " << midifile.getNumTracks() << endl;
   Array<Array<MidiInfo> > mididata;
   getMidiData(mididata, midifile);
}



//////////////////////////////
//
// storenote -- store a MIDI note into the data array for later processing.
//

void storenote(MidiInfo& info, Array<Array<MidiInfo> >& mididata,
      int i, int currtick) {
   if (info.state == 0) {
      // don't store a note already in the off state.
      cout << "Can not store an empty note" << endl;
      return;
   }

   info.tickdur = currtick - info.starttick;
   mididata[i].append(info);
   info.clear();
}



//////////////////////////////
//
// getMidiData --
//

void getMidiData(Array<Array<MidiInfo> >& mididata, MidiFile& midifile) {
   mididata.setSize(midifile.getNumTracks());
   int i;
   int j;
   for  (i=0; i<mididata.getSize(); i++) {
      mididata[i].setSize(10000);
      mididata[i].setGrowth(10000);
      mididata[i].setSize(0);
   }

   Array<MetaInfo> metadata;
   metadata.setSize(1000);
   metadata.setGrowth(1000);
   metadata.setSize(0);

   Array<Array<MidiInfo> > notestates;
   notestates.setSize(midifile.getNumTracks());
   for (i=0; i<notestates.getSize(); i++) {
      notestates[i].setSize(128);
      notestates[i].allowGrowth(0);
   }

   // extract a list of notes in the MIDI file along with their durations
   int k;
   for (i=0; i<midifile.getNumTracks(); i++) {
      for (j=0; j<midifile.getNumEvents(i); j++) {
         if (((midifile.getEvent(i, j).data[0] & 0xf0) == 0x90) &&
             (midifile.getEvent(i, j).data[2] > 0) ) {
            // a note-on message. Store the state
            k = midifile.getEvent(i, j).data[1];
            if (notestates[i][k].state == 1) {
               storenote(notestates[i][k], mididata, i,
                     midifile.getEvent(i, j).time);
            }
            notestates[i][k].track = i;
            notestates[i][k].key = k;
            notestates[i][k].state = 1;
            notestates[i][k].index = j;
            notestates[i][k].starttick = midifile.getEvent(i, j).time;
            notestates[i][k].tickdur = -1;
         } else if (((midifile.getEvent(i, j).data[0] & 0xf0) == 0x80) ||
             (((midifile.getEvent(i, j).data[0] & 0xf0) == 0x90) &&
             (midifile.getEvent(i, j).data[2] == 0)) ) {
            // a note-off message.  Print the previous stored note-on message
            k = midifile.getEvent(i, j).data[1];
            storenote(notestates[i][k], mididata, i,
                  midifile.getEvent(i, j).time);
         } else {
            processMetaMessage(midifile, i, j, metadata);
         }
      }
   }

/*
   // test print the note information
   for (i=0; i<mididata.getSize(); i++) {
      cout << "Track " << i << endl;
      for (j=0; j<mididata[i].getSize(); j++) {
         cout << "\tNote: pitch = "
              << (int)midifile.getEvent(i, mididata[i][j].index).data[1]
              << "\tduration = "
              << (double)mididata[i][j].tickdur/midifile.getTicksPerQuarterNote()
              << endl;
      }
   }
*/

   for (i=0; i<mididata.getSize(); i++) {
      qsort(mididata[i].getBase(), mididata[i].getSize(), sizeof(MidiInfo),
            MidiInfoCompare);
   }
   identifyChords(mididata);
   correctdurations(mididata, midifile.getTicksPerQuarterNote());

   printKernData(mididata, midifile, metadata);
}



//////////////////////////////
//
// processMetaMessage --
//

void processMetaMessage(MidiFile& midifile, int track, int event,
      Array<MetaInfo>& metadata) {
   MetaInfo tempmeta;
   tempmeta.type = midifile.getEvent(track, event).data[1];
   tempmeta.starttick = midifile.getEvent(track, event).time;
   int tempo = 0;
   int d;  // counter into data field of meta message
   switch (tempmeta.type) {
      case 0x00:   // sequence number
         break;
      case 0x01:   // text
         break;
      case 0x02:   // copyright notice
         break;
      case 0x03:   // sequence/track name
         break;
      case 0x04:   // instrument name
         break;
      case 0x05:   // lyric
         break;
      case 0x06:   // marker
         tempmeta.tsize = (uchar)midifile.getEvent(track, event).data[2];
         for (d=0; d<tempmeta.tsize; d++) {
            tempmeta.text[d] = midifile.getEvent(track, event).data[3+d];
         }
         tempmeta.text[tempmeta.tsize] = '\0';
         break;
      case 0x07:   // cue point
         tempmeta.tsize = (uchar)midifile.getEvent(track, event).data[2];
         for (d=0; d<tempmeta.tsize; d++) {
            tempmeta.text[d] = midifile.getEvent(track, event).data[3+d];
         }
         tempmeta.text[tempmeta.tsize] = '\0';
         break;
         break;
      case 0x20:   // MIDI channel prefix
         break;
      case 0x54:   // SMPTE Offset
         break;
      case 0x2F:   // end of track marker
         break;
      case 0x7F:   // sequencer-specific meta event
         break;
      case 0x51:   // tempo marking
         tempo = midifile.getEvent(track, event).data[3];
         tempo = (tempo << 8) | midifile.getEvent(track, event).data[4];
         tempo = (tempo << 8) | midifile.getEvent(track, event).data[5];
         tempmeta.tempo = (int)(60.0/tempo*1000000.0 + 0.5);
         break;
      case 0x58:   // time signature
         tempmeta.numerator   = midifile.getEvent(track, event).data[3];
         tempmeta.denominator = (int)pow(2.0,
               midifile.getEvent(track, event).data[4]);
         break;
      case 0x59:   // key signature
         tempmeta.keysig = midifile.getEvent(track, event).data[3];
         tempmeta.mode   = midifile.getEvent(track, event).data[4];
   }

   metadata.append(tempmeta);
   // cout << "!!meta:" << hex << tempmeta.type << dec << endl;
}


//////////////////////////////
//
// correctdurations -- if notes are turned off earlier than the
//    next note (or the next note is turned on before the current
//    note is turned off, then adjust the tick values so that the
//    rhythm values might make sense.
//

void correctdurations  (Array<Array<MidiInfo> >& mididata, int tpq) {
   int i, j;
   double duration = 0.0;
   double fraction = 0.0;
   int    count = 0;
   int    durationcorrection = 0;

   for (i=0; i<mididata.getSize(); i++) {
      if (mididata[i].getSize() == 0) {
         continue;
      }
      for (j=0; j<mididata[i].getSize(); j++) {
         duration = (double)mididata[i][j].tickdur/tpq;
         fraction = duration/quantlevel;
         count = (int)fraction;
         fraction = fraction - count;
         if (fraction > 0.50) {
            durationcorrection = -(int)((1.0 - fraction) * tpq + 0.5);
         } else {
            durationcorrection = (int)(fraction * tpq + 0.5);
         }

//         cout << "tpq: "  << tpq << endl;
//         cout << "\tFraction value: "  << fraction << endl;
//         cout << "\tCorrection value: "  << durationcorrection << endl;
//         cout << "\tDuration value: "  << mididata[i][j].tickdur << endl;
//         if (j<mididata[i].getSize()-1) {
//            cout << "\tDifference value: "
//                 << mididata[i][j+1].starttick - mididata[i][j].starttick
//                 << endl;
//            cout << "\tIdeal correction: "
//                 << mididata[i][j+1].starttick - mididata[i][j].starttick
//                    - mididata[i][j].tickdur
//                 << endl;
//         }

         if (-durationcorrection != mididata[i][j].tickdur) {
            mididata[i][j].tickdur += durationcorrection;
         }
      }
   }
}



//////////////////////////////
//
// identifyChords --
//

void identifyChords(Array<Array<MidiInfo> >& mididata) {
   int i, j;
   for (i=0; i<mididata.getSize(); i++) {
      for (j=1; j<mididata[i].getSize(); j++) {
         if ((mididata[i][j].starttick == mididata[i][j-1].starttick) &&
             (mididata[i][j].tickdur == mididata[i][j-1].tickdur) ) {
            mididata[i][j].chord = 1;
         }
      }
   }
}



//////////////////////////////
//
// MidiInfoCompare --
//

int MidiInfoCompare(const void* a, const void* b) {
   MidiInfo& A = *((MidiInfo*)a);
   MidiInfo& B = *((MidiInfo*)b);

   if (A.starttick < B.starttick) {
      return -1;
   } else if (A.starttick > B.starttick) {
      return +1;
   } else {
      // separate voices by duration:
      if (A.tickdur < B.tickdur)  {
         return -1;
      } else if (A.tickdur > B.tickdur) {
         return +1;
      } else {
         // break ties by key number
         if (A.key < B.key) {
            return -1;
         } else if (A.key > B.key) {
            return +1;
         } else {
            return 0;
         }
      }
   }
}



//////////////////////////////
//
// printKernData --
//

void printKernData(Array<Array<MidiInfo> >& mididata, MidiFile& midifile,
      Array<MetaInfo>& metadata) {
   int i;
   int j;

   Array<int> kerntrack;
   kerntrack.setSize(mididata.getSize());
   for (i=0; i<mididata.getSize(); i++) {
      if (mididata[i].getSize() == 0) {
         kerntrack[i] = 0;
      } else {
         kerntrack[i] = 1;
      }
   }

   Array<int> restcorrection;
   restcorrection.setSize(mididata.getSize());
   restcorrection.setGrowth(0);
   restcorrection.setAll(0);
   int maxticks = 0;
   int testticks = 0;
   for (i=0; i<mididata.getSize(); i++) {
      if (mididata[i].getSize() > 0) {
         testticks = mididata[i][mididata[i].getSize()-1].starttick +
             mididata[i][mididata[i].getSize()-1].tickdur;
         if (testticks > maxticks) {
            maxticks = testticks;
         }
      }
   }
   for (i=0; i<mididata.getSize(); i++) {
      if (kerntrack[i] == 0) {
         continue;
      }
      testticks = mididata[i][mididata[i].getSize()-1].starttick +
          mididata[i][mididata[i].getSize()-1].tickdur;
      restcorrection[i] = maxticks - testticks;
   }

   Array<int> startrestcorrection;
   startrestcorrection.setSize(mididata.getSize());
   startrestcorrection.setGrowth(0);
   startrestcorrection.setAll(0);
   int minstartticks = 999999;
   for (i=0; i<mididata.getSize(); i++) {
      if (kerntrack[i] == 0) {
         continue;
      }
      if (minstartticks > mididata[i][0].starttick) {
         minstartticks = mididata[i][0].starttick;
      }
   }
   for (i=0; i<mididata.getSize(); i++) {
      if (kerntrack[i] == 0) {
         continue;
      }
      startrestcorrection[i] = mididata[i][0].starttick - minstartticks;
   }

   HumdrumFile base;
   HumdrumFile extra;
   HumdrumFile tempfile;
   int baseQ = 0;
   SSTREAM *buffstream;
   HumdrumFile* hpointer[2];
   hpointer[0] = &base;
   hpointer[1] = &extra;

   char buffer[1024] = {0};
   int tpq = midifile.getTicksPerQuarterNote();
   int difference = 0;
   int chordnote = 0;
   int starttime = 0;
   int metaindex = 0;

   int ii;
   for (ii=0; ii<mididata.getSize(); ii++) {
      // go in reverse order with the tracks because the ordering is
      // usually from highest to lowest which should be reversed
      // in the Humdrum file according to the specification
      if (reverseQ) {
         i = ii;
      } else {
         i = mididata.getSize() - 1 - ii;
      }

      if (kerntrack[i] == 0) {
         continue;
      }
      if ((extracttrack > -1) && (i != (extracttrack-1))) {
         continue;
      }
      buffstream = new SSTREAM;
      (*buffstream) << "**kern\n";
      metaindex = 0;
      for (j=0; j<mididata[i].getSize(); j++) {
         while ((metaindex < metadata.getSize()) &&
             (metadata[metaindex].starttick <= mididata[i][j].starttick)) {
            printMetaData(*buffstream, metadata, metaindex);
            metaindex++;
         }
         // check for a rest
         if (j>0) {
            difference = mididata[i][j].starttick - mididata[i][j-1].starttick;
            difference -= mididata[i][j-1].tickdur;
            if (difference < 0) {
               (*buffstream) << "!funny timing: " << difference << "\n";
            } else if (difference > 0) {
               // temporary fix for duration of rests
               while ((double)difference/tpq > 4.0) {
                  (*buffstream) << "1r" << endl;
                  difference -= tpq * 4;
               }
               (*buffstream) << Convert::durationToKernRhythm(buffer,
                  (double)difference/tpq);
               (*buffstream) << "r" << endl;
            }
         } else {
            printRestCorrection(*buffstream, startrestcorrection[i], tpq);
         }
         starttime = mididata[i][j].starttick;

         chordnote = 0;
         while ((j < mididata[i].getSize()) &&
                  (starttime == mididata[i][j].starttick)) {
            if (chordnote) {
               (*buffstream) << ' ';
            }
            (*buffstream) << Convert::durationToKernRhythm(buffer,
                  (double)mididata[i][j].tickdur/tpq);
            (*buffstream) << Convert::base12ToKern(buffer, mididata[i][j].key);
            chordnote = 1;
            j++;
         }
         (*buffstream) << endl;
         j--;
      }
      printRestCorrection(*buffstream, restcorrection[i], tpq);
      (*buffstream) << "*-\n";
      (*buffstream) << ends;
      if (serialQ) {
         // cout << (*buffstream).CSTRING;
         base.clear();
         base.read(*buffstream);
         printHumdrumFileWithBarlines(cout, base);
      } else {
         tempfile.clear();
         tempfile.read(*buffstream);
         delete buffstream;
         buffstream = new SSTREAM;
         printHumdrumFileWithBarlines(*buffstream, tempfile);
         (*buffstream) << ends;
         if (baseQ == 0) {
            base.clear();
            base.read(*buffstream);
            baseQ = 1;
         } else {
            extra.clear();
            extra.read(*buffstream);
            base.assemble(tempfile, 2, hpointer);
            base = tempfile;
         }
      }
      delete buffstream;
   }

   if (!serialQ) {
      cout << base;
   }
}



//////////////////////////////
//
// printMetaData --
//

void printMetaData(ostream& out, Array<MetaInfo>& metadata, int metaindex) {
   int ii;
   int count = 0;
   switch (metadata[metaindex].type) {
      case 0x06:   // marker
         break;    // ignore for now

         for (ii=0; ii<metadata[metaindex].tsize; ii++) {
            if (std::isprint(metadata[metaindex].text[ii])) {
               count++;
            }
         }
         if (count > 0) {
            out << "! ";
            for (ii=0; ii<metadata[metaindex].tsize; ii++) {
               if (std::isprint(metadata[metaindex].text[ii])) {
                  out << metadata[metaindex].text[ii];
               }
            }
            out << "\n";
         }
         break;

      case 0x07:   // cue point
         break;    // ignore for now

         for (ii=0; ii<metadata[metaindex].tsize; ii++) {
            if (std::isprint(metadata[metaindex].text[ii])) {
               count++;
            }
         }
         if (count > 0) {
            out << "! ";
            for (ii=0; ii<metadata[metaindex].tsize; ii++) {
               if (std::isprint(metadata[metaindex].text[ii])) {
                  out << metadata[metaindex].text[ii];
               }
            }
            out << "\n";
         }
         break;

      case 0x51:    // tempo marking
         out << "*MM" << metadata[metaindex].tempo << "\n";
         break;
      case 0x58:    // time signature
         out << "*M" << metadata[metaindex].numerator << "/"
                     << metadata[metaindex].denominator << "\n";
         break;
      case 0x59:    // key signature
           out << Convert::keyNumberToKern(metadata[metaindex].keysig) << "\n";
        break;
//      default:
//         out << "!meta:" << hex << metadata[metaindex].type << dec << endl;
   }

}



//////////////////////////////
//
// printRestCorrection --
//

void printRestCorrection (ostream& out, int restcorr, int tqp) {
   double totaldur = (double)restcorr/tqp;
   while (totaldur >= 1.0) {
      out << "4r\n";
      totaldur -= 1.0;
   }

   char buffer[1024] = {0};
   if (totaldur <= 0.0) {
      return;
   }

   out << Convert::durationToKernRhythm(buffer, totaldur);
   out << "r\n";
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("s|serial=b",    "print tracks serially rather than assembled");
   opts.define("r|reverse=b",   "print spines in reverse order");
   opts.define("M|no-measure-numbers=b",   "don't print measure numbers");
   opts.define("p|pickup=d:0.0","pickup beat at start of music before barline");
   opts.define("t|track=i:-1",  "track number to extract (offset from 1)");
   opts.define("q|quantization=d:0.25",  "quantization level");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 5 March 2004" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: March 2003" << endl;
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

   extracttrack = opts.getInteger("track");
   quantlevel   = opts.getDouble("quantization");
   serialQ      = opts.getBoolean("serial");
   reverseQ     = opts.getBoolean("reverse");
   measurenumQ  =!opts.getBoolean("no-measure-numbers");
   pickupbeat   = opts.getDouble("pickup");
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



//////////////////////////////
//
// printHumdrumFileWithBarlines --
//

void printHumdrumFileWithBarlines(ostream& out, HumdrumFile& hfile) {
   hfile.analyzeRhythm("4");
   double firstdur = 0.0;
   int i;
   int measurenum = 1;
   int startmeasure = 0;
   int endmeasure = 0;
   double bpos = 0.0;
   for (i=0; i<hfile.getNumLines(); i++) {
      if (hfile[i].isData()) {
         bpos = (hfile[i].getAbsBeat() - pickupbeat) /
                   timesigtop*4.0 / timesigbottom;
         startmeasure = (int)bpos;
         endmeasure = (int)(bpos + Convert::kernToDuration(hfile[i][0]) /
                      timesigtop*4.0 / timesigbottom - 0.001);
         if (fabs(bpos - (int)bpos) < 0.001) {
            out << "=";
            if (measurenumQ) {
               out << measurenum;
            }
            if ((measurenum == 1) && (pickupbeat = 0.0)) {
               out << "-"; // non-printed barline
            }
            measurenum++;
            out << "\n";
         }
         if (startmeasure == endmeasure) {
            out << hfile[i] << "\n";
         } else {
            firstdur = ((double)endmeasure * timesigtop /
                         4.0 * timesigbottom) - hfile[i].getAbsBeat();
            splitDataWithMeasure(out, hfile, i, measurenum, firstdur);
         }
      } else {
         out << hfile[i] << "\n";
      }
   }

}



//////////////////////////////
//
// splitDataWithMeasure --
//

void splitDataWithMeasure(ostream& out, HumdrumFile& hfile, int index,
      int& measurenum, double firstdur) {
   double seconddur = Convert::kernToDuration(hfile[index][0]) - firstdur;
   int i;
   int tokencount = hfile[index].getTokenCount(0);
   char buffer[1024] = {0};
   char tokenbuffer[1024] = {0};
   int base40 = 0;

   // print first part
   for (i=0; i<tokencount; i++) {
      hfile[index].getToken(tokenbuffer, 0, i);
      base40 = Convert::kernToBase40(tokenbuffer);
      if (base40 >= 0) {  // not a rest
         out << '[';
         out << Convert::durationToKernRhythm(buffer, firstdur);
         out << Convert::base40ToKern(buffer, base40);
      } else {
         out << Convert::durationToKernRhythm(buffer, firstdur);
         out << "r";
      }
      if (i<tokencount-1) {
         out << ' ';
      }
   }
   out << "\n";

   // print barline
   out << "=";
   if (measurenumQ) {
      out << measurenum;
   }
   if ((measurenum == 1) && (pickupbeat = 0.0)) {
      out << "-"; // non-printed barline
   }
   out << "\n";
   measurenum++;

   // print second part
   for (i=0; i<tokencount; i++) {
      hfile[index].getToken(tokenbuffer, 0, i);
      base40 = Convert::kernToBase40(tokenbuffer);
      out << Convert::durationToKernRhythm(buffer, seconddur);
      if (base40 >= 0) {  // not a rest
         out << Convert::base40ToKern(buffer, base40);
         out << ']';
      } else {
         out << "r";
      }
      if (i<tokencount-1) {
         out << ' ';
      }
   }
   out << "\n";

}


/* Basic Documentation:

Here are the options available with the mid2hum program:

-s              == print the voices serially.  This needs to be done when
                   There are rhythm errors which prevent the file from being
                   printed in score format.  You will need to correct
                   the rhythm in the individual parts, and then use
                   the Humdrum assemble command to combine the files together.
-r              == output the Humdrum spines in reverse ordering.  The default
                   method is from the last track to the first track in the
                   MIDI file which usually corresponds to the lowest part
                   to highest part respectively which is the standard
                   ordering of parts in a Humdrum file.
-M              == don't print measure numbers.  This is useful if measure
                   numbers need to be edited separately, or there was a
                   measure counting problem in the program.
-p 0.0          == specify a pickup-duration with which the music starts.
                   The duration specified after the -p option is the number
                   of quarter notes in the pickup beat(s).  1.0 means one
                   quarter note, 0.5 is one half note, 1.5 is a dotted
                   quarter note, etc.
-t -1           == extract and convert only specified track.  Track 1 is
                   extracted with -t 1, Track 2 with -t 2.  Some tracks
                   do not contain data, and will have an empty conversion.
                   This is typical of a Type 1 MIDI file's first track which
                   is usually used for control data.
-q 0.25         == often score-based MIDI files contain durations which are
                   not precisely legato, and a small rest is placed after
                   the note.  This option quantizes the ending position of
                   a note to the nearest rhythmic value (by default to the
                   nearest sixteenth note.)
--version=b     == print when the program was compiled, and what version
                   is it.
--options       == print a list of all possible command-line options for
                   the program.

*/



