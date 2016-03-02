//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Feb 19 22:25:22 PST 2016
// Last Modified: Sat Feb 27 18:16:39 PST 2016
// Filename:      midifile/src-programs/mid2svg.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/src-programs/mid2svg.cpp
// Syntax:        C++; museinfo
//
// Description:   Convert a MIDI file into an SVG piano roll.
//

#include "MidiFile.h"
#include "Options.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace std;

// User interface variables:
Options options;
int      dataQ        = 0;      // used with -d option
int      roundedQ     = 0;      // used with -r option
int      darkQ        = 0;      // used with --dark option
double   Scale        = 1.0;    // used with -s option
double   Border       = 2.0;    // used with -b option
double   Opacity      = 0.75;   // used with -o option
double   drumQ        = 0;      // used with --drum option
int      lineQ        = 0;      // used with -l option
int      staffQ       = 0;      // used with --staff option
int      diatonicQ    = 0;      // used with --diatonic option
int      grandQ       = 0;      // used with --gs option
int      transparentQ = 1;      // used with -T option
double   MaxRest      = 4.0;    // used with --max-rest option

// Function declarations:
void           checkOptions          (Options& opts, int argc, char* argv[]);
void           usage                 (const char* command);
void           example               (void);
void           convertMidiFileToSvg  (stringstream& output, MidiFile& midifile,
                                      Options& options);
int            hasNotes              (MidiEventList& eventlist);
void           getMinMaxPitch        (const MidiFile& midifile,
                                      int& minpitch, int &maxpitch);
void           getMinMaxTrackPitch   (const MidiEventList& evl,
                                      int& minpitch, int &maxpitch);
double         getMaxTime            (const MidiFile& midifile);
vector<double> getTrackHues          (MidiFile& midifile);
void           drawNote              (ostream& out, MidiFile& midifile,
                                      int i, int j, int dataQ,
                                      int minpitch, int maxpitch);
void           drawLines             (ostream& out, MidiFile& midifile,
                                      vector<double>& hues, Options& options);
void           printLineToNextNote   (ostream& out, MidiFile& midifile,
                                      int track, int index, Options& options);
void           drawStaves            (ostream& out, double staffwidth,
                                      const string& staffcolor,
                                      double totalduration);
int            base12ToBase7         (int pitch);
void           printDoubleClass      (ostream& out, double value);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   stringstream notes;
   int minpitch = -1;
   int maxpitch = -1;
   getMinMaxPitch(midifile, minpitch, maxpitch);

   convertMidiFileToSvg(notes, midifile, options);
   double aspectRatio = options.getDouble("aspect-ratio");

   double minx = 0;
   double miny = minpitch;
   double width = getMaxTime(midifile);
   double height = maxpitch - minpitch + 1;

   cout << "<?xml version=\"1.0\""
        << " encoding=\"UTF-8\""
        << " standalone=\"no\""
        << "?>\n";
   cout << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\""
        << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\""
        << ">\n";
   cout << "<svg"
        << " version=\"1.1\""
        << " xmlns=\"http://www.w3.org/2000/svg\""
        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        << " viewPort=\"" << minx << " " << miny  << " "
                          << width << " " << height << "\""
        << " viewBox=\"" << -Border * Scale << " "
                         << -Border * Scale << " "
                         << (width * aspectRatio + 2 * Border) * Scale << " "
                         << (height + (2 * Border)) * Scale << "\""
        << " width=\"" << (width * aspectRatio + 2 * Border) * Scale << "\""
        << " height=\"" << (height + 2 * Border) * Scale << "\""
        << ">\n";

   // Graphics setup:

   // This filter is used to show overlap between notes:
   if (transparentQ) {
      cout << "<filter id=\"constantOpacity\">\n";
      cout << "\t<feComponentTransfer>\n";
      cout << "\t\t<feFuncA type=\"table\" tableValues=\"0 .5 .5\" />\n";
      cout << "\t</feComponentTransfer>\n";
      cout << "</filter>\n";
   }

   if (darkQ) {
      cout << "<rect class=\"background\" x=\"-500%\" y=\"-500%\""
           << " width=\"1000%\" height=\"1000%\" style=\"fill:black\" />\n";
   }

   cout << "<g"
        << " transform=\""
        << "scale(" << 1 * Scale * aspectRatio << ", " << -Scale << ")"
        << " translate(0, " << -(maxpitch+1) << ")"
        << "\" >\n";


   // Print the piano roll note boxes:
   cout << notes.str();

   // Graphics setup end closing:
   cout << "</g>\n";
   cout << "</svg>\n";
   cout << "<?mid2svg\n\t";
   for (int i=1; i<argc; i++) {
      if (strchr(argv[i], ' ') != NULL) {
         cout << '"';
      }
      cout << argv[i];
      if (strchr(argv[i], ' ') != NULL) {
         cout << '"';
      }
      if (i<argc-1) {
         cout << " ";
      }
   }
   cout << "\n?>\n";
   return 0;
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// convetMidiFileToSvg --
//

void convertMidiFileToSvg(stringstream& output, MidiFile& midifile,
     Options& options) {

   midifile.linkNotePairs();    // first link note-ons to note-offs
   midifile.doTimeAnalysis();   // then create ticks to seconds mapping

   stringstream notes;

   if (staffQ) {
      drawStaves(notes, options.getDouble("staff-width"),
           options.getString("staff-color"), midifile.getTotalTimeInSeconds());
   }

   string strokecolor = options.getString("stroke-color");
   double strokewidth = options.getDouble("stroke-width") * Scale;
   notes << "\t<g"
         << " style=\""
         <<   "stroke-width:" << strokewidth << ";"
         <<   " stroke:" << strokecolor << ";"
         << "\""
         << ">\n";

   vector<double> trackhues = getTrackHues(midifile);

   if (lineQ) {
      drawLines(notes, midifile, trackhues, options);
   }

   int minpitch = 0;
   int maxpitch = 0;

   // Draw background for increasing contrast of notes and background
   // (needed due to constant opacity filter):
   int track = 0;
   for (int i=midifile.size()-1; i>=0; i--) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      getMinMaxTrackPitch(midifile[i], minpitch, maxpitch);
      track = i;
      notes << "\t\t<g"
            << " opacity=\"0.5\"";
      notes << " class=\"note-backdrops " << "track-" << i << "\"";
      if (trackhues[i] >= 0.0) {
          if (darkQ) {
             notes << " style=\""
                   << "fill:white;"
                   << "\"";
          } else {
             notes << " style=\""
                   << "fill:black;"
                   << "\"";
          }
      }
      notes << " >\n";
      for (int j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isNoteOn()) {
            continue;
         }
         if (!drumQ) {
           if (midifile[i][j].getChannel() == 0x09) {
              continue;
           }
         }
         drawNote(notes, midifile, i, j, 0, minpitch, maxpitch);
      }
      notes << "\t\t</g>\n";
   }

   // draw the actual notes:
   for (int i=midifile.size()-1; i>=0; i--) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      track = i;
      getMinMaxTrackPitch(midifile[i], minpitch, maxpitch);
      notes << "\t\t<g"
            << " class=\"track-" << track << "\"";
      if (transparentQ) {
         notes << " filter=\"url(#constantOpacity)\"";
      }
      if (trackhues[i] >= 0.0) {
          notes << " style=\""
                << "opacity:" << Opacity << ";"
                << " fill:hsl(" << trackhues[i] << ", 100%, 75%);"
                << "\"";
      }
      notes << " >\n";

      for (int j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isNoteOn()) {
            continue;
         }
         if (!drumQ) {
           if (midifile[i][j].getChannel() == 0x09) {
              continue;
           }
         }

         drawNote(notes, midifile, i, j, dataQ, minpitch, maxpitch);
      }
      notes << "\t\t</g>\n";
   }
   notes << "\t</g>\n";

   output << notes.str();
}



//////////////////////////////
//
// drawStaves --
//

void drawStaves(ostream& out, double staffwidth, const string& staffcolor,
      double totalduration) {
   vector<double> vpos;
   if (diatonicQ) {
      vpos.insert(vpos.end(), {37.5, 39.5, 41.5, 43.5, 45.5}); // treble clef
      vpos.insert(vpos.end(), {25.5, 27.5, 29.5, 31.5, 33.5}); // bass clef
   } else {
      vpos.insert(vpos.end(), {64.5, 67.5, 71.5, 74.5, 77.5}); // treble clef
      vpos.insert(vpos.end(), {43.5, 47.5, 50.5, 53.5, 57.5}); // bass clef
   }

   out << "\t<g"
       << " class=\"staff-lines\""
       << " stroke-width=\"" << staffwidth << "\""
       << " stroke=\"" << staffcolor << "\""
       << ">\n";

   for (int i=0; i<(int)vpos.size(); i++) {
      out << "\t\t<path"
          << " d=\"M" << 0 << " " << vpos[i]
          << " L" << totalduration << " " << vpos[i] << "\" />\n";
   }

   out << "\t</g>\n";
}



//////////////////////////////
//
// drawLines -- Draw lines to connect notes.
//

void drawLines(ostream& out, MidiFile& midifile, vector<double>& hues,  
      Options& options) {
   int counter = -1;
   int i, j;
   double linewidth = options.getDouble("line-width");
   int dashing = options.getBoolean("dash");
   int track = 0;
   for (i=midifile.size()-1; i>=0; i--) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      track = i;
      counter++;
      string color = "hsl(" + to_string(hues[i]) + ", 100%, 75%)";
      out << "\t\t<g"
          << " class=\"note-lines track-" << track << "\""
          << " fill=\"none\" stroke=\"" << color << "\"";
      // double scale = options.getDouble("scale");
      if (dashing) {
         double dwidth = 0.25;
         out << " stroke-dasharray=\"" << dwidth << "\"";
      }
      out << " stroke-width=\""<< linewidth << "\">\n";
      for (j=0; j<midifile[i].size(); j++) {
         if (!midifile[i][j].isNoteOn()) {
            continue;
         }
         printLineToNextNote(out, midifile, i, j, options);
      }
      out << "\t\t</g>\n";
   }



}



//////////////////////////////
//
// printLineToNextNote --
//

void printLineToNextNote(ostream& out, MidiFile& midifile, int track,
      int index, Options& options) {
   int p1 = midifile[track][index].getP1();
   if (diatonicQ) {
      p1 = base12ToBase7(p1);
   }
   double endtime = midifile[track][index].seconds
         + midifile[track][index].getDurationInSeconds();

   stringstream path;

   int nextindex = -1;
   for (int i=index+1; i<midifile[track].size(); i++) {
      if (!midifile[track][i].isNoteOn()) {
         continue;
      }
      if (midifile[track][i].seconds >= endtime) {
         nextindex = i;
         break;
      }
   }

   if (nextindex < 0) {
      return;
   }

   int p2 = midifile[track][nextindex].getP1();
   if (diatonicQ) {
      p2 = base12ToBase7(p2);
   }
   double nextstarttime = midifile[track][nextindex].seconds;
   double difference = nextstarttime - endtime;
   if (difference > MaxRest) {
      // don't connect notes after long rests.
      return;
   }
   if (difference > 0.0) {
      // there is a rest so extent the line horizontally
      path << " M" << endtime << " " << p1 + 0.5;
      path << " L" << nextstarttime << " " << p1 + 0.5;
      path << " L" << nextstarttime << " " << p2 + 0.5;
   } else {
      // vertical line:
      path << " M" << nextstarttime << " " << p1 + 0.5;
      path << " L" << nextstarttime << " " << p2 + 0.5;
   }


   out << "\t\t\t<path d=\"" << path.str() << "\" />\n";
}



//////////////////////////////
//
// drawNote --
//

void drawNote(ostream& out, MidiFile& midifile, int i, int j, int dataQ,
      int minpitch, int maxpitch) {
   int tickstart, tickend, tickdur;
   double starttime, endtime, duration;

   tickstart = midifile[i][j].tick;
   starttime = midifile[i][j].seconds;
   if (midifile[i][j].isLinked()) {
      tickdur  = midifile[i][j].getTickDuration();
      tickend  = tickstart + tickdur;
      duration = midifile[i][j].getDurationInSeconds();
      endtime  = starttime + duration;
   } else {
      tickdur = 0;
      tickend = tickstart;
      duration = 0.0;
      endtime = starttime;
   }
   int pitch    = midifile[i][j].getP1();
   int pitch12  = pitch;
   if (diatonicQ) {
      pitch = base12ToBase7(pitch);
   }
   int velocity = midifile[i][j].getP2();
   int channel  = midifile[i][j].getChannel();  // 0-offset
   int track    = i;                            // 0-offset

   if (dataQ) {
      out << "\t\t\t<!-- ==========================================" << endl;
      out << "\t\t\t\t@Track:      " << track                        << endl;
      out << "\t\t\t\t@Pitch:      " << pitch                        << endl;
      out << "\t\t\t\t@Minpitch:   " << minpitch                     << endl;
      out << "\t\t\t\t@Maxpitch:   " << maxpitch                     << endl;
      out << "\t\t\t\t@Velocity:   " << velocity                     << endl;
      out << "\t\t\t\t@Channel:    " << channel                      << endl;
      out << "\t\t\t\t@Start-tick: " << tickstart << " ticks"        << endl;
      out << "\t\t\t\t@End-tick:   " << tickend   << " ticks"        << endl;
      out << "\t\t\t\t@Tick-dur:   " << tickdur   << " ticks"        << endl;
      out << "\t\t\t\t@Start-time: " << starttime << " seconds"      << endl;
      out << "\t\t\t\t@End-time:   " << endtime   << " seconds"      << endl;
      out << "\t\t\t\t@Duration:   " << duration  << " seconds"      << endl;
      out << "\t\t\t=========================================== -->" << endl;
   }


   // note box:
   out     << "\t\t\t<rect"
            << " vector-effect=\"non-scaling-stroke\""
            << " class=\"note key-" << pitch12;

   out << " ont-";
   printDoubleClass(out, starttime);

   out << " offt-";
   printDoubleClass(out, starttime + duration);

   if (pitch <= minpitch) {
      out << " minima";
   }
   if (pitch >= maxpitch) {
      out << " maxima";
   }
   out      << "\"";
   if (roundedQ) {
      out << "\trx=\""     << 1          << "\""
          << "\try=\""     << 1          << "\"";
   }
   out << "\tx=\""      << starttime  << "\""
       << "\ty=\""      << pitch      << "\""
       << "\twidth=\""  << duration   << "\""
       << "\theight=\"" << 1          << "\""
       << " />\n";
}



//////////////////////////////
//
// base12ToBase7 -- MIDI to diatonic pitch name. Middle C is 5th octave.
//

int base12ToBase7(int pitch) {
   int octave = pitch / 12;
   int chroma = pitch % 12;
   int output = 0;
   switch (chroma) {
      case  0: output = 0; break; // C
      case  1: output = 0; break; // C#
      case  2: output = 1; break; // D
      case  3: output = 2; break; // Eb
      case  4: output = 2; break; // E
      case  5: output = 3; break; // F
      case  6: output = 3; break; // F#
      case  7: output = 4; break; // G
      case  8: output = 4; break; // G#
      case  9: output = 5; break; // A
      case 10: output = 6; break; // Bb
      case 11: output = 6; break; // B
   }
   return output + 7 * octave;
}



//////////////////////////////
//
// printDoubleClass -- print a double note with a "d" instead of 
//     decimal point.
//

void printDoubleClass(ostream& out, double value) {
   value = int(value * 1000.0 + 0.5)/1000.0;
   char buffer[32] = {0};
   sprintf(buffer, "%.3lf", value);
   char* decimal = strchr(buffer, '.');
   if (decimal != NULL) {
      decimal[0] = 'd';
   }
   out << buffer;
}



//////////////////////////////
//
// getTrackHues -- Assign track colors by maximally spaced hue.  Maybe
//   shuffle or randomize if there are a lot of tracks.
//

vector<double> getTrackHues(MidiFile& midifile) {
   vector<double> output;
   output.resize(midifile.size());
   fill(output.begin(), output.end(), -1);
   int tcount = 0;
   int i;
   for (i=0; i<midifile.size(); i++) {
      if (hasNotes(midifile[i])) {
         tcount++;
      }
   }
   int count = 0;
   double hue = 0;
   for (i=0; i<midifile.size(); i++) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      hue = (double)count / (double)tcount * 360.0;
      count++;
      output[i] = hue;
   }

   return output;
}



//////////////////////////////
//
// hasNotes -- Return true if a track has any notes-ons in it.
//

int hasNotes(MidiEventList& eventlist) {
   for (int i=0; i<eventlist.size(); i++) {
      if (eventlist[i].isNoteOn()) {
         if (drumQ) {
            return 1;
         } else if (eventlist[i].getChannel() != 0x09) {
            return 1;
         }
      }
   }
   return 0;
}



//////////////////////////////
//
// getMinMaxPitch -- Determine the minimum and maximum pitch in the file.
//

void getMinMaxPitch(const MidiFile& midifile, int& minpitch, int& maxpitch) {
   int key = 0;
   for (int i=0; i<midifile.size(); i++) {
      for (int j=0; j<midifile[i].size(); j++) {
         if (midifile[i][j].isNoteOn()) {
            key = midifile[i][j].getP1();
            if ((minpitch < 0) || (minpitch > key)) {
               minpitch = key;
            }
            if ((maxpitch < 0) || (maxpitch < key)) {
               maxpitch = key;
            }
         }
      }
   }

   if (grandQ) {
      if (minpitch > 40) {
         minpitch = 40;
      }
      if (maxpitch < 80) {
         maxpitch = 80;
      }
   }
   if (diatonicQ) {
      minpitch = base12ToBase7(minpitch);
      maxpitch = base12ToBase7(maxpitch);
   }
}



//////////////////////////////
//
// getMinMaxTrackPitch -- Determine the minimum and maximum pitch in a
//   particular Track.
//

void getMinMaxTrackPitch(const MidiEventList& evl, int& minpitch,
      int &maxpitch) {
   int key = 0;
   minpitch = -1.0;
   maxpitch = -1.0;
   for (int i=0; i<evl.size(); i++) {
      if (evl[i].isNoteOn()) {
         key = evl[i].getP1();
         if ((minpitch < 0) || (minpitch > key)) {
            minpitch = key;
         }
         if ((maxpitch < 0) || (maxpitch < key)) {
            maxpitch = key;
         }
      }
   }

   if (diatonicQ) {
      minpitch = base12ToBase7(minpitch);
      maxpitch = base12ToBase7(maxpitch);
   }
}



//////////////////////////////
//
// getMaxTime -- return the ending time of the last note in any track.
//

double getMaxTime(const MidiFile& midifile) {
   double maxtime = 0.0;
   for (int i=0; i<midifile.size(); i++) {
      for (int j=midifile[i].size()-1; j>=0; j--) {
         if (midifile[i][j].isNoteOff()) {
            if (maxtime < midifile[i][j].seconds) {
               maxtime = midifile[i][j].seconds;
            }
            break;
         }
      }
   }
   return maxtime;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("a|aspect-ratio=d:2.5", "Aspect ratio for SVG image");
   opts.define("w|stroke-width=d:0.1", "Stroke width for line around note boxes");
   opts.define("stroke-color=s:black", "Stroke color for line around note boxes");
   opts.define("staff=b",              "Draw staff lines.");
   opts.define("gs|grand|grand-staff=b", "show at least all grand staff.");
   opts.define("sc|staff-color=s:#555555", "staff line color.");
   opts.define("sw|st|staff-width|staff-thickness=d:0.1",    "staff line width.");
   opts.define("lw|lt|line-width|line-thickness=d:0.02",  "Width of note lines");
   opts.define("dash|dashing=b",       "Dash connecting lines");
   opts.define("T|no-transparency=b",  "Do not show notes with transparency");
   opts.define("s|scale=d:1.0",        "Scaling factor for SVG image");
   opts.define("d|data=b",             "Embed note data in SVG image");
   opts.define("diatonic=b",           "Vertical axis is base-7 pitch");
   opts.define("drum=b",               "Show drum track (channel 10)");
   opts.define("r|round|rounded=b",    "Round edges of note boxes");
   opts.define("b|border=d:1.0",       "Border around piano roll");
   opts.define("dark=b",               "Background is black");
   opts.define("o|opacity=d:1.0",      "Opacity for notes");
   opts.define("l|line=b",             "Draw lines between center of notes");
   opts.define("mr|rest|max-rest=d:4.0 seconds",
      "Maximum rest through which to draw lines");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 20 February 2016" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 20 February 2016" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().data());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand().data());
      exit(1);
   }

   dataQ        =  opts.getBoolean("data");
   drumQ        =  opts.getBoolean("drum");
   darkQ        =  opts.getBoolean("dark");
   lineQ        =  opts.getBoolean("line");
   staffQ       =  opts.getBoolean("staff");
   grandQ       =  opts.getBoolean("grand-staff");
   diatonicQ    =  opts.getBoolean("diatonic");
   transparentQ = !opts.getBoolean("no-transparency");
   roundedQ     =  opts.getBoolean("rounded");
   Scale        =  opts.getDouble("scale");
   Border       =  opts.getDouble("border");
   Opacity      =  opts.getDouble("opacity");
   MaxRest      =  opts.getDouble("max-rest");
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
   cout << "Usage: " << command << " input.mid > output.svg" << endl;
}



