//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Apr 16 07:25:01 PDT 2018
// Last Modified: Mon Apr 16 08:01:18 PDT 2018
// Filename:      midifile/tools/maxtick.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Calculate the maximum timestamp in a MIDI file.
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
	options.define("s|seconds=b",  "display total time in seconds");
	options.define("q|quarters=b", "display total time in quarter notes");
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
	cout << "maxtick=" << midifile.getFileDurationInTicks();
	if (options.getBoolean("quarters")) {
		cout << "\tquarters=" << midifile.getFileDurationInQuarters();
	}
	if (options.getBoolean("seconds")) {
		cout << "\tseconds=" << midifile.getFileDurationInSeconds();
	}
	cout << endl;
}



