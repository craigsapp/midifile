//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue May 24 19:54:32 PDT 2022
// Last Modified: Tue May 24 21:51:29 PDT 2022
// Filename:      tools/mts-type2.cpp 
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/mts-type2.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Test of MTS (MIDI Tuning Specification), second type 
//                (real-time remaping of any number of pitches).   This example
//                sets MIDI keys from 60 to 84 to be quarter tones and then
//                plays a one-octave quarter-note scale, repeating the
//                first note which should be one octave below the penultimate
//                note.
//

#include "MidiFile.h"
#include "Options.h"

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace smf;


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("f|frequency=b", "calculate tunings from frequencies rather than semitones");
	options.define("o|output=s:quarter.mid", "output file name (Binasc to stdout otherwise");
	options.define("r|a4-reference-frequency=d:440.0 Hz", "Tuning of A4");
	options.process(argc, argv);

	// a4ref can be used to set the frequency of A4 when using the -f option.
	double a4ref   =  options.getDouble("a4-reference-frequency");
	string file    =  options.getString("output");
	bool stdoutQ   = !options.getBoolean("output");
	bool freqQ     =  options.getBoolean("frequency");
	int channel    =  0;
	int actiontime =  0;
	int velocity   =  64;
	int tpq        =  100;
	int key;

	MidiFile outputfile;
	outputfile.absoluteTicks();
	outputfile.addTrack(1);
	outputfile.setTicksPerQuarterNote(tpq);

	MidiEvent midievent;
	midievent.makeTempo(120.0);
	outputfile.addEvent(0, actiontime, midievent);

	midievent.makePatchChange(channel, 6);
	outputfile.addEvent(1, actiontime, midievent);

	if (freqQ) {
		// Calculate pitch mapping by frequencies:
		vector<pair<int, double>> freqtune;
		for (int i=0; i<25; i++) {
			double freq = pow(2.0, (i/2.0 - 9.0)/12.0) * a4ref;
			freqtune.push_back(make_pair(60 + i, freq));
		}
		midievent.makeMts2_KeyTuningsByFrequency(freqtune);
	} else {
		// Calculate pitch mapping by semitones:
		vector<pair<int, double>> semitune;
		for (int i=0; i<25; i++) {
			semitune.push_back(make_pair(60 + i, 60.0 + i / 2.0));
		}
		midievent.makeMts2_KeyTuningsBySemitone(semitune);
	}

	outputfile.addEvent(1, actiontime, midievent);

	for (int i=0; i<25; i++) {
		key = 60 + i;
		midievent.makeNoteOn(channel, key, velocity);
		outputfile.addEvent(1, actiontime, midievent);
		actiontime += tpq;
		if (key == 60 + 24) {
			actiontime += tpq;
		}
		midievent.makeNoteOff(channel, key);
		outputfile.addEvent(1, actiontime, midievent);
	}

	key = 60;
	midievent.makeNoteOn(channel, key, velocity);
	outputfile.addEvent(1, actiontime, midievent);
	actiontime += 2 * tpq;
	midievent.makeNoteOff(channel, key);
	outputfile.addEvent(1, actiontime, midievent);

	// make sure data is in correct order before converting to delta times:
	outputfile.sortTracks(); 
	if (stdoutQ) {
		// output in Binasc syntax
		cout << outputfile;
	} else {
		outputfile.write(file);
	}
	return 0;
}



