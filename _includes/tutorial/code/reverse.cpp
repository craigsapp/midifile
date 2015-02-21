#include "MidiFile.h"
#include "Options.h"

using namespace std;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   if (options.getArgCount() != 2) {
      cerr << "two MIDI filenames are required.\n";
      exit(1);
   }
   MidiFile midifile;
   midifile.read(options.getArg(1));
   midifile.linkNotePairs();
   vector<MidiEvent*> notes;
   notes.reserve(123456);
   for (int track=0; track<midifile.getNumTracks(); track++) {
      for (int event=0; event<midifile.getNumEvents(track); event++) {
         if (midifile[track][event].isNoteOn()) {
            if (midifile[track][event].isLinked()) {
               notes.push_back(&midifile[track][event]);
            }
         }
      }
   }
   int count = notes.size();
   for (int i=0; i<count/2; i++) {
      swapNotes(notes, i, count-1-i);
   }
   midifile.write(options.getArg(2));
   return 0;
}
