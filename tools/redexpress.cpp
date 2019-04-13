//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Feb 24 20:20:18 PST 2018
// Last Modified: Sat Feb 24 20:20:21 PST 2018
// Filename:      midifile/tools/redexpress.cpp
// Web Address:   
// Syntax:        C++; midifile
// vim:           ts=3
//
// Description:   Adds expression information to notes extracted from
//                Red Welte-Mignon piano rolls.
//
///////////////////////////////////////////////////////////
//
// red Welte-Mignon tracker holes (T100):
//
//   10 expression on left side:
//       1:  Mezzo-Forte-Off                 MIDI Key 14
//       2:  Mezzo-Forte-On                  MIDI Key 15
//       3:  Crescendo-Off                   MIDI Key 16
//       4:  Crescendo-On                    MIDI Key 17
//       5:  Forzando-Off                    MIDI Key 18
//       6:  Forzando-On                     MIDI Key 19
//       7:  Soft-Pedal-Off                  MIDI Key 20
//       8:  Soft-Pedal-On                   MIDI Key 21
//       9:  Motor-Off                       MIDI Key 22
//       10: Motor-On                        MIDI Key 23
//   Then 80 notes from C1 to G7 (MIDI note 24 to 103):
//       11: C1                              MIDI Key 24
//       ...
//       50:  D#4                            MIDI Key 63
//    Treble register (40 notes) from E4 to G7:
//       51:  E4                             MIDI Key 64
//       ...
//       90:  G7                             MIDI Key 103
//   Then 10 expression holes on the right side:
//       91:  -10: Rewind                    MIDI Key 104
//       92:  -9:  Electric-Cutoff           MIDI Key 105
//       93:  -8:  Sustain-Pedal-On          MIDI Key 106
//       94:  -7:  Sustain-Pedal-Off         MIDI Key 107
//       95:  -6:  Forzando-On               MIDI Key 108
//       96:  -5:  Forzando-Off              MIDI Key 109
//       97:  -4:  Crescendo-On              MIDI Key 110
//       98:  -3:  Crescendo-Off             MIDI Key 111
//       99:  -2:  Mezzo-Forte-On            MIDI Key 112
//       100: -1:  Mezzo-Forte-Off           MIDI Key 113
//
///////////////////////////////////////////////////////////

#include "MidiFile.h"
#include "Options.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace smf;

// function declarations:
void    checkOptions   (Options& opts, int argc, char** argv);
void    example        (void);
void    usage          (const char* command);

void    addExpression_T100  (MidiFile& midifile);
int     getRewindTime       (MidiFile& midifile, int track);
void    addSustainPedalling (MidiFile& midifile, int sourcetrack,
                             int targettrack);
void    addSoftPedalling    (MidiFile& midifile, int sourcetrack,
                             int targettrack);

#define TYPE_NONE 0
#define TYPE_T100 1

int Rolltype    = TYPE_T100;
int RewindKey   = 104;
int PedalOnKey  = 106;
int PedalOffKey = 107;
int SoftOnKey   = 21;
int SoftOffKey  = 20;

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	Options options;
	checkOptions(options, argc, argv);
	MidiFile midifile;
	if (options.getArgCount() > 0) {
		midifile.read(options.getArg(1));
	} else {
		midifile.read(cin);
	}
	addExpression_T100(midifile);
	if (options.getArgCount() >= 2) {
		midifile.write(options.getArg(2));
	} else {
		cout << midifile;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// addExpression_T100 --
//

void addExpression_T100(MidiFile& midifile) {
	int tracks = midifile.getTrackCount();
	if (tracks != 5) {
		cerr << "This program expects a five-track MIDI file to process" << endl;
		exit(1);
	}

	// add a new track with some of the expression components (pedal)
	midifile.addTrack();

	int expressiontrack = 5;
	int rewind = getRewindTime(midifile, 4);
	MidiEvent me;
	me.tick = rewind;
	me.track = expressiontrack;

	me.makeController(1, 7, 0);  // mute for any post-rewind holes: bass
	midifile.addEvent(me);
	me.makeController(2, 7, 0);  // mute for any post-rewind holes: treble
	midifile.addEvent(me);

	addSustainPedalling(midifile, 4, expressiontrack);
	addSoftPedalling(midifile, 4, expressiontrack);

	midifile.sortTracks();
}



//////////////////////////////
//
// addSustainPedalling --
//

void addSustainPedalling(MidiFile& midifile, int sourcetrack, int targettrack) {
	int count = midifile.getEventCount(sourcetrack);
	MidiEvent me;
	me.track = targettrack;
	for (int i=0; i<count; i++) {
		if (!midifile[sourcetrack][i].isNoteOn()) {
			continue;
		}
		int key = midifile[sourcetrack][i].getKeyNumber();
		me.tick = midifile[sourcetrack][i].tick;
		if (key == PedalOnKey) {
			me.makeController(1, 64, 127);
			midifile.addEvent(me);
			me.makeController(2, 64, 127);
			midifile.addEvent(me);
		} else if (key == PedalOffKey) {
			me.makeController(1, 64, 0);
			midifile.addEvent(me);
			me.makeController(2, 64, 0);
			midifile.addEvent(me);
		}
	}
}



//////////////////////////////
//
// addSoftPedalling --
//

void addSoftPedalling(MidiFile& midifile, int sourcetrack, int targettrack) {
	int count = midifile.getEventCount(sourcetrack);
	MidiEvent me;
	me.track = targettrack;
	for (int i=0; i<count; i++) {
		if (!midifile[sourcetrack][i].isNoteOn()) {
			continue;
		}
		int key = midifile[sourcetrack][i].getKeyNumber();
		me.tick = midifile[sourcetrack][i].tick;
		if (key == SoftOnKey) {
			me.makeController(1, 67, 127);
			midifile.addEvent(me);
			me.makeController(2, 67, 127);
			midifile.addEvent(me);
		} else if (key == SoftOffKey) {
			me.makeController(1, 67, 0);
			midifile.addEvent(me);
			me.makeController(2, 67, 0);
			midifile.addEvent(me);
		}
	}
}



//////////////////////////////
//
// getRewindTime -- Return the start time of the rewind hole.
//    Any notes after this time should be muted (set master volume to 0).
//

int getRewindTime(MidiFile& midifile, int track) {
	int count = midifile.getEventCount(track);
	for (int i=count-1; i>=0; i--) {
		if (!midifile[track][i].isNoteOn()) {
			continue;
		}
		if (midifile[track][i].getKeyNumber() == RewindKey) {
			return midifile[track][i].tick;
		}
	}

	return -1;
}



//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts, int argc, char* argv[]) {
	opts.define("t|type=s:redwelte",  "type of roll to process");
	opts.define("t100|T100=b", "red Welte-Mignon roll, 100 tracker holes");

	opts.define("author=b",  "author of program");
	opts.define("version=b", "compilation info");
	opts.define("example=b", "example usages");
	opts.define("h|help=b",  "short description");
	opts.process(argc, argv);

	// handle basic options:
	if (opts.getBoolean("author")) {
		cout << "Written by Craig Stuart Sapp, "
			  << "craig@ccrma.stanford.edu, 24 February 2018" << endl;
		exit(0);
	} else if (opts.getBoolean("version")) {
		cout << argv[0] << ", version: February 2018" << endl;
		cout << "compiled: " << __DATE__ << endl;
		exit(0);
	} else if (opts.getBoolean("help")) {
		usage(opts.getCommand().data());
		exit(0);
	} else if (opts.getBoolean("example")) {
		example();
		exit(0);
	}

	if (opts.getArgCount() > 2) {
		usage(opts.getCommand().data());
		exit(1);
	}

	Rolltype = TYPE_T100;
	if (opts.getBoolean("type")) {
		string typestring = opts.getString("type");
		if (typestring == "redwelte") {
			Rolltype = TYPE_T100;
		} else if (typestring == "t100") {
			Rolltype = TYPE_T100;
		} else if (typestring == "T100") {
			Rolltype = TYPE_T100;
		}
	} else if (opts.getBoolean("T100")) {
		Rolltype = TYPE_T100;
	}

	if (Rolltype != TYPE_T100) {
		cerr << "Unknown roll type" << endl;
		exit(1);
	}
}



//////////////////////////////
//
// example --
//

void example(void) {

}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {

}




