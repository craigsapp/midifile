#include "MidiFile.h"
#include "Options.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {

   Options options;
   options.define("0|O|o|z|9|zero=b", 
      "Set note-offs to note-on messages with zero velocity.");
   options.process(argc, argv);
   bool zeroQ = options.getBoolean("zero");
   if (options.getArgCount() != 2) {
      cerr << "Usage: " << options.getCommand() << " input.mid output.mid\n";
      exit(1);
   }

   MidiFile midifile;
   midifile.read(options.getArg(1));
   if (!midifile.status()) {
      cerr << "Error: could not read file" << options.getArg(1) << endl;
      exit(1);
   }
   for (int track=0; track<midifile.size(); track++) {
      for (int event=0; event<midifile[track].size(); event++) {
         if (!midifile[track][event].isNoteOff()) {
            continue;
         }
         if (zeroQ) {
            midifile[track][event].setCommandNibble(0x90);
            midifile[track][event][2] = 0;
         } else {
            midifile[track][event].setCommandNibble(0x80);
         }
	}
   }
   midifile.write(options.getArg(2));

   return 0;
}
