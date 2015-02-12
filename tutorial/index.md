---
layout: default
title: programming tutorial
---


<h2> Print a list of notes attacks in a MIDI file </h2>

The following example program will print a list of all note-ons
in the MIDI file.


``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
using namespace std;

int main(int argc char** argv) {
   Options options;
   options.process(argc, argv);
   if (options.getArgCount() != 1) {
      cerr << "At least one MIDI filename is required.\n";
      exit(1);
   }
   MidiFile midifile;
   midifile.read(options.getArg(1));
   midifile.joinTracks();
   midifile.absoluteTime();
   int track = 0;
   for (int i=0; i<midifile.getEventCount(track); i++) {
      if (!midifile[track][i].isNoteOn()) {
         continue;
      }
      cout << midifile[track][i].time 
           << '\t' << midifile[track][i].data[2] 
           << endl;
   }
}
```

