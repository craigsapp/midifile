//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jan  8 23:44:16 PST 2018
// Last Modified: Mon Jan  8 23:44:22 PST 2018
// Filename:      retick.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/tools/retick.cpp
// Syntax:        C++; museinfo
// vim:           ts=3
//
// Description:   Change TPQ to a new value and update timestamps for new TPQ to keep
//                time values the same as before.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

// user interface variables

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	Options options;
	options.define("t|tpq=i:120", "Set TPQ to this value and adjust timestamps");
	options.process(argc, argv);

	int status;
	MidiFile midifile;
	if (options.getArgCount()) {
		status = midifile.read(options.getArg(1));
	} else {
		status = midifile.read(cin);
	}
	if (status == 0) {
		cerr << "Error: could not read MIDI file" << endl;
		exit(1);
	}

	int tpq = midifile.getTicksPerQuarterNote();
	int newtpq = options.getInteger("tpq");
	double factor = (double)newtpq / (double)tpq;

	for (int track=0; track<midifile.getTrackCount(); track++) {
		for (int event=0; event < midifile.getEventCount(track); event++) {
			MidiEvent& ev = midifile.getEvent(track, event);
			ev.tick = int(ev.tick * factor + 0.5);
		}
	}

	midifile.setTPQ(newtpq);

	if (options.getArgCount() > 1) {
		midifile.write(options.getArg(2));
	} else {
		cout << midifile;
	}

	return 0;
}


