//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb 23 05:34:17 PST 2016
// Last Modified: Tue Feb 23 05:34:20 PST 2016
// Filename:      midifile/tools/smfdur.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/tools/smfdur.cpp
// Syntax:        C++11
//
// Description:   Calcualte the total duration of a MIDI file.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

// User interface variables:
Options options;

// Function declarations:
void      checkOptions          (Options& opts, int argc, char* argv[]);
void      usage                 (const char* command);
void      example               (void);
double    getTotalDuration      (MidiFile& midifile);
string    minutes               (double seconds);


//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
	int fileQ  = options.getBoolean("filename");
	int minuteQ = options.getBoolean("minute");

   MidiFile midifile;

	int counter = 0;
	double sum = 0.0;
   int numinputs = options.getArgCount();
   for (int i=0; i < numinputs || i==0; i++) {
      midifile.clear();
      if (options.getArgCount() < 1) {
         midifile.read(cin);
      } else {
         midifile.read(options.getArg(i+1));
      }
      if (options.getArgCount() > 1) {
         cout << options.getArg(i+1) << "\t";
      }
		double duration = getTotalDuration(midifile);
		sum += duration;
		counter++;
      cout << duration;
		if (minuteQ) {
			cout << "\t" << minutes(duration);
		}
		if (fileQ) {
			cout << "\t";
			cout << midifile.getFilename();
		}
      cout << endl;
   }
	if (counter > 1) {
		cout << "TOTAL\t" << sum;
		if (minuteQ) {
			cout << "\t" << minutes(sum);
		}
		cout << endl;
		cout << "AVERAGE\t" << sum/counter;
		if (minuteQ) {
			cout << "\t" << minutes(sum/counter);
		}
		cout << endl;
	}
}


//////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// minutes -- display time in seconds as minutes mm:ss, or hh:mm:ss if 
//    an hour or longer
//

string minutes(double seconds) {
	int minutes = seconds / 60;
	seconds = seconds - minutes * 60;
	if (seconds - int(seconds) >= 0.5) {
		seconds = int(seconds) + 1;
	} else{
		seconds = int(seconds);
	}
	if (seconds == 60) {
		minutes++;
		seconds = 0;
	}
	int hours = minutes / 60;
	if (hours >= 1) {
		minutes = minutes - hours * 60;
	}

	string output;
	if (hours) {
		output += to_string(hours);
		output += ":";
		if (minutes < 10) {
			output += "0";
		}
	}
	output += to_string(minutes);
	output += ":";
	if (seconds < 10) {
		output += "0";
	}
	int isec = seconds;
	output += to_string(isec);
	return output;
}




//////////////////////////////
//
// getTotalDuration --
//

double getTotalDuration(MidiFile& midifile) {
   midifile.doTimeAnalysis();
   midifile.joinTracks();
   return midifile[0].last().seconds;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
	opts.define("f|filename=b", "display filename after duration");
	opts.define("m|minute|minutes=b", "also display duration in minutes after seconds display");

   opts.define("author=b",  "author of program");
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");
   opts.define("h|help=b",  "short description");
   opts.process(argc, argv);

   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 23 February 2016" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 23 February 2016" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand().c_str());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
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
   cout << "Usage: " << command << " input(s)" << endl;
}



