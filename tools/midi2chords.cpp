//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Oct 24 18:54:19 PDT 2022
// Last Modified: Mon Oct 24 22:17:12 PDT 2022
// Filename:      midifile/tools/midi2chords.cpp
// Syntax:        C++
//
// Description:   Identify chord sequences in MIDI files.  Whenever
//                there are three or more note attacks at a given
//                time, a chord will be identified (currently notes must
//                all attck at the same time).  Additional chord
//                identifications can be added to the
//                Sonority::identifySonority() function.
//

#include "Options.h"
#include "MidiFile.h"

using namespace std;
using namespace smf;

#include <iostream>

///////////////////////////////////////////////////////////////////////////

class Sonority {
	public:
		double qstamp = 0.0;     // quarter-note timestamp
		double qdur   = 0.0;     // duration to next chord
		vector<int> pitches;     // MIDI pitches in sonority

		// pcs: pitch classes in chord.  This also counds the number
		// of notes for each pitch class.  The lowest pitch class
		// could be converted to a negative number (but not yet done).
		vector<int> pcs = {0,0,0,0,0,0,0,0,0,0,0,0};

		string root;             // root pitch of sonority
		string quality;          // type of chord

		void addNote(int pitch);
		int  countPcs(void);
		void clear(void);
		void identifySonority(void);
		void setChordLabel(int pc, const string& qual);
		static int checkForMatch(vector<int>& data, vector<int>& prototype);
};

void Sonority::addNote(int pitch) {
	pitches.push_back(pitch);
	int pc = pitch % 12;
	pcs.at(pc)++;
}

int Sonority::countPcs(void) {
	int output = 0;
	for (int i=0; i<(int)pcs.size(); i++) {
		if (pcs[i]) {
			output++;
		}
	}
	return output;
}

void Sonority::clear(void) {
	qstamp = 0.0;
	qdur   = 0.0;
	pitches.clear();
	fill(pcs.begin(), pcs.end(), 0);
}


void Sonority::identifySonority(void) {
	int match;

	// Triadic chord:

	vector<int> major = {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0};
	match = Sonority::checkForMatch(pcs, major);
	if (match >= 0) {
		setChordLabel(match, "major");
		return;
	}

	vector<int> minor = {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0};
	match = Sonority::checkForMatch(pcs, minor);
	if (match >= 0) {
		setChordLabel(match, "minor");
		return;
	}

	vector<int> diminished = {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0};
	match = Sonority::checkForMatch(pcs, diminished);
	if (match >= 0) {
		setChordLabel(match, "diminished");
		return;
	}

	vector<int> augmented = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
	match = Sonority::checkForMatch(pcs, augmented);
	if (match >= 0) {
		// Note that the augmented root is ambiguious for MIDI:
		// the chord is symmetric, and any not could be the root.
		setChordLabel(match, "augmented");
		return;
	}

	// Seventh chords:

	vector<int> majmin = {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
	match = Sonority::checkForMatch(pcs, majmin);
	if (match >= 0) {
		setChordLabel(match, "dominant seventh");
		return;
	}

	vector<int> minmin = {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0};
	match = Sonority::checkForMatch(pcs, minmin);
	if (match >= 0) {
		setChordLabel(match, "minor seventh");
		return;
	}

	vector<int> majmaj = {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1};
	match = Sonority::checkForMatch(pcs, majmaj);
	if (match >= 0) {
		setChordLabel(match, "major seventh");
		return;
	}

	vector<int> minmaj = {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
	match = Sonority::checkForMatch(pcs, minmaj);
	if (match >= 0) {
		setChordLabel(match, "minor-major seventh");
		return;
	}

	vector<int> halfdim = {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0};
	match = Sonority::checkForMatch(pcs, halfdim);
	if (match >= 0) {
		setChordLabel(match, "half diminished seventh");
		return;
	}

	vector<int> fulldim = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0};
	match = Sonority::checkForMatch(pcs, fulldim);
	if (match >= 0) {
		setChordLabel(match, "fully diminished seventh");
		return;
	}

	// Other chord definitions can be added here.

}

void Sonority::setChordLabel(int pc, const string& qual) {
	quality = qual;
	switch (pc) {
		case  0: root = "C";  break;
		case  1: root = "C#"; break;
		case  2: root = "D";  break;
		case  3: root = "E-"; break;
		case  4: root = "E";  break;
		case  5: root = "F";  break;
		case  6: root = "F#"; break;
		case  7: root = "G";  break;
		case  8: root = "A-"; break;
		case  9: root = "A";  break;
		case 10: root = "B-"; break;
		case 11: root = "B";  break;
	}
}

int Sonority::checkForMatch(vector<int>& data, vector<int>& prototype) {
	// returns -1 if no match found; otherwise, returns pitch class
	// of match (0 = C, 1 = C#, ..., 11 = B).
	int output = -1;
	bool match = false;
	if (data.size() != 12) {
		return -1;
	}
	if (prototype.size() != 12) {
		return -1;
	}
	for (int i=0; i<12; i++) {
		match = true;
		output = i;
		for (int j=0; j<12; j++) {
			if ((!!prototype.at(j)) != data.at((i+j) % 12)) {
				match = false;
				break;
			}
		}
		if (match) {
			break;
		}
	}

	return match ? output : -1;
}


///////////////////////////////////////////////////////////////////////////

// function declarations:
void processMidiFile (Options& options, MidiFile& midifile);
void calculateDurations(vector<Sonority>& chordSequence, double maxQstamp);
void printChordSequence(vector<Sonority>& chordSequence);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	int       status;
	MidiFile  inputfile;
	Options   options(argc, argv);
	options.define("s|sustained=b", "consider sustained sonorities");
	options.process(argc, argv);

	int argcount = options.getArgCount();
	if (argcount != 1) {
		cerr << "Usage: " << options.getCommand() << " midifile" << endl;
		exit(1);
	}

	status = inputfile.read(options.getArg(1));
	if (status == 0) {
		cout << "Syntax error in file: " << options.getArg(1) << "\n";
	}

	processMidiFile(options, inputfile);
	return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// processMidiFile --
//

void processMidiFile(Options& options, MidiFile& midifile) {
	// bool sustainedQ = options.getBoolean("sustained");
	midifile.absoluteTicks();  // convert from delta time to absolute time
	midifile.joinTracks();     // merge all notes to single timeline.

	vector<Sonority> chordSequence;
	chordSequence.reserve(10000);

	Sonority currentChord;
	double tpq = midifile.getTicksPerQuarterNote();

	int eventcount = midifile.getEventCount(0);
	MidiEvent *event;
	for (int i=0; i<eventcount; i++) {
		event = &(midifile[0][i]);

		if (!event->isNoteOn()) {
			// ignore events that are not note-ons
			continue;
		}

		if (event->getChannel() == 0x09) {
			// ignore drum tracks
			continue;
		}

		double qstamp = event->tick / tpq;
		int pitch = event->getKeyNumber();

		if (qstamp == currentChord.qstamp) {
			// Add a note to the current chord:
			currentChord.addNote(pitch);
		} else if (qstamp > currentChord.qstamp) {
			// Store the current chord in the array
			// if there are at least 3 pitch classes:
			if (currentChord.countPcs() >= 3) {
				chordSequence.push_back(currentChord);
			}
			currentChord.clear();
			currentChord.qstamp = qstamp;
			currentChord.addNote(pitch);
		} else {
			cerr << "Causality violation at qstamp " << qstamp << endl;
		}
	}

	// Add last chord in file:
	if (currentChord.countPcs() >= 3) {
		chordSequence.push_back(currentChord);
	}


	double maxQstamp = midifile[0].back().tick / tpq;
	calculateDurations(chordSequence, maxQstamp);
	for (int i=0; i<(int)chordSequence.size(); i++) {
		chordSequence[i].identifySonority();
	}
	printChordSequence(chordSequence);
}



//////////////////////////////
//
// calculateDurations --
//

void calculateDurations(vector<Sonority>& chordSequence, double maxQstamp) {
	vector<Sonority>& cs = chordSequence;
	for (int i=0; i<(int)cs.size() - 1; i++) {
		cs[i].qdur = cs[i+1].qstamp - cs[i].qstamp;
	}
	// calculate last chord duration:
	cs.back().qdur = maxQstamp - cs.back().qstamp;
}



/////////////////////////////
//
// printChordSequence --
//

void printChordSequence(vector<Sonority>& chordSequence) {
	vector<Sonority>& cs = chordSequence;
	for (int i=0; i<(int)cs.size(); i++) {
		cout << cs[i].qstamp << "\t" << cs[i].qdur << ":\t";
		for (int j=0; j<cs[i].pcs.size(); j++) {
			cout << (cs[i].pcs[j] ? 1 : 0);
		}
		cout << ":\t" << cs[i].root;
		cout << "\t" << cs[i].quality;
		cout << endl;
	}
}


/* test file: Save content below to chordtest.txt and then run the command:
 *     bin/binasc chordtest -c chordtest.mid
 * then test with the command:
 *     bin/midi2chords chordtest.mid
 * results:
 *    0	1:	100010010000:	C	major
 *    1	1:	100001000100:	F	major
 *    2	1:	100010010000:	C	major
 *    3	1:	001001010001:	G	dominant seventh
 *    4	1:	100010010000:	C	major
 *
 * First number is the time in quarter notes since the
 * start of the MIDI file.  Second number is the duration
 * to the next chord (in quarter notes), then a 12-digit
 * list of pitch classes (first is C, then C#, etc. to B).
 * Lastly is the identified chord (if found in list of defined
 * chords).


"MThd"			; MIDI header chunk marker
4'6			; bytes to follow in header chunk
2'0			; file format: Type-0 (single track)
2'1			; number of tracks
2'100			; ticks per quarter note

;;; TRACK 0 ----------------------------------
"MTrk"			; MIDI track chunk marker
4'132			; bytes to follow in track chunk
v0	90 '60 '60	; note-on C4
v0	90 '64 '60	; note-on E4
v0	90 '67 '60	; note-on G4

v100	90 '60 '0	; note-off C4
v0	90 '64 '0	; note-off E4
v0	90 '67 '0	; note-off G4

v0	90 '60 '60	; note-on C4
v0	90 '65 '60	; note-on F4
v0	90 '69 '60	; note-on A4

v100	90 '60 '0	; note-off C4
v0	90 '65 '0	; note-off F4
v0	90 '69 '0	; note-off A4

v0	90 '60 '60	; note-on C4
v0	90 '64 '60	; note-on E4
v0	90 '67 '60	; note-on G4

v100	90 '60 '0	; note-off C4
v0	90 '64 '0	; note-off E4
v0	90 '67 '0	; note-off G4

v0	90 '59 '60	; note-on B3
v0	90 '62 '60	; note-on D4
v0	90 '65 '60	; note-on F4
v0	90 '67 '60	; note-on G4

v100	90 '59 '0	; note-off B3
v0	90 '62 '0	; note-off D4
v0	90 '65 '0	; note-off F4
v0	90 '67 '0	; note-off G4

v0	90 '60 '60	; note-on C4
v0	90 '64 '60	; note-on E4
v0	90 '67 '60	; note-on G4

v100	90 '60 '0	; note-off C4
v0	90 '64 '0	; note-off E4
v0	90 '67 '0	; note-off G4

v0	ff 2f v0	; end-of-track

*/



