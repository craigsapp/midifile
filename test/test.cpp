#include <sstream>
#include <vector>
#include <tuple>

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

TEST_CASE("Writing and reading a MIDI file with note on and note off messages works") {
    smf::MidiFile midifile1;

    std::vector<uint8_t> noteOnMessage = { 0x90, 0x3c, 0x7F };
    smf::MidiEvent noteOn(0, 1, noteOnMessage);
    std::vector<uint8_t> noteOffMessage = { 0x80, 0x3c, 0x7F };
    smf::MidiEvent noteOff(100, 1, noteOffMessage);
    std::vector<uint8_t> trackEndMessage = { 0xFF, 0x2F, 0x00 };
    smf::MidiEvent trackEnd(200, 1, trackEndMessage);
    midifile1[0].push_back(noteOn);
    midifile1[0].push_back(noteOff);
    midifile1[0].push_back(trackEnd);

    std::stringstream stream;
    midifile1.write(stream);
    REQUIRE(midifile1.status());
    smf::MidiFile midifile2;
    midifile2.read(stream);

    REQUIRE(midifile1.size() == midifile2.size());
    REQUIRE(midifile1[0].size() == midifile2[0].size());
    for (int i = 0; i < midifile1[0].size(); i++) {
        REQUIRE(midifile1[0][i] == midifile2[0][i]);
    }
}
