#include "MidiFile.h"
#include "Options.h"

using namespace std;

void swapNotes(vector<MidiEvent*>& notes, int index1, int index2);

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   if (options.getArgCount() != 2) {
      cerr << "two MIDI filenames are required.\n";
      exit(1);
   }
   MidiFile midifile;
   midifile.read(options.getArg(1));
   if (!midifile.status()) {
      cerr << "Error reading MIDI file " << options.getArg(1) << endl;
      exit(1);
   }
   midifile.linkNotePairs();
   vector<MidiEvent*> notes;
   notes.reserve(123456);
   for (int track=0; track<midifile.size(); track++) {
      for (int event=0; event<midifile[track].size(); event++) {
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

void swapNotes(vector<MidiEvent*>& notes, int index1, int index2) {
   MidiEvent* noteon1  = notes[index1];
   MidiEvent* noteon2  = notes[index2];
   MidiEvent* noteoff1 = notes[index1]->getLinkedEvent();
   MidiEvent* noteoff2 = notes[index2]->getLinkedEvent();
   if (noteon1  == NULL) { return; }
   if (noteon2  == NULL) { return; }
   if (noteoff1 == NULL) { return; }
   if (noteoff2 == NULL) { return; }
   int pitch1 = noteon1->getKeyNumber();
   int pitch2 = noteon2->getKeyNumber();
   if (pitch1 == pitch2) { return; }
   if (pitch1 < 0) { return; }
   if (pitch2 < 0) { return; }
   noteon1->setKeyNumber(pitch2);
   noteoff1->setKeyNumber(pitch2);
   noteon2->setKeyNumber(pitch1);
   noteoff2->setKeyNumber(pitch1);
}
