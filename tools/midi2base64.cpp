//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun 24 20:54:59 PDT 2021
// Last Modified: Thu Jun 24 20:57:18 PDT 2021
// Filename:      midifile/tools/midi2base64.cpp
// Syntax:        C++11
//
// Description:   Test of base64 export of Standard MIDI files.
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
   opts.define("w|width=i:0", "Line-wrap length for base-64 output");
	opts.process();
	if (opts.getArgCount() != 1) {
		cerr << "Usage: " << opts.getCommand() << " midifile" << endl;
		exit(1);
	}
	int width = opts.getInteger("width");

   MidiFile midifile;
	midifile.read(argv[1]);
   midifile.writeBase64(cout, width);
   return 0;
}


