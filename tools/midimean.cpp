//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun 29 14:05:26 PDT 2023
// Last Modified: Mon Feb  5 11:51:41 PST 2024
// Filename:      tools/midimean.cpp
// URL:           https://github.com/craigsapp/midifile/blob/master/tools/midimean.cpp
// Syntax:        C++11
// vim:           ts=3
//
// Description:   Calculate the mean pitch of MIDI notes in a midifile,
//                excluding any notes in drum track.  The mean can be weighted
//                by duration, and a specific track or channel can be selected.
//
// Options:       -d   :: weight pitch averages by note durations
//                -s   :: for duration weighting, weight by performance time in seconds
//                -t # :: calculate only for given track (0-indexed)
//                -c # :: calculate only for given channel (1-indexed)
//                -m # :: process notes greater or equal to minimum duration (in quarter notes)
//                -M # :: process notes less than or equal to minimum duration (in quarter notes)
//
// To do:         Maybe add a separate census of notes in the drum track.
//                Allow multiple input midifiles.
//                Add -f option to print filenames when processing
//                   multiple midifile.
//                For multiple tracks, include instrument name meta message in output
//

#include "MidiFile.h"
#include "Options.h"

#include <iostream>

using namespace std;
using namespace smf;


void   processFile               (MidiFile& midifile, Options& options);
double getPitchMeanByNoteAttack  (MidiFile& midifile, int strack = -1, int schan = -1,
                                  double mindur = -1.0, double maxdur = -1.0);
double getPitchMeanByNoteDuration(MidiFile& midifile, int strack = -1, int schan = -1,
                                  double mindur = -1.0, double maxdur = -1.0, bool secondsQ = false);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Options options;
	options.define("d|duration|duration-weighted=b", "Duration weighted mean.");
	options.define("s|seconds=b",                    "Use seconds when calculating duration weighted mean.");
	options.define("t|track=i:-1",                   "Calculate mean for a given track (0-indexed)");
	options.define("c|channel=i:-1",                 "Calculate mean for a given channel (1-indexed)");
	options.define("m|min|minimum-duration=d:-1.0",   "Minimum duration to consider (in quarter notes)");
	options.define("M|max|maximum-duration=d:-1.0",  "Maximum duration to consider (in quarter notes)");
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
	double mean = 0.0;
	bool secondsQ = options.getBoolean("seconds");

	int track = options.getInteger("track");
	if (track >= midifile.getTrackCount()) {
		cerr << "Track too large: " << track << " setting to " << midifile.getTrackCount() << endl;
		track = midifile.getTrackCount() - 1;
	}
	
	int channel = options.getInteger("channel");
	channel = channel - 1;
	if (channel >= 16) {
		cerr << "Channel too large: " << channel << ": setting to " << 1 << endl;
		track = 0;
	}
	if (channel == 0x09) {
		cerr << "Warning: MIDI channel 10 is not handleded well by this program and has no meaning\n";
	}

	double mindur = options.getDouble("minimum-duration");
	double maxdur = options.getDouble("maximum-duration");

	if (options.getBoolean("duration-weighted")) {
		mean = getPitchMeanByNoteDuration(midifile, track, channel, mindur, maxdur, secondsQ);
	} else {
		mean = getPitchMeanByNoteAttack(midifile, track, channel, mindur, maxdur);
	}
	cout << mean << endl;
}



//////////////////////////////
//
// getPitchMeanByNoteDuration -- Get the mean pitch height weighted by the
//     duration of the notes.
//      strack = selected track (default negative value means calculate for all tracks)
//      schan  = selected channel (default negative value means calculate for all channels)
//      mindur = minimum duration of notes to consider in quarter note units (negative values means all durations)
//      maxdur = maximum duration of notes to consider in quarter note units (negative values means all durations)
//      secondsQ  = weight durations in units of seconds (performance time), default false.
//

double getPitchMeanByNoteDuration(MidiFile& midifile, int strack, int schannel, double mindur, double maxdur, bool secondsQ) {
	double dursum = 0.0;
	double weightsum = 0.0;

	// Connect note-ons and note-offs to calculate note durations:
	midifile.linkNotePairs();
	if (secondsQ) {
		// Need to calculate duration of notes in seconds with the following line:
		midifile.doTimeAnalysis();
	}

	if (schannel == 0x09) {
		return -1000.0;
	}

	for (int t=0; t<midifile.getTrackCount(); t++) {
		for (int e=0; e<midifile[t].getEventCount(); e++) {
			MidiEvent &event = midifile[t][e];
			if (!event.isNoteOn()) {
				continue;
			}
			int chan = event.getChannel();
			if (chan == 0x09) {
				// ignore percussion channel
				continue;
			}

			if ((schannel >= 0) && (chan != schannel)) {
				// Calculating for a specific channel, which is not this one.
				continue;
			}

			int track = t;
			if ((strack >= 0) && (strack != track)) {
				// Calculating for a specific track, which is not this one.
				continue;
			}

			if ((mindur > 0.0) || (maxdur > 0.0)) {
				double tpq = midifile.getTicksPerQuarterNote();
				double qdur = event.getTickDuration() / tpq;
				if ((mindur > 0.0) && (maxdur < 0.0)) {
					if (qdur < mindur) {
						continue;
					}
				} else if ((mindur > 0.0) && (maxdur < 0.0)) {
					if (qdur > maxdur) {
						continue;
					}
				} else {
					if (qdur < mindur) {
						continue;
					}
					if (qdur > maxdur) {
						continue;
					}
				}
			
			}

			double dur = 0.0;
			if (secondsQ) {
				dur = event.getDurationInSeconds();
			} else {
				dur = event.getTickDuration();
			}
			int key = event.getKeyNumber();
			
			dursum += dur;
			weightsum += key * dur;
		}
	}
	if ((double)dursum > 0.0) {
		return weightsum/dursum;
	} else {
		// No notes with duration in the file/track/channel.
		return -1000.0;
	}
}



//////////////////////////////
//
// getPitchMeanByNoteAttack --
//      strack = selected track (default negative value means calculate for all tracks)
//      schan  = selected channel (default negative value means calculate for all channels)
//      mindur = minimum duration of notes to consider in quarter note units (negative values means all durations)
//      maxdur = maximum duration of notes to consider in quarter note units (negative values means all durations)
//

double getPitchMeanByNoteAttack(MidiFile& midifile, int strack, int schan, double mindur, double maxdur) {
	double sum = 0.0;
	int count = 0;

	if ((mindur > 0.0) || (maxdur > 0.0)) {
		// Need to calculate duration of notes (in ticks):
		midifile.linkNotePairs();
	}

	for (int t=0; t<midifile.getTrackCount(); t++) {
		for (int e=0; e<midifile[t].getEventCount(); e++) {
			MidiEvent &event = midifile[t][e];
			if (!event.isNoteOn()) {
				continue;
			}
			int chan = event.getChannel();
			if ((schan >= 0) && (chan != schan)) {
				// Calculating for a specific channel, which is not this one.
				continue;
			}

			int track = t;
			if ((strack >= 0) && (track != strack)) {
				// Calculating for a specific track, which is not this one.
				continue;
			}

			if (chan == 0x09) {
				// ignore percussion channel
				continue;
			}

			if ((mindur > 0.0) || (maxdur > 0.0)) {
				double tpq = midifile.getTicksPerQuarterNote();
				double qdur = event.getTickDuration() / tpq;
				if ((mindur > 0.0) && (maxdur < 0.0)) {
					if (qdur < mindur) {
						continue;
					}
				} else if ((mindur > 0.0) && (maxdur < 0.0)) {
					if (qdur > maxdur) {
						continue;
					}
				} else {
					if (qdur < mindur) {
						continue;
					}
					if (qdur > maxdur) {
						continue;
					}
				}
			
			}

			sum += event.getKeyNumber();
			count++;
		}
	}
	return sum/count;
}



