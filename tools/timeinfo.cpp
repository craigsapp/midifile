//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Feb  4 12:00:25 PM PST 2025
// Last Modified: Tue Feb  4 12:00:29 PM PST 2025
// Filename:      tools/timeinfo.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/timeinfo.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Display the absolute time for note-ons in both ticks and seconds.
//                MIDI file is merged into a type-0 file first for a single timeline.
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


void processFile(MidiFile& midifile, Options& options);
void printTrackInfo(MidiEventList& midifile, int tpq);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.process(argc, argv);
	MidiFile midifile;
	if (options.getArgCount() == 0) {
		midifile.read(cin);
		processFile(midifile, options);
	} else {
		int count = options.getArgCount();
		for (int i=0; i<count; i++) {
			string filename = options.getArg(i+1);
			if (count > 1) {
				cout << filename << "\t";
			}
			midifile.read(filename);
			processFile(midifile, options);
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processFile --
//

void processFile(MidiFile& midifile, Options& options) {
	midifile.doTimeAnalysis();
	midifile.joinTracks();  // convert to Type-0 MIDI file
	int tpq = midifile.getTPQ();
	printTrackInfo(midifile[0], tpq);
}


//////////////////////////////
//
// printTrackInfo --
//

void printTrackInfo(MidiEventList& events, int tpq) {
	cout << "# TPQ = " << tpq << endl;
	cout << "# TICK\tSEC" << endl;
	for (int i=0; i<events.getEventCount(); i++) {
		if (events[i].isTempo()) {
			double tempo = events[i].getTempoBPM();
			int    micro = events[i].getTempoMicroseconds();
			cout << "# TEMPO = " << tempo << "\t" << micro << endl;
		} else if (events[i].isNoteOn()) {
			double time_tick = events[i].tick;
			double time_sec = events[i].seconds;
			cout << time_tick << "\t" << time_sec << endl;
		}
	}
}



