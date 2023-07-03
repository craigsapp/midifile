//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun 29 14:05:26 PDT 2023
// Last Modified: Thu Jun 29 14:05:31 PDT 2023
// Filename:      midifile/tools/mean.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Calculate the mean pitch (excluding any notes in
//                drum track)
//
// To do:         Select specific channel(s) to analyze.
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
	double sum = 0.0;
	int count = 0;
	for (int t=0; t<midifile.getTrackCount(); t++) {
		for (int e=0; e<midifile[t].getEventCount(); e++) {
			MidiEvent &event = midifile[t][e];
			if (!event.isNoteOn()) {
				continue;
			}
			int channel = event.getChannel();
			if (channel == 0x09) {
				// ignore percussion channel
				continue;
			}
			// cout << "MIDI NOTE: " << event.getKeyNumber() << endl;
			sum += event.getKeyNumber();
			count++;
		}
	}
	cout << (sum / count) << endl;
}



