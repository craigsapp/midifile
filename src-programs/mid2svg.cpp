//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Feb 19 22:25:22 PST 2016
// Last Modified: Sun Feb 21 15:35:35 PST 2016
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

using namespace std;

// User interface variables:
Options options;
int     dataQ    = 0;       // used with -d option
int     roundedQ = 0;       // used with -r option
int     darkQ    = 0;       // used with --dark option
double  Scale    = 1.0;     // used with -s option
double  Border   = 2.0;     // used with -b option
double  Opacity  = 0.75;    // used with -o option
double  drumQ    = 0;       // used with --drum option

// Function declarations:
void           checkOptions          (Options& opts, int argc, char* argv[]);
void           usage                 (const char* command);
void           example               (void);
void           convertMidiFileToSvg  (stringstream& output, MidiFile& midifile,
                                      Options& options);
int            hasNotes              (MidiEventList& eventlist);
void           getMinMaxPitch        (const MidiFile& midifile,
                                      double& minpitch, double &maxpitch);
double         getMaxTime            (const MidiFile& midifile);
vector<double> getTrackHues          (MidiFile& midifile);
void           drawNote              (ostream& out, MidiFile& midifile,
                                      int i, int j, int dataQ);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   stringstream notes;
   double minpitch = -1.0;
   double maxpitch = -1.0;
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

   if (darkQ) {
      cout << "<rect x=\"-500%\" y=\"-500%\" width=\"1000%\" height=\"1000%\" style=\"fill:black\" />\n";
   }

   // Graphics setup:
   
   // This filter is used to show overlap between notes:
   cout << "<filter id=\"constantOpacity\">\n";
   cout << "  <feComponentTransfer>\n";
   cout << "    <feFuncA type=\"table\" tableValues=\"0 .5 .5\" />\n";
   cout << "  </feComponentTransfer>\n";
   cout << "</filter>\n";

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

   string strokecolor = options.getString("stroke-color");
   double strokewidth = options.getDouble("stroke-width") * Scale;
   notes << "<g"
         << " style=\""
         <<   "stroke-width:" << strokewidth << ";"
         <<   " stroke:" << strokecolor << ";"
         << "\""
         << ">\n";

   vector<double> trackhues = getTrackHues(midifile);


   // Draw background for increaing contrast of notes and background
   // (needed due to constant opacity filter):
   for (int i=0; i<midifile.size(); i++) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      notes << "\t<g"
            << " opacity=\"0.5\""
            << " class=\"note-background-" << i << "\"";
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
         drawNote(notes, midifile, i, j, 0);
      }
      notes << "\t</g>\n";
   }

   // draw the actual notes:
   for (int i=0; i<midifile.size(); i++) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      notes << "\t<g"
            << " class=\"track-" << i << "\""
            << " filter=\"url(#constantOpacity)\"";
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

         drawNote(notes, midifile, i, j, dataQ);
      }
      notes << "\t</g>\n";
   }
   notes << "</g>\n";

   output << notes.str();
}



//////////////////////////////
//
// drawNote --
//

void drawNote(ostream& out, MidiFile& midifile, int i, int j, int dataQ) {
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
   int velocity = midifile[i][j].getP2();
   int channel  = midifile[i][j].getChannel();  // 0-offset
   int track    = i;                            // 0-offset

   if (dataQ) {
      out << "\t\t\t<!-- ==========================================" << endl;
      out << "\t\t\t\t@Track:      " << track                        << endl;
      out << "\t\t\t\t@Pitch:      " << pitch                        << endl;
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
   out     << "\t\t<rect"
            << " vector-effect=\"non-scaling-stroke\""
            << " class=\"key-" << pitch << "\"";
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

void getMinMaxPitch(const MidiFile& midifile, double& minpitch,
      double &maxpitch) {
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
   opts.define("s|scale=d:1.0",        "Scaling factor for SVG image");
   opts.define("d|data=b",             "Embed note data in SVG image");
   opts.define("drum=b",               "Show drum track (channel 10)");
   opts.define("r|round|rounded=b",    "Round edges of note boxes");
   opts.define("b|border=d:1.0",       "Border around piano roll");
   opts.define("dark=b",               "Background is black");
   opts.define("o|opacity=d:1.0",     "Opacity for notes");

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

   dataQ    = opts.getBoolean("data");
   drumQ    = opts.getBoolean("drum");
   darkQ    = opts.getBoolean("dark");
   roundedQ = opts.getBoolean("rounded");
   Scale    = opts.getDouble("scale");
   Border   = opts.getDouble("border");
   Opacity  = opts.getDouble("opacity");
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



