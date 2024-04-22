//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jul  3 07:24:54 PDT 2023
// Last Modified: Mon Jul  3 07:24:58 PDT 2023
// Filename:      tools/midireg.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/midireg.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Categorize the number of notes in low, mid, high register
//                Ignoring channel 10 (0x09).  The default low register is
//                defined as notes lower than C3 (midi key number 48), and
//                the default definition of high notes are notes higher than
//                C5 (midi key number 72).
//                
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
	options.define("l|lower-boundary=i:48", "lowest note of mid-register");
	options.define("u|upper-boundary=i:72", "highest note of mid-register");
	options.define("s|split=i:60", "dual register split note (value included in higher range");
	options.define("f|fraction=b");
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

	int lowCut = options.getInteger("lower-boundary");
	int hiCut  = options.getInteger("upper-boundary");

	bool splitQ   = options.getBoolean("split");
	int  splitCut = options.getInteger("split");

	int lowCount = 0;
	int midCount = 0;
	int hiCount  = 0;

	bool fractionQ = options.getBoolean("fraction");

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
		if (splitQ) {
			if (key < splitCut) {
				lowCount++;
			} else {
				hiCount++;
			}
		} else if (key < lowCut) {
			lowCount++;
		} else if (key > hiCut) {
			hiCount++;
		} else {
			midCount++;
		}
	}

	double total = lowCount + midCount + hiCount;

	if (splitQ) {
		if (fractionQ) {
			cout << lowCount/total << "\t" << hiCount/total << endl;
		} else {
			cout << lowCount << "\t" << hiCount << endl;
		}
	} else {
		if (fractionQ) {
			cout << lowCount/total << "\t" << midCount/total << "\t" << hiCount/total << endl;
		} else {
			cout << lowCount << "\t" << midCount << "\t" << hiCount << endl;
		}
	}
}



