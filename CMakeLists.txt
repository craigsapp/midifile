cmake_minimum_required(VERSION 2.8)

project(midifile C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFiles)
include(GNUInstallDirs)

include_directories(include)

check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(humdrum.h HAVE_HUMDRUM_H)
check_include_files(sys/io.h HAVE_SYS_IO_H)

option(BUILD_TOOLS "build tools" ON)

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
    src/Options.cpp
    src/Binasc.cpp
    src/MidiEvent.cpp
    src/MidiEventList.cpp
    src/MidiFile.cpp
    src/MidiMessage.cpp
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

install(TARGETS midifile
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_TOOLS)
  add_subdirectory(tools)
endif()

install(FILES ${HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/midifile)
