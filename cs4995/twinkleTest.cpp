#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody, bass;
    melody << "C C G G A A G . F F E E D D C -";
    bass << "C3 C4 E4 C4 F4 C4 E4 C4 D4 B3 C4 A3 F3 G3 C3 -";

    // combine tracks
    MidiOutput out;
    out.addTrack(melody);
    out.addTrack(bass);

    // write tracks
    out.write("twinkle.mid");

    return 0;
}
