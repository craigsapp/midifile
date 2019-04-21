#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    //melody << "C C G G A A G . F F E E D D C -";
    //bass << "C3 C4 E4 C4 F4 C4 E4 C4 D4 B3 C4 A3 F3 G3 C3 -";
    melody << "C/E/G - G/B/D - A/C/E - F/A/C -";


    // combine tracks
    MidiOutput out;
    out.addTrack(melody);

    // write tracks
    out.write("chords.mid");
    
    out.old_write("working_chords.mid");


    return 0;
}
