//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Jun  5 13:56:03 PDT 2022
// Last Modified: Sun Jun  5 14:06:06 PDT 2022
// Filename:      tools/channelinfo.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/channelinfo.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   List channels used for notes in each track and list timbres
//                used in each channel.
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


void processFile(MidiFile& midifile, Options& options);
void printTrackInfo(MidiFile& midifile, int track);


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
	for (int i=0; i<midifile.getTrackCount(); i++) {
		printTrackInfo(midifile, i);
	}
}


//////////////////////////////
//
// printTrackInfo --
//

void printTrackInfo(MidiFile& midifile, int track) {
	vector<bool> channels(16, false);
	string trackName = "";
	vector<vector<int>> patches(16);
	int chan;
	for (int i=0; i<midifile[track].getEventCount(); i++) {
		if (midifile[track][i].isPatchChange()) {
			chan = midifile[track][i].getChannel();
			patches.at(chan).push_back(midifile[track][i].getP1());
			channels.at(chan) = true;
		} else if (midifile[track][i].isNoteOn()) {
			chan = midifile[track][i].getChannel();
			channels.at(chan) = true;
		} else if (midifile[track][i].isTrackName()) {
			trackName = midifile[track][i].getMetaContent();
		}
	}

	cout << "TRACK " << (track+1) << ":" << endl;
	if (!trackName.empty()) {
		cout << "\tNAME: " << trackName << endl;
	}
	for (int i=0; i<16; i++) {
		if (!channels[i]) {
			continue;
		}
		cout << "\tChannel " << (i+1);
		if (!patches[i].empty()) {
			cout << ": ";
		}
		for (int j=0; j<(int)patches[i].size(); j++) {
			cout << MidiFile::getGMInstrumentName(patches[i][j]);
			if (j < (int)patches[i].size()- 1) {
				cout << ", ";
			}
		}
		cout << endl;
	}
}



