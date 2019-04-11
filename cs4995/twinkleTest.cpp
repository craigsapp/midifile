#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include "MidiFile.h"
#include "MidiObject.hpp"

using namespace smf;

int main() {

   vector<Note> melody = toMelody("C C G G A A G . F F E E D D C -");
   vector<Note> bass = toMelody("C3 C4 E4 C4 F4 C4 E4 C4 D4 B3 C4 A3 F3 G3 C3 -");
   MidiOutput out;
   out.addTrack(melody);
   out.addTrack(bass);
   out.write("twinkle.mid");

   return 0;
}
