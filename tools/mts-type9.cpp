//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed May 25 00:44:56 PDT 2022
// Last Modified: Wed May 25 00:56:47 PDT 2022
// Filename:      tools/mts-type9.cpp 
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/mts-type9.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Test of MTS (MIDI Tuning Specification), nineth type 
//                (real-time two-byte octave temperaments).
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>
#include <utility>
#include <vector>

using namespace std;
using namespace smf;


int addChords(MidiFile& midifile, int actiontime, int tpq);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("o|output=s:quarter.mid", "output file name (Binasc to stdout otherwise");
	options.process(argc, argv);

	// a4ref can be used to set the frequency of A4 when using the -f option.
	string file    =  options.getString("output");
	bool stdoutQ   = !options.getBoolean("output");
	int actiontime =  0;
	int tpq        =  100;

	MidiFile outputfile;
	outputfile.absoluteTicks();
	outputfile.addTrack(1);
	outputfile.setTicksPerQuarterNote(tpq);

	MidiEvent midievent;
	midievent.makeTempo(120.0);
	outputfile.addEvent(0, actiontime, midievent);

	int channel = 0;
	midievent.makePatchChange(channel, 6);
	outputfile.addEvent(1, actiontime, midievent);

	midievent.makeText("Pythagorean temperament");
	outputfile.addEvent(1, actiontime, midievent);
	midievent.makeTemperamentPythagorean();
	outputfile.addEvent(1, actiontime, midievent);
	actiontime = addChords(outputfile, actiontime, tpq);

	actiontime += 2 * tpq;
	midievent.makeText("Bad temperament: random +/- 25 cents changes");
	outputfile.addEvent(1, actiontime, midievent);
	midievent.makeTemperamentBad(25);
	outputfile.addEvent(1, actiontime, midievent);
	actiontime = addChords(outputfile, actiontime, tpq);

	actiontime += 2 * tpq;
	midievent.makeText("Bad temperament: random +/- 100 cents changes");
	outputfile.addEvent(1, actiontime, midievent);
	midievent.makeTemperamentBad(100);
	outputfile.addEvent(1, actiontime, midievent);
	actiontime = addChords(outputfile, actiontime, tpq);

	actiontime += 2 * tpq;
	midievent.makeText("Quarter-tone meantone temperament");
	outputfile.addEvent(1, actiontime, midievent);
	midievent.makeTemperamentMeantone();
	outputfile.addEvent(1, actiontime, midievent);
	actiontime = addChords(outputfile, actiontime, tpq);

	actiontime += 2 * tpq;
	midievent.makeText("Equal temperament");
	outputfile.addEvent(1, actiontime, midievent);
	midievent.makeTemperamentEqual();
	outputfile.addEvent(1, actiontime, midievent);
	actiontime = addChords(outputfile, actiontime, tpq);

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



//////////////////////////////
//
// addChords -- C, D, E, F, F# major chords.
//

int addChords(MidiFile& midifile, int actiontime, int tpq) {
	int channel = 0;
	int track = 1;
	int vel = 64;

	midifile.addNoteOn(track, actiontime, channel, 60, vel);
	midifile.addNoteOn(track, actiontime, channel, 64, vel);
	midifile.addNoteOn(track, actiontime, channel, 67, vel);
	actiontime += tpq;
	midifile.addNoteOff(track, actiontime, channel, 60);
	midifile.addNoteOff(track, actiontime, channel, 64);
	midifile.addNoteOff(track, actiontime, channel, 67);

	midifile.addNoteOn(track, actiontime, channel, 62, vel);
	midifile.addNoteOn(track, actiontime, channel, 66, vel);
	midifile.addNoteOn(track, actiontime, channel, 69, vel);
	actiontime += tpq;
	midifile.addNoteOff(track, actiontime, channel, 62);
	midifile.addNoteOff(track, actiontime, channel, 66);
	midifile.addNoteOff(track, actiontime, channel, 69);

	midifile.addNoteOn(track, actiontime, channel, 64, vel);
	midifile.addNoteOn(track, actiontime, channel, 68, vel);
	midifile.addNoteOn(track, actiontime, channel, 71, vel);
	actiontime += tpq;
	midifile.addNoteOff(track, actiontime, channel, 64);
	midifile.addNoteOff(track, actiontime, channel, 68);
	midifile.addNoteOff(track, actiontime, channel, 71);

	midifile.addNoteOn(track, actiontime, channel, 65, vel);
	midifile.addNoteOn(track, actiontime, channel, 69, vel);
	midifile.addNoteOn(track, actiontime, channel, 72, vel);
	actiontime += tpq;
	midifile.addNoteOff(track, actiontime, channel, 65);
	midifile.addNoteOff(track, actiontime, channel, 69);
	midifile.addNoteOff(track, actiontime, channel, 72);

	midifile.addNoteOn(track, actiontime, channel, 66, vel);
	midifile.addNoteOn(track, actiontime, channel, 70, vel);
	midifile.addNoteOn(track, actiontime, channel, 73, vel);
	actiontime += tpq * 2;
	midifile.addNoteOff(track, actiontime, channel, 66);
	midifile.addNoteOff(track, actiontime, channel, 70);
	midifile.addNoteOff(track, actiontime, channel, 73);

	actiontime += tpq * 2;
	return actiontime;
}



