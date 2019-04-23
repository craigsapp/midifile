#include "MidiOutput.hpp"
#include "Note.hpp"
#include "Key.hpp"
#include "Track.hpp"

using namespace smf;

int main() {

    // create tracks
    Track melody;
    // melody << "4( C E G F ) 1( E ) 8( E/G/B E/G/B E G E D C C ) 2( A )";
    melody << "4(C E G F) 1(E)";

    // combine tracks
    MidiOutput out;
    out.addTrack(melody);

    // write tracks
    out.write("notesDiv.mid");



    return 0;
}
