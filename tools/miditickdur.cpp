//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Sep 13 22:01:30 PDT 2024
// Last Modified: Fri Sep 13 22:01:33 PDT 2024
// Filename:      tool/miditickdur.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/miditickdur.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   List notes start times in ticks/seconds in MIDI file.
//

#include "MidiFile.h"
#include "Options.h"

using namespace std;
using namespace smf;

double roundToThreeDigits(double input);
string getPitchName(MidiMessage& message);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	Options options;
	options.define("m|md|markdown=b", "Display in markdown table");
	options.process(argc, argv);
	bool markdownQ = options.getBoolean("markdown");
	MidiFile midifile(options.getArg(1));
	midifile.makeAbsoluteTicks();
	midifile.doTimeAnalysis();
	midifile.joinTracks();
	midifile.linkNotePairs();
	double tpq = midifile.getTPQ();

	vector<MidiEvent*> noteons;
	noteons.reserve(midifile[0].getEventCount() / 2);
	for (int i=0; i<midifile[0].getEventCount(); i++) {
		MidiEvent& event = midifile[0][i];
		if (event.isNoteOn()) {
			noteons.push_back(&midifile[0][i]);
		} else if (event.isTempo()) {
			if (!markdownQ) {
				cout << "# Tempo: " << event.getTempoBPM() << " at tick " << event.tick << endl;
			}
		}
	}

	if (markdownQ) {
		cout << "| Track |\tQ |\tQdur |\tQioi |\tTick |\tTdur |\tSec |\tSdur |\tNote |\n";
		cout << "| ----- | --- | ---- | ----- | ----- | ----- | ---- | ----- | ----- |\n";
	} else {
		cout << "Track\tQ\tQdur\tQioi\tTick\tTdur\tSec\tSdur\tNote\n";
	}

	for (int i=0; i<(int)noteons.size(); i++) {
		MidiEvent& event = *noteons[i];

		if (markdownQ) { cout << "|"; }
		cout << event.track << '\t';

		if (markdownQ) { cout << "|"; }
		cout << roundToThreeDigits(event.tick/tpq) << '\t';
		if (markdownQ) { cout << "|"; }
		cout << roundToThreeDigits(event.getTickDuration()/tpq) << '\t';

		if (i < (int)noteons.size() - 1) {
			int tick1 = event.tick;
			int tick2 = noteons[i+1]->tick;
			if (markdownQ) { cout << "|"; }
			cout << tick2-tick1 << '\t';
		} else {
			if (markdownQ) { cout << "|"; }
			cout << "none" << '\t';
		}

		if (markdownQ) { cout << "|"; }
		cout << event.tick << '\t';
		if (markdownQ) { cout << "|"; }
		cout << event.getTickDuration() << '\t';

		if (markdownQ) { cout << "|"; }
		cout << roundToThreeDigits(event.seconds) << '\t';
		if (markdownQ) { cout << "|"; }
		cout << roundToThreeDigits(event.getDurationInSeconds()) << '\t';

		if (markdownQ) { cout << "|"; }
		cout << getPitchName(event);
		if (markdownQ) { cout << "|"; }
		cout << endl;
	}

	return 0;
}



//////////////////////////////
//
// roundToThreeDigits --
//

double roundToThreeDigits(double input) {
	double output = int(input * 1000.0 + 0.5) / 1000.0;
	return output;
}



//////////////////////////////
//
// getPitchName --
//

string getPitchName(MidiMessage& message) {
	if (!message.isNote()) {
		return "X";
	}
	int key = message.getP1();
	int chroma = key % 12;
	int octave = key / 12 - 1;
	string output;
	switch (chroma) {
		case  0: output = "C";  break;
		case  1: output = "C#"; break;
		case  2: output = "D";  break;
		case  3: output = "Eb"; break;
		case  4: output = "E";  break;
		case  5: output = "F";  break;
		case  6: output = "F#"; break;
		case  7: output = "G";  break;
		case  8: output = "Ab"; break;
		case  9: output = "A";  break;
		case 10: output = "Bb"; break;
		case 11: output = "B";  break;
	}
	output += to_string(octave);
	return output;
}



