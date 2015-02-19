Midifile: C++ MIDI file parsing library
=======================================

Midifile is a library of C++ classes for reading/writing 
Standard MIDI files. The three main components of the 
source code are:

# [src-library](https://github.com/craigsapp/midifile/tree/master/src-library) &mdash; Source code for the classes in the library.
# [include](https://github.com/craigsapp/midifile/tree/master/include) &mdash; Include files for the classes in the library.
# [src-programs](https://github.com/craigsapp/midifile/tree/master/src-programs) &mdash; Example programs which use the library.

The library consists of 6 classes:

* MidiFile &mdash; The main interface for dealing with MIDI files.  The MidiFile class appears as a two dimensional array: the first dimension is a list of tracks, and the second dimension is a list of MidiEvents.
* MidiEvent &mdash; The primary storage unit for MidiMessages in a MidiFile.  The class consists of a (delta)tick timestamp and a MIDI (or meta) message.
* MidiEventList &mdash; A data structure that manages the list of MidiEvents in a track.
* MidiMessage &mdash; The base class for MidiEvents.  This is a vector&lt;uchar&gt; of bytes in a MIDI (or meta) message.
* Binasc &mdash; A helper class for MidiFile which allows reading/writing of MIDI files in an ASCII format describing the bytes of the binary Standard MIDI File.
* Options &mdash; A convenience class used for parsing command-line options in the example programs.


Compiling
----------

The library can be compiled with the command:
``` bash
make library
```

This will create the file `lib/libmidifile.a` which can be used to link
to programs that use the library.  Example programs can be compiled with
the command:
``` bash
make programs
```
This will compile all example programs in the src-programs directory. To 
compile only a single program, such as `createmidifile`, type:
``` bash
make createmidifile
```
Compiled example programs will be stored in the `bin` directory.


Documentation
-------------

Documentation is under construction at 
[http://midifile.sapp.org](http://midifile.sapp.org).



