//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Feb  1 22:05:57 PST 2024
// Last Modified: Fri Feb  2 20:22:50 PST 2024
// Filename:      tools/temper.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/temper.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Split a single-timbre MIDI file into 12 channels,
//                one for each pitch class.  Then apply pitch bends
//                to tune each channel to a specific temperament.
//                This method should work on all synthesizers.
//
//                But could also use another MIDI standard for assigning pitch to keys:
//                See: https://github.com/craigsapp/midifile/blob/master/tools/mts-type2.cpp
//                See: https://github.com/craigsapp/midifile/blob/master/tools/mts-type9.cpp
//                But this method does not work on all synthesizers.
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


void processFile                  (MidiFile& midifile, Options& options);
void applyInstrument              (MidiFile& midifile, int timbre);
void removeInstruments            (MidiFile& midifile);
void applyTemperament             (MidiFile& midifile, const string& temperament, int base = 0);
void applyEqualTemperament        (MidiFile& midifile, int base = 0);
void applyMeantoneTemperament     (MidiFile& midifile, int base = 0);
void applyPythagoreanTemperament  (MidiFile& midifile, int base = 0);
void applyKirnberger3Temperament  (MidiFile& midifile, int base = 0);
void applyValottiTemperament      (MidiFile& midifile, int base = 0);
void applyBadTemperament          (MidiFile& midifile, int base = 0);
void applyWeirdTemperament        (MidiFile& midifile, int base = 0);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("b|base-pitch-class=i:0", "Base pitch class for temperament");
	options.define("B|bad|bad-temperaent=b", "Use bad temperament");
	options.define("e|et|equal|equal-temperament=b", "Use equal temperament");
	options.define("i|instrument=i:6", "General MIDI instrument to use");
	options.define("k|kirnberger|kirnberger3=b","Use Kirnberger III  temperament");
	options.define("m|meantone=b",     "Use meantone temperament");
	options.define("p|pythagorean=b",  "Use pythagorean temperament");
	options.define("t|temperament=s:weird", "Apply given temperament");
	options.define("v|valotti=b",  "Use Valotti temperament");
	options.define("w|weird|weird-temperament=b", "Use weird temperament");
	options.define("0|type-0|O|o=b", "Convert to Type-0 midifile");
	options.process(argc, argv);
	MidiFile midifile;

	if (options.getArgCount() == 0) {
		midifile.read(cin);
		processFile(midifile, options);
		cout << midifile;
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
	midifile.joinTracks();
	for (int e=0; e<midifile[0].getEventCount(); e++) {
		MidiEvent *curNote = &midifile[0][e];
		if (curNote->isPatchChange()) {
			curNote->clear();
		}
		if (!curNote->isNote()) {
			continue;
		}
		int chan = curNote->getChannel();
		if (chan == 0x09) {
			// ignore drum track
			continue;
		}
		int pitch = curNote->getKeyNumber();
		int pc = pitch % 12;
		int newchan = (pc < 0x09) ? pc : pc + 1;
		curNote->setChannel(newchan);
	}

	if (!options.getBoolean("type-0")) {
		midifile.splitTracks();
	}

	removeInstruments(midifile);
	int instrumentCode = options.getInteger("instrument");
	applyInstrument(midifile, instrumentCode);

	int base = options.getInteger("base-pitch-class");
	if (base < 0) {
		base = 0;
	} else if (base > 11) {
		base = base % 12;
	}
	string temperament = options.getString("temperament");
	if (options.getBoolean("pythagorean")) {
		temperament = "pythagorean";
	}
	if (options.getBoolean("meantone")) {
		temperament = "meantone";
	}
	if (options.getBoolean("kirnberger3")) {
		temperament = "kirnberger3";
	}
	if (options.getBoolean("valotti")) {
		temperament = "valotti";
	}
	if (options.getBoolean("bad")) {
		temperament = "bad";
	}
	if (options.getBoolean("weird")) {
		temperament = "weird";
	}
	if (options.getBoolean("equal")) {
		temperament = "equal";
	}
	applyTemperament(midifile, temperament, base);

	midifile.sortTracks();
}



//////////////////////////////
//
// removeInstruments -- Remove any existing patch changes.
//    Input is assumbed to be in merged format (single track);
//

void removeInstruments(MidiFile& midifile) {
	int count = midifile[0].getEventCount();
	for (int i=0; i<count; i++) {
		MidiEvent* event = &midifile[0][i];
		if (event->isPatchChange()) {
			event->clear();
		}
	}
}



//////////////////////////////
//
// applyInstrument -- Set the timbre for all pitch classes
//

void applyInstrument(MidiFile& midifile, int timbre) {
	int track = 0;
	if (midifile.getTrackCount() > 1) {
		track = 1;
	}
	MidiEvent pcMessage;
	pcMessage.tick = 0;  // add messages at start of track (after sorting)
	for (int i=0; i<12; i++) {
		int chan = (i < 0x09) ? i : i+1;
		pcMessage.makePatchChange(chan, timbre);
		midifile[track].append(pcMessage);
	}
}



//////////////////////////////
//
// applyTemperament --
//

void applyTemperament(MidiFile& midifile, const string& temperament, int base) {
	if (temperament == "pythagorean") {	
		applyPythagoreanTemperament(midifile);
	} else if (temperament == "meantone") {
		applyMeantoneTemperament(midifile);
	} else if (temperament == "kirnberger3") {
		applyKirnberger3Temperament(midifile);
	} else if (temperament == "valotti") {
		applyValottiTemperament(midifile);
	} else if (temperament == "bad") {
		applyBadTemperament(midifile);
	} else if (temperament == "weird") {
		applyWeirdTemperament(midifile);
	} else if (temperament == "equal") {
		applyEqualTemperament(midifile);
	}
}


//////////////////////////////
//
// applyPythagoreanTemperament -- Centered on C if base=0.
//     See https://en.wikipedia.org/wiki/Pythagorean_tuning
//

void applyPythagoreanTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 0, 64);
	midifile[track].append(event); // C:    0    cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 49, 68);
	midifile[track].append(event); // C#: +13.7  cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 32, 65);
	midifile[track].append(event); // D:   +3.9  cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 16, 62);
	midifile[track].append(event); // Ef:  -5.9  cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 64, 66);
	midifile[track].append(event); // E:   +7.8  cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 48, 63);
	midifile[track].append(event); // F:   -1.96 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 96, 67);
	midifile[track].append(event); // F#: +11.7  cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 80, 64);
	midifile[track].append(event); // G:   +1.96 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 64, 61);
	midifile[track].append(event); // Af:  -1.78 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 112, 65);
	midifile[track].append(event); // A:   +5.87 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 96, 62);
	midifile[track].append(event); // Bf:  -3.9  cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 16, 67);
	midifile[track].append(event); // B:   +9.8  cents from equal temperament
}



//////////////////////////////
//
// applyMeantoneTemperament -- Quarter-comman meantone centered on C if base=0.
//     See https://en.wikipedia.org/wiki/Quarter-comma_meantone
//

void applyMeantoneTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 0, 64);
	midifile[track].append(event); // C:    0   cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 41, 56);
	midifile[track].append(event); // C#: -24.0 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 105, 61);
	midifile[track].append(event); // D:   -6.8 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 38, 67);
	midifile[track].append(event); // Ef: +10.3 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 79, 59);
	midifile[track].append(event); // E:  -13.7 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 11, 65);
	midifile[track].append(event); // F:   +3.4 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 56, 57);
	midifile[track].append(event); // F#: -20.5 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 117, 62);
	midifile[track].append(event); // G:   -3.4 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 30, 55);
	midifile[track].append(event); // Af: -27.4 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 90, 60);
	midifile[track].append(event); // A:  -10.3 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 23, 66);
	midifile[track].append(event); // Bf:  +6.8 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 68, 58);
	midifile[track].append(event); // B:  -17.1 cents from equal temperament
}



//////////////////////////////
//
// applyKirnberger3Temperament -- Kirnberger III temperament based on C if base=0.
//     See https://en.wikipedia.org/wiki/Kirnberger_temperament
//

void applyKirnberger3Temperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 0, 64);
	midifile[track].append(event); // C:    0   cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 112, 60);
	midifile[track].append(event); // C#:  -9.8 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 104, 61);
	midifile[track].append(event); // D:   -6.8 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 16, 62);
	midifile[track].append(event); // Ef:  -5.9 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 79, 59);
	midifile[track].append(event); // E:  -13.7 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 48, 63);
	midifile[track].append(event); // F:   -2.0 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 112, 60);
	midifile[track].append(event); // F#:  -9.8 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 116, 62);
	midifile[track].append(event); // G:   -3.4 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 64, 61);
	midifile[track].append(event); // Af:  -7.8 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 92, 60);
	midifile[track].append(event); // A:  -10.3 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 96, 62);
	midifile[track].append(event); // Bf:  +3.9 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 32, 60);
	midifile[track].append(event); // B:  -11.7 cents from equal temperament
}



//////////////////////////////
//
// applyValottiTemperament -- Valotti temperament based on C#/A if base=0 (for some reason).
//     See https://en.wikipedia.org/wiki/Vallotti_temperament
//

void applyValottiTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 118, 65);
	midifile[track].append(event); // C: 0   cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 0, 64);
	midifile[track].append(event); // C#: 0 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 82, 64);
	midifile[track].append(event); // D: 2 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 36, 65);
	midifile[track].append(event); // Ef: 4 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 46, 63);
	midifile[track].append(event); // E: -2 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 72, 66);
	midifile[track].append(event); // F: 8 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 46, 63);
	midifile[track].append(event); // F#: -2 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 36, 65);
	midifile[track].append(event); // G: 4 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 82, 64);
	midifile[track].append(event); // Af: 2 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 0, 64);
	midifile[track].append(event); // A: 0 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 118, 65);
	midifile[track].append(event); // Bf: +6 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBend(chan, 92, 62);
	midifile[track].append(event); // B: -4 cents from equal temperament
}



//////////////////////////////
//
// applyBadTemperament -- Bad (way out of tune) temperament based on C if base=0.
//     Lots of deviation from equal-temperament.
//

void applyBadTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.2);
	midifile[track].append(event); // C:   40 cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.15);
	midifile[track].append(event); // C#:  30 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.1);
	midifile[track].append(event); // D:   20 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.1);
	midifile[track].append(event); // Ef: -20 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.15);
	midifile[track].append(event); // E: -30 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.2);
	midifile[track].append(event); // F: 40 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.05);
	midifile[track].append(event); // F#: -10 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.1);
	midifile[track].append(event); // G: -20 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.4);
	midifile[track].append(event); // Af: -20 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.3);
	midifile[track].append(event); // A: -15 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.25);
	midifile[track].append(event); // Bf: 50 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.025);
	midifile[track].append(event); // B: 5 cents from equal temperament
}



//////////////////////////////
//
// applyWeirdTemperament -- Weird temperament based on C if base=0.
//     Swap the pitch of each pair of semitones.
//

void applyWeirdTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // C: 100 cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // C#: -100 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // D: 100 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // Ef: -100 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // E: 100 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // F: -100 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // F#: 100 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // G: -100 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // Af: 100 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // A: -100 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.5);
	midifile[track].append(event); // Bf: 100 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, -0.5);
	midifile[track].append(event); // B: -100 cents from equal temperament
}



//////////////////////////////
//
// applyEqualTemperament -- Equal temperament based on C if base=0.
//     See https://en.wikipedia.org/wiki/12_equal_temperament
//

void applyEqualTemperament(MidiFile& midifile, int base) {
	int tracks = midifile.getTrackCount();
	int track = (tracks == 1) ? 0 : 1;
	MidiEvent event;
	event.tick = 0;
	int chan;

	chan = 0 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // C: 0 cents from equal temperament

	chan = 1 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // C#: 0 cents from equal temperament

	chan = 2 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // D: 0 cents from equal temperament

	chan = 3 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // Ef: 0 cents from equal temperament

	chan = 4 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // E: 0 cents from equal temperament

	chan = 5 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // F: 0 cents from equal temperament

	chan = 6 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // F#: 0 cents from equal temperament

	chan = 7 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // G: 0 cents from equal temperament

	chan = 8 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // Af: 0 cents from equal temperament

	chan = 9 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // A: 0 cents from equal temperament

	chan = 10 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // Bf: 0 cents from equal temperament

	chan = 11 + base;
	chan = (chan < 0x09) ? chan : chan+1;
	event.makePitchBendDouble(chan, 0.0);
	midifile[track].append(event); // B: 0 cents from equal temperament
}



