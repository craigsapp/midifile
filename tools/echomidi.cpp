//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Feb 13 12:07:10 PST 2025
// Last Modified: Thu Feb 13 12:07:14 PST 2025
// Filename:      tools/echomidi.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/echomidi.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Read in a MIDI file and output it with minimal processing.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

void processTrack(MidiEventList& list);

int main(int argc, char** argv) {
	Options options;
	options.define("v|verbose=b", "display input/output in binasc format");
	options.process(argc, argv);
	if (options.getArgCount() < 1) {
		cerr << "Usage: " << options.getCommand() << "input.mid [output.mid]" << endl;
		exit(1);
	}
	MidiFile mfile(options.getArg(1));

	if (options.getArgCount() >= 2) {
		mfile.write(options.getArg(2));
	} else {
		// Output as in binasc form to the console:
		mfile.writeBinasc(cout);
	}

	if (options.getBoolean("verbose")) {
		cout << "OUTPUT FILE:\n==============================\n";
		cout << mfile;
	}

	return 1;
}



