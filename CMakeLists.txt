cmake_minimum_required(VERSION 2.8)

project(midifile C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFiles)

include_directories(include)

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(humdrum.h HAVE_HUMDRUM_H)
check_include_files(sys/io.h HAVE_SYS_IO_H)

##############################
##
## Operating-system specific settings:
##

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    option(STATIC_CRT "Use static CRT libraries" OFF)

    # Rewrite command line flags to use /MT if necessary
    if(STATIC_CRT)
        foreach(flag_var
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()
endif()


##############################
##
## Library:
##

set(SRCS
    src-library/Options.cpp
    src-library/Binasc.cpp
    src-library/MidiEvent.cpp
    src-library/MidiEventList.cpp
    src-library/MidiFile.cpp
    src-library/MidiMessage.cpp
)

set(HDRS
    include/Binasc.h
    include/MidiEvent.h
    include/MidiEventList.h
    include/MidiFile.h
    include/MidiMessage.h
    include/Options.h
)

add_library(midifile STATIC ${SRCS} ${HDRS})

##############################
##
## Programs:
##

add_executable(80off src-programs/80off.cpp)
add_executable(asciimidi src-programs/asciimidi.cpp)
add_executable(binasc src-programs/binasc.cpp)
add_executable(createmidifile src-programs/createmidifile.cpp)
add_executable(createmidifile2 src-programs/createmidifile2.cpp)
add_executable(drumtab src-programs/drumtab.cpp)
add_executable(durations src-programs/durations.cpp)
add_executable(mid2mat src-programs/mid2mat.cpp)
add_executable(mid2mtb src-programs/mid2mtb.cpp)
add_executable(mid2svg src-programs/mid2svg.cpp)
add_executable(midi2binasc src-programs/midi2binasc.cpp)
add_executable(midi2melody src-programs/midi2melody.cpp)
add_executable(midi2notes src-programs/midi2notes.cpp)
add_executable(midi2skini src-programs/midi2skini.cpp)
add_executable(midi2text src-programs/midi2text.cpp)
add_executable(midicat src-programs/midicat.cpp)
add_executable(midimixup src-programs/midimixup.cpp)
add_executable(miditime src-programs/miditime.cpp)
add_executable(perfid src-programs/perfid.cpp)
add_executable(retick src-programs/retick.cpp)
add_executable(shutak src-programs/shutak.cpp)
add_executable(smfdur src-programs/smfdur.cpp)
add_executable(stretch src-programs/stretch.cpp)
add_executable(sysextest src-programs/sysextest.cpp)
add_executable(text2midi src-programs/text2midi.cpp)
add_executable(textmidi src-programs/textmidi.cpp)
add_executable(toascii src-programs/toascii.cpp)
add_executable(tobin src-programs/tobin.cpp)
add_executable(tobinary src-programs/tobinary.cpp)
add_executable(todec src-programs/todec.cpp)
add_executable(tohex src-programs/tohex.cpp)
add_executable(type0 src-programs/type0.cpp)
add_executable(vlv src-programs/vlv.cpp)

target_link_libraries(80off midifile)
target_link_libraries(asciimidi midifile)
target_link_libraries(binasc midifile)
target_link_libraries(createmidifile midifile)
target_link_libraries(createmidifile2 midifile)
target_link_libraries(drumtab midifile)
target_link_libraries(durations midifile)
target_link_libraries(mid2mat midifile)
target_link_libraries(mid2mtb midifile)
target_link_libraries(mid2svg midifile)
target_link_libraries(midi2binasc midifile)
target_link_libraries(midi2melody midifile)
target_link_libraries(midi2notes midifile)
target_link_libraries(midi2skini midifile)
target_link_libraries(midi2text midifile)
target_link_libraries(midicat midifile)
target_link_libraries(midimixup midifile)
target_link_libraries(miditime midifile)
target_link_libraries(perfid midifile)
target_link_libraries(retick midifile)
target_link_libraries(shutak midifile)
target_link_libraries(smfdur midifile)
target_link_libraries(stretch midifile)
target_link_libraries(sysextest midifile)
target_link_libraries(text2midi midifile)
target_link_libraries(textmidi midifile)
target_link_libraries(toascii midifile)
target_link_libraries(tobin midifile)
target_link_libraries(tobinary midifile)
target_link_libraries(todec midifile)
target_link_libraries(tohex midifile)
target_link_libraries(type0 midifile)
target_link_libraries(vlv midifile)

if(HAVE_UNISTD_H AND HAVE_SYS_IO_H)
    add_executable(midi2beep src-programs/midi2beep.cpp)

    target_link_libraries(midi2beep midifile)
endif()

# The following programs require headers from humextra repository.
# <https://github.com/humdrum-tools/humextra>
if(HAVE_HUMDRUM_H)
    add_executable(henonfile src-programs/henonfile.cpp)
    add_executable(mid2hum src-programs/mid2hum.cpp)
    add_executable(midiexcerpt src-programs/midiexcerpt.cpp)
    add_executable(peep2midi src-programs/peep2midi.cpp)

    target_link_libraries(henonfile midifile)
    target_link_libraries(mid2hum midifile)
    target_link_libraries(midiexcerpt midifile)
    target_link_libraries(peep2midi midifile)
endif()
