#include "MidiFile.h"
#include "Options.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {

   Options options;
   options.process(argc, argv);
   if (options.getArgCount() < 1) {
      cerr << "Usage: " << options.getCommand() << " input(s)\n";
      exit(1);
   }

   MidiFile midifile;
   for (int i=1; i<options.getArgCount(); i++ ) {
      midifile.read(options.getArg(i));
      if (midifile.status()) {
         midifile.write(options.getArg(i));
      } else {
         cerr << "Error reading MIDI file: " << options.getArg(i) << endl;
      }
   }

   return 0;
}
