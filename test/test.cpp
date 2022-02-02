#include <sstream>

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "MidiFile.h"

TEST_CASE("Writing and reading an empty MIDI file works") {
    smf::MidiFile midifile1;
    REQUIRE(midifile1.size() == 1);
    std::stringstream stream;
    midifile1.write(stream);
    REQUIRE(midifile1.status());
    smf::MidiFile midifile2;
    midifile2.read(stream);
    REQUIRE(midifile2.size() == 1);
    REQUIRE(midifile2.status());
}
