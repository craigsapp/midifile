//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb 12 23:09:37 PST 2025
// Last Modified: Thu Feb 13 11:40:30 PST 2025
// Filename:      tools/sortnotes.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/sortnotes.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Sort notes that occur at the same tick time in the same track.
//                Note-offs will be placed before Note-ons when they occur at
//                the same time, and note on/off groups will each be sorted further
//                by key number (low to high).
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

void processTrack(MidiEventList& list);
void printTickNoteList(MidiFile& mfile, Options& options);
void printEventDeltaTicks(MidiFile& mfile);

int main(int argc, char** argv) {
	Options options;
	options.define("v|verbose=b",  "display input/output in binasc format");
	options.define("a|absolute=b", "display input/output data with absolute ticks note list");
	options.define("d|delta=b",    "display input/output data with delta ticks note list");
	options.process(argc, argv);
	if (options.getArgCount() < 1) {
		cerr << "Usage: " << options.getCommand() << "input.mid [output.mid]" << endl;
		exit(1);
	}
	MidiFile mfile(options.getArg(1));

	if (options.getBoolean("absolute")) {
		cout << "INPUT NOTE LIST:\n\n";
		printTickNoteList(mfile, options);
	}
	if (options.getBoolean("verbose")) {
		cout << "INPUT FILE:\n\n";
		cout << mfile;
	}

	// When a MIDI file is loaded, all events are assigned a sequence number
	// which is stored in MidiEvent::seq.  This is used to preserve the original
	// order when using MidiFile::joinTracks() in order to reverse the process
	// with MidiFile::splitTracks().  Here the sequence numbers are cleared (set
	// to 0) so that the sorting process does not use the sequence numbers, which
	// would otherwise preserve the sequence number and not more around the note
	// ons/offs specifically.
	mfile.clearSequence();
	mfile.sortTracks();
	if (options.getArgCount() >= 2) {
		mfile.write(options.getArg(2));
	} else {
		// Rewrite to same file if only one midifile name given:
		mfile.write(options.getArg(1));
	}

	if (options.getBoolean("absolute")) {
		cout << "OUTPUT NOTES:\n==============================\n";
		printTickNoteList(mfile, options);
	} 
	mfile.makeDeltaTicks();
	printEventDeltaTicks(mfile);

	if (options.getBoolean("verbose")) {
		cout << "OUTPUT FILE:\n==============================\n";
		cout << mfile;
	}

	return 1;
}



///////////////////////////////
//
// printEventDeltaTicks --
//

void printEventDeltaTicks(MidiFile& mfile) {
	cout << "========================================" << endl;
	for (int i=0; i<mfile.getTrackCount(); i++) {
		for (int j=0; j<mfile[i].getEventCount(); j++) {
			cout << "DTICK: " << mfile[i][j].tick;
			cout << "\tDATA: " << mfile[i][j] << endl;
		}
	}
	cout << "========================================" << endl;
}



//////////////////////////////
//
// printTickNoteList --
//

void printTickNoteList(MidiFile& mfile, Options& options) {
	bool deltaQ = options.getBoolean("delta");
	for (int i=0; i<mfile.getTrackCount(); i++) {
		cout << "Track " << i << endl;
		cout << "-------------------------------------" << endl;
		int lasttick = 0;
		for (int j=0; j<mfile[i].getEventCount(); j++) {
			if (!mfile[i][j].isNote()) {
				continue;
			}
			cout << "TICK: ";
			if (mfile[i][j].tick < 10) {
				cout << " ";
			}
			cout << mfile[i][j].tick;

			if (deltaQ) {
				cout << "\tDELTA: ";
				cout << (mfile[i][j].tick - lasttick);
			}
			lasttick = mfile[i][j].tick;

			cout << "\tKEY: " << mfile[i][j].getKeyNumber();
			if (mfile[i][j].isNoteOn()) {
				cout << "\tON";
			} else {
				cout << "\tOFF";
			}
			cout << endl;
		}
	}
}



/* 

Test input file:

"MThd"  ; MIDI header chunk marker
4'6     ; bytes to follow in header chunk
2'0     ; file format: Type-0 (single track)
2'1     ; number of tracks
2'96    ; ticks per quarter note

;;; TRACK 0 ----------------------------------
"MTrk"  ; MIDI track chunk marker
4'126   ; bytes to follow in track chunk
v0  ff 58 v4 '4 '2 '36 '8   ; time signature
v0  90 '67 '80   ; note-on G4
v0  90 '71 '80   ; note-on B4
v24 80 '67 '64   ; note-off G4
v0  90 '68 '34   ; note-on G#4
v0  80 '71 '64   ; note-off B4
v0  90 '72 '34   ; note-on C5
v24 90 '62 '34   ; note-on D4
v0  90 '66 '34   ; note-on F#4
v0  80 '68 '64   ; note-off G#4
v0  80 '72 '64   ; note-off C5
v24 80 '62 '64   ; note-off D4
v0  80 '66 '64   ; note-off F#4
v0  90 '66 '34   ; note-on F#4
v0  90 '70 '34   ; note-on A#4
v24 90 '65 '34   ; note-on F4
v0  80 '66 '64   ; note-off F#4
v0  90 '69 '34   ; note-on A4
v0  80 '70 '64   ; note-off A#4
v24 90 '63 '80   ; note-on D#4
v0  80 '65 '64   ; note-off F4
v0  90 '67 '80   ; note-on G4
v0  80 '69 '64   ; note-off A4
v24 90 '61 '34   ; note-on C#4
v0  80 '63 '64   ; note-off D#4
v0  90 '65 '34   ; note-on F4
v0  80 '67 '64   ; note-off G4
v24 80 '61 '64   ; note-off C#4
v0  80 '65 '64   ; note-off F4
v0  ff 2f v0     ; end-of-track

Test output file, with note-offs placed before note-ons when they occur at the same time:

"MThd"   ; MIDI header chunk marker
4'6      ; bytes to follow in header chunk
2'0      ; file format: Type-0 (single track)
2'1      ; number of tracks
2'96     ; ticks per quarter note

;;; TRACK 0 ----------------------------------
"MTrk"   ; MIDI track chunk marker
4'126    ; bytes to follow in track chunk
v0  ff 58 v4 '4 '2 '36 '8   ; time signature
v0  90 '67 '80   ; note-on G4
v0  90 '71 '80   ; note-on B4
v24 80 '67 '64   ; note-off G4
v0  80 '71 '64   ; note-off B4
v0  90 '68 '34   ; note-on G#4
v0  90 '72 '34   ; note-on C5
v24 80 '68 '64   ; note-off G#4
v0  80 '72 '64   ; note-off C5
v0  90 '62 '34   ; note-on D4
v0  90 '66 '34   ; note-on F#4
v24 80 '62 '64   ; note-off D4
v0  80 '66 '64   ; note-off F#4
v0  90 '66 '34   ; note-on F#4
v0  90 '70 '34   ; note-on A#4
v24 80 '66 '64   ; note-off F#4
v0  80 '70 '64   ; note-off A#4
v0  90 '65 '34   ; note-on F4
v0  90 '69 '34   ; note-on A4
v24 80 '65 '64   ; note-off F4
v0  80 '69 '64   ; note-off A4
v0  90 '63 '80   ; note-on D#4
v0  90 '67 '80   ; note-on G4
v24 80 '63 '64   ; note-off D#4
v0  80 '67 '64   ; note-off G4
v0  90 '61 '34   ; note-on C#4
v0  90 '65 '34   ; note-on F4
v0  80 '61 '64   ; note-off C#4
v0  80 '65 '64   ; note-off F4
v0  ff 2f v0     ; end-of-track

*/
