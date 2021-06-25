//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun 24 21:02:37 PDT 2021
// Last Modified: Thu Jun 24 21:02:40 PDT 2021
// Filename:      midifile/tools/base642midi.cpp
// Syntax:        C++11
//
// Description:   Test of base64 import of Standard MIDI files.
//

#include "Options.h"
#include "MidiFile.h"
#include <iostream>

using namespace std;
using namespace smf;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   Options opts;
	opts.setOptions(argc, argv);
   opts.define("o|output=s:test.mid", "Output filename");
	opts.process();
	if (opts.getArgCount() != 0) {
		cerr << "Usage: cat base64.txt | " << opts.getCommand() << " -o output.mid " << endl;
		exit(1);
	}

	string filename = opts.getString("output");

   MidiFile midifile;
	midifile.readBase64(cin);
	midifile.write(filename);
   return 0;
}


