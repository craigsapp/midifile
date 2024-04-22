//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jul  3 07:24:54 PDT 2023
// Last Modified: Mon Jul  3 07:24:58 PDT 2023
// Filename:      tools/midirange.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/midirange.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Note pitch range in data, highest note first, then lowest.
//                Ignoring channel 10 (0x09).
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
	midifile.linkNotePairs();
	midifile.joinTracks();

	int highest = -1;
	int lowest = 128;

	for (int e=0; e<midifile[0].getEventCount(); e++) {
		MidiEvent *note = &midifile[0][e];
		if (!note->isNoteOn()) {
			continue;
		}
		int channel  = note->getChannel();
		if (channel == 0x09) {
			continue;
		}
		int key = note->getKeyNumber();
		if (key < lowest) {
			lowest = key;
		}
		if (key > highest) {
			highest = key;
		}
	}

	int range = highest - lowest;
	cout << highest << "\t" << lowest << "\t" << range << endl;
}



