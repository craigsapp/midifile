#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    melody << "C/E/G - G/B/D - A/C/E - F/A/C -";


    // combine tracks
    MidiOutput out;
    out.addTrack(melody);

    // write tracks
    out.write("chords.mid");
    


    return 0;
}
