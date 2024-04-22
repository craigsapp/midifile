//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Jul  3 11:58:38 PDT 2023
// Last Modified: Mon Jul 10 22:02:03 PDT 2023
// Filename:      tools/mididiss.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/mididiss.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Calculate an average dissonance score.  The input MIDI file
//                is expected to be quantized.  Scores:
//                   -1 = rest (ignore)
//                    0 = unison / octave / single note (no intervals)
//                    1 = other perfect intervals P4 P5
//                    2 = imperfect intervals m3 M3 m6 M6
//                    3 = weak dissonance M2 m7
//                    4 = strong dissonant M7 m9 A4 (other than minor second)
//                    5 = minor second M2
//                The score of a sonority is the maximum value of any interval pairing
//                Then the scores a duration-weighted to calculate an average score for
//                all individual sonority scores.
//
// To do: maybe add an non-duration weighted option for averaging scores.
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


vector<int> Errors(128, 0);

bool dataQ = false;
bool dataNoRestQ = false;
bool intervalQ = false;

class Sonority {
	public:
		Sonority(void) { /* do nothing */};
		~Sonority()    { /* do nothing */};
		vector<int> pitches;
		int startTick   = -1;  // starting tick of sonority (absolute)
		int durTick     = -1;  // duration of sonority in ticks
		double score    = -1;  // sonority score (larger is more dissonant)
		int pitchIndex1 = -1;  // first pitch index of first most dissonant interval
		int pitchIndex2 = -1;  // second pitch index of first more dissonant interval
};


void     addSonority(int tick, vector<Sonority>& sonorities, vector<int>& activeNotes);
int      calculateSonorityScore(Sonority& sonority);
double   calculateFinalScore(vector<Sonority>& sonorities);
void     calculateSonorityDurations(vector<Sonority>& sonorities, MidiFile& midifile);
void     calculateSonorityScores(vector<Sonority>& sonorities);
void     doSonorityAnalysis(vector<Sonority>& sonorities, MidiFile& midifile);
int      getIntervalScore(int a, int b);
void     printSonorityInfo(vector<Sonority>& sonorities, bool restQ = true);
void     processFile(MidiFile& midifile, Options& options);
int      updateActiveNotes(MidiEventList& events, int start, vector<int>& activeNotes);
double   weightedAverage(vector<double>& values, vector<double>& weights);

ostream& operator<<(ostream& output, Sonority& sonority);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("d|data=b", "print data used to calculate dissonance score");
	options.define("D|data-no-rest=b", "print data used to calculate dissonance score without rest sonorities");
	options.define("i|interval-scores=b", "print interval");
	options.process(argc, argv);
	dataQ = options.getBoolean("data");
	dataNoRestQ = options.getBoolean("data-no-rest");
	intervalQ = options.getBoolean("interval-scores");
	MidiFile midifile;
	if (options.getArgCount() == 0) {
		midifile.read(cin);
		processFile(midifile, options);
	} else {
		int count = options.getArgCount();
		for (int i=0; i<count; i++) {
			string filename = options.getArg(i+1);
			if (count > 1) {
				cout << filename << '\t';
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
// operator<< --
//

ostream& operator<<(ostream& out, Sonority& sonority) {
	double s = sonority.score;
	if (s > 0.0) {
		s = int(s * 100.0 + 0.5)/100.0;
	} else {
		s = int(s * 100.0 - 0.5)/100.0;
	}
	out << "TICK:" << sonority.startTick << '\t';
	if (sonority.startTick < 100) {
		out << '\t';
	}
	out << "DUR:"<< sonority.durTick << '\t';
	if (sonority.durTick < 1000) {
		out << '\t';
	}
	out << "SCORE:" << s;
	if (sonority.score > 0) {
		out << '\t';
	}
	out  << "\tPITCHES: ";
	for (int i=0; i<(int)sonority.pitches.size(); i++) {
		if ((i == sonority.pitchIndex1) || (i == sonority.pitchIndex2)) {
			out << "(";
		}
		out << sonority.pitches[i];
		if ((i == sonority.pitchIndex1) || (i == sonority.pitchIndex2)) {
			out << ")";
		}
		if (i < (int)sonority.pitches.size() - 1) {
			out << ", ";
		}
	}
	out << "\tI:";
	// print most dissonant interval, -1 for no interval (rest)
	if (sonority.pitchIndex1 < 0) {
		out << "-1";
	} else if (sonority.pitchIndex2 < 0) {
		out << "-1";
	} else {
		int p1 = sonority.pitches[sonority.pitchIndex1];
		int p2 = sonority.pitches[sonority.pitchIndex2];
		if (p1 < 0) {
			out << -1;
		}
		if (p2 < 0) {
			out << -1;
		}
		int diff = p2 - p1;
		if (diff < 0) {
			diff = -diff;
		}
		if (diff == 1) {
			out << 1;
		} else {
			diff = diff % 12;
			if (diff == 1) {
				out << diff + 12;
			} else {
				out << diff;
			}
		}
	}

	return out;
}



//////////////////////////////
//
// processFile --
//

void processFile(MidiFile& midifile, Options& options) {
	midifile.linkNotePairs();
	midifile.joinTracks();

	vector<Sonority> sonorities;
	sonorities.reserve(10000);

	vector<int> activeNotes(128, 0);
	for (int i=0; i<midifile[0].getEventCount(); i++) {
		MidiEvent* event = &midifile[0][i];
		if (!(event->isNoteOn() ||event->isNoteOff())) {
			continue;
		}
		int channel = event->getChannel();
		if (channel == 0x09) {
			// ignore drum track
			continue;
		}
		int tick = event->tick;
		i = updateActiveNotes(midifile[0], i, activeNotes);
		addSonority(tick, sonorities, activeNotes);
	}
	doSonorityAnalysis(sonorities, midifile);
	if (dataQ || dataNoRestQ) {
		printSonorityInfo(sonorities, !dataNoRestQ);
	}
}



//////////////////////////////
//
// printSonorityInfo --
//

void printSonorityInfo(vector<Sonority>& sonorities, bool restQ) {
	for (int i=0; i<(int)sonorities.size(); i++) {
		if (restQ) {
			cout << sonorities[i] << endl;
		} else {
			if (sonorities[i].score >= 0.0) {
				cout << sonorities[i] << endl;
			}
		}
	}
}



//////////////////////////////
//
// doSonorityAnalysis --
//

void doSonorityAnalysis(vector<Sonority>& sonorities, MidiFile& midifile) {
	calculateSonorityDurations(sonorities, midifile);
	calculateSonorityScores(sonorities);
	double finalScore = calculateFinalScore(sonorities);
	cout << finalScore << endl;
};




//////////////////////////////
//
// calculateSonorityScores --
//

void calculateSonorityScores(vector<Sonority>& sonorities) {
	for (int i=0; i<(int)sonorities.size(); i++) {
		sonorities[i].score = calculateSonorityScore(sonorities[i]);
	}
}



//////////////////////////////
//
// calculateSonorityScore --
//

int calculateSonorityScore(Sonority& sonority) {
	vector<int>& p = sonority.pitches;
	if (p.empty()) {
		sonority.pitchIndex1 = -1;
		sonority.pitchIndex2 = -1;
		return -1; // rest
	}
	if (p.size() == 1) {
		sonority.pitchIndex1 = 0;
		sonority.pitchIndex2 = 0;
		return 0; // unison equivalent
	}
	int score = -1;
	int pi = -1;
	int pj = -1;
	for (int i=0; i<(int)p.size() - 1; i++) {
		for (int j=i+1; j<(int)p.size(); j++) {
			int testScore = getIntervalScore(p[i], p[j]);
			if (testScore > score) {
				score = testScore;
				pi = p[i];
				pj = p[j];
				sonority.pitchIndex1 = i;
				sonority.pitchIndex2 = j;
			}
		}
	}
	if (intervalQ) {
		cout << "INTERVAL: " << pi << " TO " << pj << " = " << score << endl;
	}
	return score;
}



//////////////////////////////
//
// getIntervalScore --
//    0 = unison / octave
//    1 = other perfect intervals P4 P5
//    2 = imperfect intervals m3 M3 m6 M6
//    3 = weak dissonance M2 m7
//    4 = strong dissonant M7 m9 A4 (other than minor second)
//    5 = minor second M2
//

int getIntervalScore(int a, int b) {

	int unison    = 0;
	int perfect   = 1;
	int imperfect = 2;
	int weak      = 3;
	int strong    = 4;
	int m2        = 5;

	int diff = a - b;
	if (diff < 0) {
		diff = -diff;
	}
	if (diff == 1) {
		return m2;
	}

	switch (diff % 12) {
		case  0: return unison;
		case  1: return strong;
		case  2: return weak;
		case  3: return imperfect;
		case  4: return imperfect;
		case  5: return perfect;
		case  6: return strong;
		case  7: return perfect;
		case  8: return imperfect;
		case  9: return imperfect;
		case 10: return weak;
		case 11: return strong;
	}
	return -1;
}



//////////////////////////////
//
// calculateSonorityDurations --
//

void calculateSonorityDurations(vector<Sonority>& sonorities, MidiFile& midifile) {
	for (int i=0; i<(int)sonorities.size() - 1; i++) {
		int diff= sonorities[i+1].startTick - sonorities[i].startTick;
		sonorities[i].durTick = diff;
	}
	// assuming last note event is a note-off:
	sonorities.back().durTick = 0;
}



///////////////////////////////
//
// updateActiveNotes --
//

int updateActiveNotes(MidiEventList& events, int start, vector<int>& activeNotes) {
	MidiEvent* event = &events[start];
	int startTick = event->tick;
	int i = start;
	for (i=start; i<events.getEventCount(); i++) {
		event = &events[i];
		int tick = event->tick;
		if (tick != startTick) {
			i--;
			break;
		}
		if (!(event->isNoteOn() || event->isNoteOff())) {
			continue;
		}
		int key = event->getKeyNumber();
		if (event->isNoteOn()) {
			activeNotes[key]++;
		} else if (event->isNoteOff()) {
			activeNotes[key]--;
		}
	}
	return i;
}



//////////////////////////////
//
// addSonority -- Add a sonority to the list of sonorities.
//

void addSonority(int tick, vector<Sonority>& sonorities, vector<int>& activeNotes) {
	sonorities.emplace_back();
	sonorities.back().startTick = tick;

	Sonority& curr = sonorities.back();
	for (int i=0; i<128; i++) {
		if (activeNotes[i] > 0) {
			curr.pitches.push_back(i);
		} else if (activeNotes[i] < 0) {
			if (Errors[i] == 0) {
				if (i > 0) {
					cerr << "Error excessive note-offs for pitch " << i << " starting at tick " << tick << endl;
				}
			}
			Errors[i]++;
		}
	}
}



//////////////////////////////
//
// calculateFinalScore --
//

double calculateFinalScore(vector<Sonority>& sonorities) {
	vector<double> scores;
	vector<double> ticks;
	for (int i=0; i<(int)sonorities.size(); i++) {
		if (sonorities[i].pitches.empty()) {
			//rest
			continue;
		}
		if (sonorities[i].durTick <= 0) {
			// strange problem
			continue;
		}
		if (sonorities[i].score < 0) {
			// rest
			continue;
		}
		scores.push_back(sonorities[i].score);
		ticks.push_back(sonorities[i].durTick);
	}
	return weightedAverage(scores, ticks);
}



//////////////////////////////
//
// weightedAverage --
//

double weightedAverage(vector<double>& values, vector<double>& weights) {
	if (values.size() != weights.size()) {
		cerr << "Error: The number of values and weights must be the same." << endl;
		return 0.0;
	}

	double sumValues = 0.0;
	double sumWeights = 0.0;

	for (size_t i = 0; i < values.size(); ++i) {
		sumValues += values[i] * weights[i];
		sumWeights += weights[i];
	}

	if (sumWeights == 0.0) {
		cerr << "Error: Sum of weights is zero." << endl;
		return 0.0;
	}

	return sumValues / sumWeights;
}



