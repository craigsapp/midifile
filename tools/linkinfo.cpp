//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Feb 13 23:07:30 PST 2025
// Last Modified: Thu Feb 13 23:07:33 PST 2025
// Filename:      tools/linknotes.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/linknotes.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Demo program for linking note-ons and note-offs.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
	Options options;
	options.define("o|on|ons-only=b", "Show only links of note-ons");
	options.define("d|dur|duration=b", "Show tick duration between linked events");
	options.process(argc, argv);
	bool onsonlyQ = options.getBoolean("ons-only");
	bool durQ     = options.getBoolean("duration");
	if (options.getArgCount() < 1) {
		cerr << "Usage: " << options.getCommand() << " input.mid" << endl;
		exit(1);
	}
	MidiFile mfile(options.getArg(1));
	mfile.linkNotePairs();

	for (int i=0; i<mfile.getTrackCount(); i++) {
		for (int j=0; j<mfile[i].getEventCount(); j++) {
			cout << mfile[i][j];
			if (mfile[i][j].isLinked()) {
				if (onsonlyQ && mfile[i][j].isNoteOn()) {
					MidiEvent* link = mfile[i][j].getLinkedEvent();
					if (link) {
						cout << "\t" << *link;
						if (durQ) {
							cout << "\tDUR: " << (link->tick - mfile[i][j].tick);
						}
					}
				} else if (!onsonlyQ) {
					MidiEvent* link = mfile[i][j].getLinkedEvent();
					if (link) {
						cout << "\t" << *link;
						if (durQ) {
							cout << "\tDUR: " << (link->tick - mfile[i][j].tick);
						}
					}
				}
			}
			cout << endl;
		}
	}

	return 1;
}



