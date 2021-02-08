//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Feb  7 11:37:52 PST 2021
// Last Modified: Sun Feb  7 14:12:31 PST 2021
// Filename:      midifile/tools/deltatimes.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Generate list of delta times in a MIDI file, either as MIDI ticks,
//                quarter notes, or at real-time values in seconds or milliseconds
//                (why applying tempo meta messages to the tick values).  Useful
//                for determining if the MIDI file contains a score or a performance.
//
// Options:
//      -q        Output delta times as quarter note units instead of MIDI ticks.
//      -s        Output real times in seconds.
//      -m        Output real times in milliseconds.
//      -a        Output MIDI tick, quarter note, second and millisecond timings.
//      -c        Count timings by value (output a delta-time histogram).
//      -r        Reverse: place the count after the timing values in the histogram.
//      -n #      Normalize by the maximum count in the histogram scaled to number #.
//      -N #      Normalize by the sum of the histogram counts scaled to number #.
//      -C        Sort histogram by counts (largest to smallest).
//      --tpq     Print ticks-per-quarter note from MIDI header(s) only.
//      --notes   Calculate for MIDI notes only.
//      --on      Calculate for MIDI notes-ons only.
//

#include "MidiFile.h"
#include "Options.h"

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;
using namespace smf;

#define TIME_TICKS        0
#define TIME_SECONDS      1
#define TIME_MILLISECONDS 2
#define TIME_QUARTERS     3

// Function definitions:
void processFile(MidiFile& midifile, map<string, int>& histogram, Options& options);
void printHistogram(map<string, int>& histogram, Options& options);
void extractDeltaTimesForAllEvents(MidiFile& midifile, map<string, int>& histogram, Options& options);
void extractDeltaTimesForAllNotes(MidiFile& midifile, map<string, int>& histogram, Options& options);
void extractDeltaTimesForAllNotesOns(MidiFile& midifile, map<string, int>& histogram, Options& options);


///////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
	Options options;
	options.define("a|all=b", "output all types of time values for delta times");
	options.define("s|sec|second|seconds=b", "display times in seconds");
	options.define("m|ms|millisecond|milliseconds=b", "display times in milliseconds");
	options.define("q|quarter|quarters|quarter-notes|quarter-note=b", "display total time in quarter notes");
	options.define("c|count|counts=b", "display times in a histogram");
	options.define("n|normalize-by-max=d:100.0", "Normalize to max value in histogram");
	options.define("N|normalize-by-total=d:100.0", "Normalize to total count in histogram");
	options.define("r|reverse=b", "reverse histogram label/count order");
	options.define("C|sort-by-count=b", "sort histogram by counts rather than times");
	options.define("tpq=b", "print only the ticks-per-quarter note value from the MIDI header");
	options.define("notes|note=b", "delta times only between note on/offs");
	options.define("on|note-on|note-ons=b", "delta times only between note ons");
	options.process(argc, argv);

	MidiFile midifile;
	map<string, int> histogram;

	if (options.getArgCount() == 0) {
		midifile.read(cin);
		processFile(midifile, histogram, options);
	} else {
		int count = options.getArgCount();
		for (int i=0; i<count; i++) {
			string filename = options.getArg(i+1);
			midifile.read(filename);
			processFile(midifile, histogram, options);
		}
	}
	if (options.getBoolean("count") && !options.getBoolean("tpq")) {
		printHistogram(histogram, options);
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processFile -- Extract delta timings from the MIDI file, either
//    as raw ticks or as real times interpreted from tempo meta messages.
//

void processFile(MidiFile& midifile, map<string, int>& histogram, Options& options) {
	if (options.getBoolean("tpq")) {
			int tpq = midifile.getTicksPerQuarterNote();
			cerr << tpq << endl;
			return;
	}

	if (options.getBoolean("notes")) {
		extractDeltaTimesForAllNotes(midifile, histogram, options);
	} else if (options.getBoolean("on")) {
		extractDeltaTimesForAllNotesOns(midifile, histogram, options);
	} else {
		extractDeltaTimesForAllEvents(midifile, histogram, options);
	}
}



//////////////////////////////
//
// extractDeltaTimesForAllEvents -- calculate delta timings between all events.
//

void extractDeltaTimesForAllEvents(MidiFile& midifile, map<string, int>& histogram, 
		Options& options) {
	int tpq = midifile.getTicksPerQuarterNote();
	int timeFormat = TIME_TICKS;
	bool realtimeQ  = false;
	bool allQ = options.getBoolean("all");
	if (allQ) {
		realtimeQ = true;
	}
	if (options.getBoolean("seconds")) {
		timeFormat = TIME_SECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("milliseconds")) {
		timeFormat = TIME_MILLISECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("quarters")) {
		timeFormat = TIME_QUARTERS;
	}
	bool histogramQ = options.getBoolean("count");

	if (realtimeQ) {
		midifile.doTimeAnalysis();
	}

	midifile.makeDeltaTicks();

	for (int i=0; i<midifile.getTrackCount(); i++) {
		for (int j=0; j<midifile[i].getEventCount(); j++) {
			double value;
			double newtime;
			double oldtime;
			stringstream key;
			if (allQ) {
				key.str().clear();
				value = midifile[i][j].tick;
				key << value << "\t";         // MIDI ticks
				key << value/tpq << "\t";     // quarter notes
				newtime = midifile[i][j].seconds;
				if (j > 0) {
					oldtime = midifile[i][j-1].seconds;
				} else {
					oldtime = 0.0;
				}
				value = newtime - oldtime;
				key << value << "\t";         // seconds
				key << value * 1000.0;        // milliseconds
			} else if (realtimeQ) {
				newtime = midifile[i][j].seconds;
				if (j == 0) {
					oldtime = 0;
				} else {
					oldtime = midifile[i][j-1].seconds;
				}
				value = newtime - oldtime;
				if (timeFormat == TIME_MILLISECONDS) {
					value *= 1000.0;
				}
				key.str().clear();
				key << value;
			} else {
				value = midifile[i][j].tick;
				if (timeFormat == TIME_QUARTERS) {
					value /= tpq;
				}
				key.str().clear();
				key << value;
			}
			if (histogramQ) {
				histogram[key.str()]++;
			} else {
				cout << key.str() << endl;
			}
		}
	}
}



//////////////////////////////
//
// extractDeltaTimesForAllNotes -- calculate delta timings between all note MIDI
//     messages (ignoring other MIDI messages).
//

void extractDeltaTimesForAllNotes(MidiFile& midifile, map<string, int>& histogram, 
		Options& options) {
	int tpq = midifile.getTicksPerQuarterNote();
	int timeFormat = TIME_TICKS;
	bool realtimeQ  = false;
	bool allQ = options.getBoolean("all");
	int lastindex = -1;
	if (allQ) {
		realtimeQ = true;
	}
	if (options.getBoolean("seconds")) {
		timeFormat = TIME_SECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("milliseconds")) {
		timeFormat = TIME_MILLISECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("quarters")) {
		timeFormat = TIME_QUARTERS;
	}
	bool histogramQ = options.getBoolean("count");

	if (realtimeQ) {
		midifile.doTimeAnalysis();
	}

	for (int i=0; i<midifile.getTrackCount(); i++) {
		lastindex = -1;
		for (int j=0; j<midifile[i].getEventCount(); j++) {
			if (!midifile[i][j].isNote()) {
				continue;
			}
			if (lastindex < 0) {
				lastindex = j;
				continue;
			}
			double value;
			stringstream key;
			if (allQ) {
				key.str().clear();
				value = midifile[i][j].tick - midifile[i][lastindex].tick;
				key << value << "\t";         // MIDI ticks
				key << value/tpq << "\t";     // quarter notes
				value = midifile[i][j].seconds - midifile[i][lastindex].seconds;
				key << value << "\t";         // seconds
				key << value * 1000.0;        // milliseconds
			} else if (realtimeQ) {
				value = midifile[i][j].seconds - midifile[i][lastindex].seconds;
				if (timeFormat == TIME_MILLISECONDS) {
					value *= 1000.0;
				}
				key.str().clear();
				key << value;
			} else {
				value = midifile[i][j].tick - midifile[i][lastindex].tick;
				if (timeFormat == TIME_QUARTERS) {
					value /= tpq;
				}
				key.str().clear();
				key << value;
			}
			if (histogramQ) {
				histogram[key.str()]++;
			} else {
				cout << key.str() << endl;
			}
			lastindex = j;
		}
	}
}



//////////////////////////////
//
// extractDeltaTimesForAllNoteOns -- calculate delta timings between all note-on MIDI
//     messages (ignoring other MIDI messages).
//

void extractDeltaTimesForAllNotesOns(MidiFile& midifile, map<string, int>& histogram, 
		Options& options) {
	int tpq = midifile.getTicksPerQuarterNote();
	int timeFormat = TIME_TICKS;
	bool realtimeQ  = false;
	bool allQ = options.getBoolean("all");
	int lastindex = -1;
	if (allQ) {
		realtimeQ = true;
	}
	if (options.getBoolean("seconds")) {
		timeFormat = TIME_SECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("milliseconds")) {
		timeFormat = TIME_MILLISECONDS;
		realtimeQ  = true;
	} else if (options.getBoolean("quarters")) {
		timeFormat = TIME_QUARTERS;
	}
	bool histogramQ = options.getBoolean("count");

	if (realtimeQ) {
		midifile.doTimeAnalysis();
	}

	for (int i=0; i<midifile.getTrackCount(); i++) {
		lastindex = -1;
		for (int j=0; j<midifile[i].getEventCount(); j++) {
			if (!midifile[i][j].isNoteOn()) {
				continue;
			}
			if (lastindex < 0) {
				lastindex = j;
				continue;
			}
			double value;
			stringstream key;
			if (allQ) {
				key.str().clear();
				value = midifile[i][j].tick - midifile[i][lastindex].tick;
				key << value << "\t";         // MIDI ticks
				key << value/tpq << "\t";     // quarter notes
				value = midifile[i][j].seconds - midifile[i][lastindex].seconds;
				key << value << "\t";         // seconds
				key << value * 1000.0;        // milliseconds
			} else if (realtimeQ) {
				value = midifile[i][j].seconds - midifile[i][lastindex].seconds;
				if (timeFormat == TIME_MILLISECONDS) {
					value *= 1000.0;
				}
				key.str().clear();
				key << value;
			} else {
				value = midifile[i][j].tick - midifile[i][lastindex].tick;
				if (timeFormat == TIME_QUARTERS) {
					value /= tpq;
				}
				key.str().clear();
				key << value;
			}
			if (histogramQ) {
				histogram[key.str()]++;
			} else {
				cout << key.str() << endl;
			}
			lastindex = j;
		}
	}
}



//////////////////////////////
//
// printHistogram --
//

void printHistogram(map<string, int>& histogram, Options& options) {
	bool reverseQ = options.getBoolean("reverse");
	double scale = 1.0;
	if (options.getBoolean("normalize-by-max")) {
		int maximum = 0;
		for (auto it : histogram) {
			if (it.second > maximum) {
				maximum = it.second;
			}
		}
		if (maximum > 0) {
			scale = options.getDouble("normalize-by-max") / maximum;
		}
	} else if (options.getBoolean("normalize-by-total")) {
		int sum = 0;
		for (auto it : histogram) {
			sum += it.second;
		}
		if (sum > 0) {
			scale = options.getDouble("normalize-by-total") / sum;
		}
	}

	if (options.getBoolean("sort-by-count")) {
		// Sort by count, from lowest to highest:
		typedef function<bool(pair<int, string>, pair<int, string>)> Comparator;
		Comparator sortFunction = [](pair<int, string> a, pair<int, string> b) {
			if (a.first == b.first) {
				return stod(a.second) < stod(b.second);
			} else {
				return a.first > b.first;
			}
		};
		map<pair<int, string>, int, Comparator> sortedkeys(sortFunction);;
		for (auto it : histogram) {
			sortedkeys[make_pair(it.second, it.first)] = it.second;
		}
		for (auto it : sortedkeys) {
			string label = it.first.second;
			double count = it.second * scale;
			if (reverseQ) {
				cout << label << '\t' << count << endl;
			} else {
				cout << count << '\t' << label << endl;
			}
		}
	} else {
		// Sort by delta time, from lowest to highest:
		map<double, string, less<double>> sortedkeys;
		for (auto it : histogram) {
			sortedkeys[stod(it.first)] = it.first;
		}
		for (auto it : sortedkeys) {
			string label = it.second;
			double count = histogram[it.second] * scale;
			if (reverseQ) {
				cout << label << '\t' << count << endl;
			} else {
				cout << count << '\t' << label << endl;
			}
		}
	}
}



