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
int      dataQ        = 0;         // used with -d option
int      roundedQ     = 0;         // used with -r option
int      darkQ        = 0;         // used with --dark option
int      bwQ          = 0;         // used with --bw option
double   Scale        = 1.0;       // used with -s option
double   Border       = 2.0;       // used with -b option
double   Opacity      = 0.75;      // used with -o option
double   drumQ        = 0;         // used with --drum option
int      lineQ        = 0;         // used with -l option
int      staffQ       = 0;         // used with --staff option
int      clefQ        = 0;         // used with --clef option
int      braceQ       = 0;         // used with --clef option
int      diatonicQ    = 0;         // used with --diatonic option
int      grandQ       = 0;         // used with --gs option
int      finalQ       = 0;         // used with -f option
int      doubleQ      = 0;         // used with --double option
int      transparentQ = 1;         // used with -T option
double   ClefFactor   = 6;         
double   StaffThickness = 2.0;     // used with --staff-width
double   LineThickness  = 2.0;     // used with --line-width
string   StaffColor   = "#555555"; // used with -staff-color
string   ClefColor    = "#555555"; // used with -clef-color
double   MaxRest      = 4.0;       // used with --max-rest option
double   EndSpace     = 0.0;       // used with -e option
int      percmapQ     = 0;         // used with --perc option
double   AspectRatio  = 2.5;       // used with -a option
vector<int> PercussionMap;         // used with --perc option
vector<string> Shapes;

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
void           makeMappings          (vector<int>& mapping,
                                      const string& mapstring);
void           drawNoteShape         (ostream& out, string& shape, double x, 
                                      double y, double width, double height);
void           drawRectangle         (ostream& out, double x, double y,
                                      double width, double height);
void           drawDiamond           (ostream& out, double x, double y, 
                                      double width, double height);
void           drawEyelid            (ostream& out, double x, double y, 
                                      double width, double height);
void           drawHexThick          (ostream& out, double x, double y, 
                                      double width, double height);
void           drawHexThin           (ostream& out, double x, double y, 
                                      double width, double height);
string         getTrackShape         (int track);
void           drawClefs             (ostream& out);
void           drawBrace             (ostream& out);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile(options.getArg(1));
   stringstream notes;
   int minpitch = -1;
   int maxpitch = -1;
   getMinMaxPitch(midifile, minpitch, maxpitch);

   convertMidiFileToSvg(notes, midifile, options);

   double minx = 0;
   double miny = minpitch;
   double width = getMaxTime(midifile);
   double height = maxpitch - minpitch + 1;

   double clefwidth = 0;
   if (clefQ) {
      clefwidth = 180 / 12;
   }

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
        << " viewBox=\"" << (-Border - clefwidth) * Scale << " "
                         << -Border * Scale << " "
                         << ((width + EndSpace) * AspectRatio + 2 * Border + 
                             clefwidth) * Scale << " "
                         << (height + (2 * Border)) * Scale << "\""
        << " width=\"" << ((width + EndSpace) * AspectRatio + 2 * Border + 
                            clefwidth) * Scale << "\""
        << " height=\"" << (height + 2 * Border) * Scale << "\""
        << ">\n";

   // Graphics setup:

   // This filter is used to show overlap between notes:
   if (transparentQ && !bwQ) {
      cout << "<filter id=\"constantOpacity\">\n";
      cout << "\t<feComponentTransfer>\n";
      cout << "\t\t<feFuncA type=\"table\" tableValues=\"0 .5 .5\" />\n";
      cout << "\t</feComponentTransfer>\n";
      cout << "</filter>\n";
   }

   if (darkQ && !bwQ) {
      cout << "<rect class=\"background\" x=\"-500%\" y=\"-500%\""
           << " width=\"1000%\" height=\"1000%\" style=\"fill:black\" />\n";
   }

   cout << "<g"
        << " transform=\""
        << "scale(" << 1 * Scale * AspectRatio << ", " << -Scale << ")"
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
      drawStaves(notes, StaffThickness, StaffColor,
          midifile.getTotalTimeInSeconds());
      if (clefQ) {
         drawClefs(notes);
      }
      if (braceQ) {
         drawBrace(notes);
      }
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
   if (!bwQ) {
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
             if (bwQ) {
                notes << " style=\""
                      << "fill:none;"
                      << "\"";
             } else if (darkQ) {
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
      if (transparentQ && !bwQ) {
         notes << " filter=\"url(#constantOpacity)\"";
      }
      if (trackhues[i] >= 0.0) {
          if (bwQ) {
             notes << " style=\""
                   << " fill:white;"
                   << "\"";
          } else {
             notes << " style=\""
                   << "opacity:" << Opacity << ";"
                   << " fill:hsl(" << trackhues[i] << ", 100%, 75%);"
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

         drawNote(notes, midifile, i, j, dataQ, minpitch, maxpitch);
      }
      notes << "\t\t</g>\n";
   }
   notes << "\t</g>\n";

   output << notes.str();
}



//////////////////////////////
//
// drawBrace -- Draw curley brace
//

void drawBrace(ostream& out) {
   double unscale = 2.5 / AspectRatio;
   string fill   = ClefColor;
   string stroke = ClefColor;
   if (bwQ) {
      fill   = "transparent";
   }
   double strokewidth = StaffThickness * ClefFactor;
   double xpos = 30.5;
   double ypos = -324;

   out << "<g transform=\"scale(" << unscale << ", 1)\">\n";

	out  <<
"	<g class=\"brace\" transform=\"rotate(0.2) translate(" << xpos << "," << ypos << ") scale(1,-1) scale(.0175)\">\n"
"		<path vector-effect=\"non-scaling-stroke\" stroke=\"" << stroke << "\" fill=\"" << fill << "\" stroke-width=\"" << strokewidth << "\" stroke-linejoin=\"round\" d=\"M-2031.812-22924.453\n"
"			c-24.919,88.91-36.494,171.418-39.024,279.09c-2.379,101.156,9.877,207.264,16.991,305.533\n"
"			c6.605,91.162,10.955,191.51-0.048,282.248c-2.757,22.732-8.614,43.227-13.597,63.254c-8.261,33.207-2.886,38.68,5.136,72.111\n"
"			c10.485,43.689,11.808,90.887,13.172,140.795c3.604,132-16.215,270.613-24.675,400.074\n"
"			c-8.818,134.883,3.651,255.861,34.113,366.164c-17.906-92.289-29.886-179.082-25.068-283.635\n"
"			c4.572-99.275,18.379-195.207,24.078-294.23c5.11-88.826,9.914-203.154-8.344-284.342c-4.91-21.82-10.217-42.227-15.835-62.98\n"
"			c-9.146-33.812-7.235-31.322,2.353-65.664c11.021-39.504,20.774-80.965,24.589-128.662c10.5-131.361-2.347-271.072-13.816-398.416\n"
"			c-5.957-66.148-10.878-134.006-7.34-202.238C-2055.575-22803.877-2044.086-22863.117-2031.812-22924.453L-2031.812-22924.453z\"/>\n"
"	</g>\n";

   out << "</g>\n";
}



//////////////////////////////
//
// drawClefs --
//

void drawClefs(ostream& out) {
   string fill        = ClefColor;
   string stroke      = ClefColor;
   double unscale = 2.5 / AspectRatio;
   double strokewidth = StaffThickness * ClefFactor;
   if (bwQ) {
      fill = "transparent";
   }
   
   out << "<g transform=\"scale(" << unscale << ", 1)\">\n";
	out  <<
"	<g vector-effect=\"non-scaling-stroke\" class=\"treble-clef\" stroke=\"" << stroke << "\" stroke-width=\"" << strokewidth << "\" fill=\"" << fill << "\" transform=\"translate(-17.25, 5.15) scale(0.06, 0.07)\">\n"
"		<path vector-effect=\"non-scaling-stroke\" d=\"M250.026,1080.111c-2.375-5.813-4.501-12.037-5.616-19.656c-1.07-7.32-1.387-15.609-1.62-23.113\n"
"			c-0.426-13.713,0.409-28.548,1.296-41.038c0.132-1.861,0.517-3.882,0.432-5.183c-0.118-1.794-1.622-3.834-2.484-5.401\n"
"			c-4.551-8.266-9.023-16.291-13.176-25.488c-3.56-7.883-6.46-16.235-8.316-27.648c-1.087-6.682-1.406-14.26-2.053-21.816"
"			c0-3.313,0-6.623,0-9.936c0.817-17.854,4.099-30.916,9.396-39.743c1.084-1.808,2.228-3.951,3.24-5.186\n"
"			c6.218-7.579,16.081-9.873,25.812-7.343c0.598-8.887,1.858-19.53,2.376-28.944c0.193-3.49,0.54-7.194,0.54-10.368\n"
"			c-0.003-13.193-3.44-20.603-7.452-26.135c-3.645-2.613-9.925-3.099-13.176,0.43c4.904-0.174,8.174,5.425,9.612,12.744\n"
"			c2.427,12.351-1.738,24.386-7.344,25.918c-8.458,2.316-13.738-12.691-11.556-28.295c1.622-11.598,6.603-18.42,13.176-19.655\n"
"			c5.073-0.956,10.149,0.426,13.608,5.399c1.988,2.858,4.009,7.685,4.968,11.879c1.246,5.448,2.18,12.212,2.16,18.792\n"
"			c-0.01,3.246-0.453,6.761-0.648,10.152c-0.614,10.689-1.579,19.729-2.376,29.812c6.095,5.437,10.941,13.432,13.392,25.704\n"
"			c1.302,6.52,2.035,15.517,1.727,23.111c-0.375,9.302-2.565,17.712-5.183,23.975c-3.863,9.248-9.852,13.985-17.604,14.257\n"
"			c-0.849,9.749-1.633,19.633-2.593,29.159c2.376,4.619,4.55,9.643,6.805,14.906c2.185,5.095,3.892,11.286,5.508,17.712\n"
"			c2.98,11.855,5.576,29.004,5.4,46.44c-0.102,10.157-1.538,19.319-3.24,27.215c-1.691,7.847-3.557,15.81-6.912,20.089\n"
"			c-0.36,0-0.72,0-1.08,0C254.113,1090.306,252.068,1085.104,250.026,1080.111z M248.19,997.6\n"
"			c-1.362,15.429-0.295,36.475,2.7,48.384c1.367,5.441,2.426,10.329,4.428,15.122c0.854,2.04,3.006,5.909,4.104,5.83\n"
"			c2.104-0.153,3.064-9.245,3.24-13.824c0.542-14.092-2.623-28.387-5.832-37.366C254.247,1008.515,250.911,1002.572,248.19,997.6z\n"
"			 M249.27,935.823c-6.565-5.809-13.02-17.569-13.716-34.776c-0.425-10.517,1.463-18.693,3.888-24.624\n"
"			c1.422-3.478,3.789-6.692,6.156-9.072c1.287-1.293,4.09-3.666,3.996,1.296c-0.048,2.584-2.504,4.06-3.888,6.263\n"
"			c-2.557,4.079-4.489,12.399-3.348,21.385c0.481,3.784,1.636,7.983,2.808,10.153c1.458,2.697,3.725,4.34,6.048,5.615\n"
"			c1.987-19.137,3.451-39.323,5.292-58.753c-4.493-1.347-9.224-1.49-13.068-0.213c-6.922,2.298-11.753,12.045-14.256,23.111\n"
"			c-1.838,8.128-2.944,20.546-1.837,31.752c0.529,5.353,1.939,11.271,3.348,15.984c4.113,13.751,11.055,26.606,16.308,36.289\n"
"			C247.971,952.523,248.466,943.862,249.27,935.823z M260.286,855.473c-1.479,19.361-3.545,38.701-4.86,57.671\n"
"			c5.07-0.638,9.023-3.541,11.556-9.718c4.25-10.369,3.378-29.217-0.648-39.53C264.924,860.283,262.694,856.868,260.286,855.473z\"/>\n"
"	</g>\n";

	out << 
"	<g class=\"bass-clef\" fill=\"" << fill << "\" stroke=\"" << stroke << "\" transform=\"scale(1.02, 1) translate(-0.25, 0.25)\" stroke-width=\"" << strokewidth << "\">\n"
"		<g transform=\"matrix(0.1835537,0,0,0.1830159,-98.297967,-1128.8415)\">\n"
"			<path vector-effect=\"non-scaling-stroke\" d=\"M514.308,6407.837c3.419,4.086,5.655,9.64,7.124,12.803\n"
"				c2.204,4.648,4.046,10.097,5.538,16.384c1.492,6.287,2.238,13.068,2.238,20.385c0,4.115-0.338,7.925-0.991,11.507\n"
"				c-0.653,3.543-1.527,6.4-2.647,8.459c-1.108,2.095-2.297,3.125-3.545,3.125c-1.142,0-2.262-0.687-3.369-2.135\n"
"				c-1.119-1.41-2.029-3.505-2.752-6.325c-0.711-2.819-1.073-6.135-1.073-9.982c0-3.011,0.326-5.411,0.968-7.317\n"
"				c0.653-1.867,1.446-2.78,2.379-2.78c0.513,0,0.991,0.342,1.422,1.104c0.431,0.724,0.769,1.753,1.014,3.011\n"
"				c0.245,1.257,0.373,2.628,0.373,4.077c0,2.019-0.245,3.733-0.735,5.144c-0.501,1.41-1.049,2.132-1.667,2.132\n"
"				c-0.21,0-0.466-0.152-0.781-0.38c-0.303-0.229-0.501-0.342-0.571-0.342c-0.14,0-0.292,0.152-0.466,0.495\n"
"				c-0.175,0.342-0.268,0.799-0.292,1.41c0.07,0.495,0.117,0.838,0.117,1.104c0.338,2.896,0.944,4.953,1.796,6.249\n"
"				c0.863,1.258,1.784,1.905,2.763,1.905c1.014,0,1.83-0.953,2.46-2.858c0.63-1.943,1.073-4.344,1.329-7.277\n"
"				c0.268-2.896,0.396-5.981,0.396-9.22c-0.035-3.468-0.221-7.125-0.583-10.897c-0.361-3.81-0.804-7.163-1.317-10.059\n"
"				c-0.653-3.925-1.458-7.468-2.414-10.593c-0.956-3.124-1.889-5.752-2.775-7.849c-0.886-2.133-2.731-5.973-4.104-9.297\n"
"				C514,6409.468,514.126,6407.62,514.308,6407.837z\"/>\n"
"			<path vector-effect=\"non-scaling-stroke\" d=\"M531.259,6465.557c0.313-0.979,0.705-1.415,1.189-1.415\n"
"				c0.457,0,0.862,0.436,1.228,1.306c0.339,0.908,0.522,2.032,0.522,3.339c0,1.233-0.17,2.358-0.522,3.302\n"
"				c-0.353,0.979-0.745,1.488-1.163,1.488c-0.483,0-0.888-0.436-1.215-1.343c-0.34-0.871-0.509-1.958-0.509-3.266\n"
"				C530.789,6467.626,530.945,6466.5,531.259,6465.557z\"/>\n"
"			<path vector-effect=\"non-scaling-stroke\" d=\"M531.272,6447.847c0.327-0.871,0.732-1.342,1.241-1.342\n"
"				c0.457,0,0.849,0.471,1.189,1.378c0.327,0.908,0.497,2.033,0.497,3.447c0,1.162-0.183,2.214-0.522,3.193\n"
"				c-0.366,0.98-0.745,1.452-1.163,1.452c-0.509,0-0.914-0.436-1.241-1.343c-0.313-0.87-0.483-1.996-0.483-3.301\n"
"				C530.789,6449.879,530.958,6448.717,531.272,6447.847z\"/>\n"
"		</g>\n"
"	</g>\n";

   out << "</g>\n";

}



//////////////////////////////
//
// drawStaves --
//

void drawStaves(ostream& out, double staffwidth, const string& staffcolor,
      double totalduration) {
   vector<double> vpos;
   double unscale = 2.5 / AspectRatio;
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

   double start = 0.0;
   if (clefQ) {
      start = -4.65 * unscale;
   }
   double endx = totalduration + EndSpace;
   for (int i=0; i<(int)vpos.size(); i++) {
      out << "\t\t<path vector-effect=\"non-scaling-stroke\""
          << " d=\"M" << start << " " << vpos[i]
          << " L" << endx << " " << vpos[i] << "\" />\n";
   }

   double maxy = *max_element(vpos.begin(), vpos.end());
   double miny = *min_element(vpos.begin(), vpos.end());
   double thickness = 0.5 * unscale;
   if (finalQ) {
      out << "\t\t<path stroke=\"#cccccc\" fill=\"#cccccc\""
          << " d=\"M" << endx << "," << miny
          << " L"  << endx << "," << maxy
          << " L"  << endx - thickness << "," << maxy
          << " L"  << endx - thickness << "," << miny
          << " z"
          << "\"/>\n";

      out << "\t\t<path stroke=\"" << StaffColor << "\" fill=\"" << StaffColor << "\""
          << " d=\"M" << endx-thickness-thickness/2.0 << "," << miny
          << " L" << endx-thickness-thickness/2.0 << "," << maxy
          << "\"/>\n";
   } else if (doubleQ) {
      out << "\t\t<path vector-effect=\"non-scaling-stroke\""
          << " d=\"M" << endx-thickness << "," << miny
          << " L" << endx-thickness << "," << maxy
          << "\"/>\n";
      out << "\t\t<path vector-effect=\"non-scaling-stroke\""
          << " d=\"M" << endx << "," << miny
          << " L" << endx << "," << maxy
          << "\"/>\n";
   }

   if (braceQ) {
      staffwidth = 5 * staffwidth;
      out << "\t\t<path vector-effect=\"non-scaling-stroke\""
          << " d=\"M" << start << "," << miny 
          << " L"  << start << "," << maxy
          << " z"
          << "\"/>\n";
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
   int dashing = options.getBoolean("dash");
   int track = 0;
   for (i=midifile.size()-1; i>=0; i--) {
      if (!hasNotes(midifile[i])) {
         continue;
      }
      track = i;
      counter++;
      string color = "hsl(" + to_string(hues[i]) + ", 100%, 75%)";
      if (bwQ) {
         color = StaffColor;
      }
      out << "\t\t<g"
          << " class=\"note-lines track-" << track << "\""
          << " fill=\"none\" stroke=\"" << color << "\"";

      // double scale = options.getDouble("scale");
      if (dashing) {
         double dwidth = 2.25;
         out << " stroke-dasharray=\"" << dwidth << "\"";
         out << " vector-effect=\"non-scaling-stroke\"";
      }
      out << " stroke-width=\""<< LineThickness << "\">\n";
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
   if (midifile[track][index].getChannel() == 9) {
      p1 = PercussionMap[p1];
   }
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
   if (midifile[track][nextindex].getChannel() == 9) {
      p2 = PercussionMap[p2];
   }
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


   out << "\t\t\t<path ";
   out << " vector-effect=\"non-scaling-stroke\"";
   out << " d=\"" << path.str() << "\" />\n";
}



//////////////////////////////
//
// drawNote --
//

void drawNote(ostream& out, MidiFile& midifile, int i, int j, int dataQ,
      int minpitch, int maxpitch) {
   int tickstart, tickend, tickdur;
   double starttime, endtime, duration;
   int height = 1;
   tickstart  = midifile[i][j].tick;
   starttime  = midifile[i][j].seconds;
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
   if (midifile[i][j].getChannel() == 9) {
      pitch = PercussionMap[pitch];
   }
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
   out     << "\t\t\t<g"
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
   out << "\"";
   out << ">\n";

   // string shape = "diamond";
   // string shape = "rectangle";
   // string shape = "eyelid";
   // string shape = "hexthin";
   // string shape = "hexthick";
   string shape = getTrackShape(track);
   drawNoteShape(out, shape, starttime, pitch, duration, height);

   out << "\t\t\t</g>\n";
}



//////////////////////////////
//
// getTrackShape --
//

string getTrackShape(int track) {
   if (track < 0) {
      track = 0;
   } 
   if (track > 0) {
      track = track - 1;
   }
   if (track < (int)Shapes.size()) {
      return Shapes[track];
   } else {
      return "rectangle";
   }
}



//////////////////////////////
//
// drawNoteShape -- Draw the desired note shape.
//

void drawNoteShape(ostream& out, string& shape, double x, double y,
      double width, double height) {
   if (shape == "rectangle") {
      drawRectangle(out, x, y, width, height);
   } else if (shape == "diamond") {
      drawDiamond(out, x, y, width, height);
   } else if (shape == "eyelid") {
      drawEyelid(out, x, y, width, height);
   } else if (shape == "hexthin") {
      drawHexThin(out, x, y, width, height);
   } else if (shape == "hexthick") {
      drawHexThick(out, x, y, width, height);
   } else {
      drawRectangle(out, x, y, width, height);
   }
}



//////////////////////////////
//
// drawDiamond --
//

void drawDiamond(ostream& out, double x, double y, double width,
      double height) {
   out << "\t\t\t\t<path vector-effect=\"non-scaling-stroke\" d=\"";

   out << "M "  << x           << " " << y+height/2.0;
   out << " L " << x+width/2.0 << " " << y;
   out << " L " << x+width     << " " << y+height/2.0;
   out << " L " << x+width/2.0 << " " << y+height;
   out << " z";
   out << "\"";
   out << "/>\n";

   //if (roundedQ) {
   //   out << "\trx=\""  << 1      << "\""
   //       << "\try=\""  << 1      << "\"";
   //}
}



//////////////////////////////
//
// drawEyelid --
//

void drawEyelid(ostream& out, double x, double y, double width,
      double height) {
   out << "\t\t\t\t<path vector-effect=\"non-scaling-stroke\" d=\"";
   out << "M "  << x           << " " << y+height/2.0;
   out << " Q " << x+width/2.0 << " " << y+height;
   out << " "   << x+width     << " " << y+height/2.0;
   out << " Q " << x+width/2.0 << " " << y;
   out << " "   << x           << " " << y+height/2.0;
   out << " z\"";
   out << "/>\n";
}



//////////////////////////////
//
// drawHexThin --
//

void drawHexThin(ostream& out, double x, double y, double width,
      double height) {
   double& h = height;
   double& w = width;
   out << "\t\t\t\t<path vector-effect=\"non-scaling-stroke\" d=\"";
   out << "M "  << x           << " " << y+h/3.0;
   out << " L " << x           << " " << y+h*2.0/3.0;
   out << " L " << x+w/2.0     << " " << y+h;
   out << " L " << x+w         << " " << y+h*2.0/3.0;
   out << " L " << x+w         << " " << y+h/3.0;
   out << " L " << x+w/2.0     << " " << y;
   out << " z\"";
   out << "/>\n";
}



//////////////////////////////
//
// drawHexThick --
//

void drawHexThick(ostream& out, double x, double y, double width,
      double height) {
   double& h = height;
   double& w = width;
   double maxbevel = h/2.0/AspectRatio;
   if (width*AspectRatio < height) {
      maxbevel = w/2.0/AspectRatio;
   }
   double& b = maxbevel;
   out << "\t\t\t\t<path vector-effect=\"non-scaling-stroke\" d=\"";
   out << "M "  << x           << " " << y+h/2.0;
   out << " L " << x+b         << " " << y+h;
   out << " L " << x+w-b       << " " << y+h;
   out << " L " << x+w         << " " << y+h/2.0;
   out << " L " << x+w-b       << " " << y;
   out << " L " << x+b         << " " << y;
   out << " z\"";
   out << "/>\n";
}



//////////////////////////////
//
// drawRectangle --
//

void drawRectangle(ostream& out, double x, double y, double width,
      double height) {
   out << "\t\t\t\t<rect vector-effect=\"non-scaling-stroke\"";
   if (roundedQ) {
      out << "\trx=\""  << 1      << "\""
          << "\try=\""  << 1      << "\"";
   }
   out << "\tx=\""      << x      << "\""
       << "\ty=\""      << y      << "\""
       << "\twidth=\""  << width  << "\""
       << "\theight=\"" << height << "\""
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
   opts.define("cc|clef-color=s:#cdcdcd", "cleff fill/stroke color.");
   opts.define("sw|st|staff-width|staff-thickness=d:0.5",    "staff line width.");
   opts.define("lw|lt|line-width|line-thickness=d:0.5",  "Width of note lines");
   opts.define("dash|dashing=b",       "Dash connecting lines");
   opts.define("T|no-transparency=b",  "Do not show notes with transparency");
   opts.define("s|scale=d:1.0",        "Scaling factor for SVG image");
   opts.define("d|data=b",             "Embed note data in SVG image");
   opts.define("f|final|final-barline=b", "draw final barline");
   opts.define("double|double-barline=b", "draw final double barline");
   opts.define("bw|black-and-white=b", "Display as black and white (outlines only)");
   opts.define("diatonic=b",           "Vertical axis is base-7 pitch");
   opts.define("drum=b",               "Show drum track (channel 10)");
   opts.define("pm|perc|percussion-map=s", "Map percussion notes to different pitch");
   opts.define("r|round|rounded=b",    "Round edges of note boxes");
   opts.define("b|border=d:1.0",       "Border around piano roll");
   opts.define("dark=b",               "Background is black");
   opts.define("o|opacity=d:1.0",      "Opacity for notes");
   opts.define("l|line=b",             "Draw lines between center of notes");
   opts.define("e|end-space=d:0.0",    "extra horiz. space at end of piece");
   opts.define("c|clef|clefs=b",       "Draw clefs");
   opts.define("S|shapes=s:rectangle,rectangle", "shape of notes for each track");
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
   clefQ        =  opts.getBoolean("clefs");
   staffQ       =  opts.getBoolean("staff");
   grandQ       =  opts.getBoolean("grand-staff");
   bwQ          =  opts.getBoolean("black-and-white");
   diatonicQ    =  opts.getBoolean("diatonic");
   transparentQ = !opts.getBoolean("no-transparency");
   roundedQ     =  opts.getBoolean("rounded");
   Scale        =  opts.getDouble("scale");
   Border       =  opts.getDouble("border");
   AspectRatio  =  opts.getDouble("aspect-ratio");
   Opacity      =  opts.getDouble("opacity");
   MaxRest      =  opts.getDouble("max-rest");
   if (clefQ) {
      staffQ = 1;
      braceQ = 1;
   }

   PercussionMap.resize(128);
   for (int i=0; i<(int)PercussionMap.size(); i++) {
      PercussionMap[i] = i;
   }
   percmapQ     =  opts.getBoolean("percussion-map");
   if (percmapQ) {
      makeMappings(PercussionMap, opts.getString("percussion-map"));
      drumQ = 1;
   }

   finalQ   = opts.getBoolean("final-barline");
   doubleQ = opts.getBoolean("double-barline");
   EndSpace = opts.getDouble("end-space");
   if (finalQ || doubleQ) {
      EndSpace += 2.0;
   }
   StaffThickness = opts.getDouble("staff-width");
   if (bwQ) {
       StaffThickness = StaffThickness * 0.25;
   }
   StaffColor = opts.getString("staff-color");
   ClefColor = opts.getString("clef-color");
   if (!opts.getBoolean("line-width")) {
      LineThickness = StaffThickness;
   }

   char buffer[12345] = {0};
   strcpy(buffer, opts.getString("shapes").c_str());
   Shapes.resize(0);
   string current;
   const char* spacers = "\t :,;|\n";
   char* ptr = strtok(buffer, spacers);
   while (ptr != NULL) {
      current = ptr;
      if (current == "r") {
         Shapes.push_back("rectangle");
      } else if (current == "e") {
         Shapes.push_back("eyelid");
      } else if (current == "d") {
         Shapes.push_back("diamond");
      } else if (current == "h") {
         Shapes.push_back("hexthin");
      } else if (current == "H") {
         Shapes.push_back("hexthick");
      } else {
         Shapes.push_back(current);
      }
      ptr = strtok(NULL, spacers);
   }
}



//////////////////////////////
//
// makeMappings -- Manually move percussion notes to other locations in pitch range.
//     Maybe add separate coloring of percussion instruments.
//     Maybe need to add automatic note offs for persussion instruments (as they
//     do not always have note-offs.
//
//   --perc "60>40, 61>51,62>44"
//

void makeMappings(vector<int>& mapping, const string& mapstring) {
   string newmap = mapstring + ' ';
   int ltx = 0;
   int d = 1;
   int digit1 = 0;
   int digit2 = 0;
   int ii;
   for (ii=0; ii<(int)newmap.size(); ii++) {
      if (isdigit(newmap[ii])) {
         break;
      }
   }
   for (int i=ii; i<(int)newmap.size(); i++) {
      if (isdigit(newmap[i])) {
        if (d) {
          digit1 = digit1 * 10 + (newmap[i] - '0');
        } else {
          digit2 = digit2 * 10 + (newmap[i] - '0');
        }
        ltx = 0;
      } else {
         if (!ltx) {
            d = !d;
         }
         ltx = 1;
         if (d) {
            digit1 = digit1 > 127 ? 127 : digit1;
            digit1 = digit1 <   0 ?   0 : digit1;
            digit2 = digit2 > 127 ? 127 : digit2;
            digit2 = digit2 <   0 ?   0 : digit2;
            mapping[digit1] = digit2;
            digit1 = 0;
            digit2 = 0;
         }
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
   cout << "Usage: " << command << " input.mid > output.svg" << endl;
}



