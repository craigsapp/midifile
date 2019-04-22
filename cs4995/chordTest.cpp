#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    melody << "C/E/G - B_1/D/G - C/E/A - C/F/A -";


    // combine tracks
    MidiOutput out;
    out.addTrack(melody);

    // write tracks
    out.write("chords.mid");



    return 0;
}
