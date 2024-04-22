//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun 29 14:05:26 PDT 2023
// Last Modified: Thu Jun 29 14:05:31 PDT 2023
// Filename:      tools/midiuniq.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/midiuniq.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   When notes attacks for the same pitch occur at
//                the same time, remove one of them.
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


void processFile(MidiFile& midifile, Options& options);


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
		if (count != 2) {
			cerr << "Usage: << options.getCommand input.mid output.mid" << endl;
			exit(1);
		}
		string filename = options.getArg(1);
		midifile.read(filename);
		processFile(midifile, options);
		midifile.write(options.getArg(2));
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////
//
// processFile --
//

void processFile(MidiFile& midifile, Options& options) {
	midifile.linkNotePairs();
	midifile.joinTracks();

	MidiEvent* lastNote = NULL;
	for (int e=0; e<midifile[0].getEventCount(); e++) {
		MidiEvent *curNote = &midifile[0][e];
		if (!curNote->isNoteOn()) {
			continue;
		}
		if (!lastNote) {
			lastNote = curNote;
			continue;
		}
		int tick = curNote->tick;
		if (tick != lastNote->tick) {
			lastNote = curNote;
			continue;
		}
		int pitch = curNote->getKeyNumber();
		if (pitch != lastNote->getKeyNumber()) {
			lastNote = curNote;
			continue;
		}

		// Notes are the same pitch at the same time
		// so remvove the shorter one (maybe enhand to be
		// in the same channel as an option?)
		int cdur = curNote->getTickDuration();
		int odur = lastNote->getTickDuration();
		if (cdur < odur) {
			cerr << "DELETEING CURNOTE" << endl;
			curNote->clear();
		} else {
			cerr << "DELETEING LASTNOTE" << endl;
			lastNote->clear();
		}
	}
}



