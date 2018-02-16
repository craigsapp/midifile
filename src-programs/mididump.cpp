//
// Programmer:    Ziemowit Laski <zlaski@ziemas.net>
// Creation Date: Wed, 07 Feb 2018 19:07:27 -0800
// Filename:      midifile/src-programs/mididump.cpp
// Syntax:        C++11
//
// Description:   A simple MIDI dumper showing all the
//                MIDI events in (annotated) raw form.


#include "MidiFile.h"
#include "MidiData.h"
#include "Options.h"

#include <stdio.h>


int main(int argc, char** argv) {
	Options options;
	options.define("t|track=i:-1", "dump only single track");
	options.process(argc, argv);
	if (options.getArgCount() != 1) {
		fprintf(stderr, "Usage: %s midi_filename\n", options.getCommand().c_str());
		exit(1);
	}

	int trk = options.getInt("track");
	const char *filename = options.getArgument(1).c_str();

	MidiFile m(filename);
	int numTracks = m.getTrackCount();

	printf("File:     %s\n", filename);
	printf("TPQN:     %d\n", m.getTicksPerQuarterNote());
	printf("Tracks:   %d\n\n", numTracks);

	for (int t = 0; t < numTracks; ++t) {
		if (trk != -1 && t != trk) {
			continue;
		}
		printf("TRACK %02d\n\n", t);

		// print the list of events in the track
		int eventcount = m.getEventCount(t);
		for (int i = 0; i<eventcount; i++) {
			MidiEvent ev = m.getEvent(t, i);
			printf("%08X:", ev.tick);
			for (int j = 0; j < (int)ev.size(); ++j) {
				printf(" %02X", ev[j]);
			}
			printf("\n");
		}
		printf("\n");
	}

	printf("End:      %s\n", filename);
	return 0;
}

///////////////////////////////////////////////////////////////////////////
