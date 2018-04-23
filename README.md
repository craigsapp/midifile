Midifile: C++ MIDI file parsing library
=======================================


[![Travis Build Status](https://travis-ci.org/craigsapp/midifile.svg?branch=master)](https://travis-ci.org/craigsapp/midifile) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/oo393u60ut1rtbf3?svg=true)](https://ci.appveyor.com/project/craigsapp/midifile)

Midifile is a library of C++ classes for reading/writing Standard
MIDI files.  The library consists of 6 classes:


<table>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiFile">MidiFile</a>
</td><td>
	The main interface for dealing with MIDI files.  The MidiFile class
	appears as a two dimensional array of MidiEvents: the first dimension
	is a list of tracks, and the second dimension is a list of MidiEvents.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiEventList">MidiEventList</a>
</td><td>
	A data structure that manages the list of MidiEvents for a MIDI file track.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiEvent">MidiEvent</a>
</td><td>
	The primary storage unit for MidiMessages in a MidiFile.  The class
	consists of a tick timestamp (delta or absolute) and a vector of
        MIDI message bytes (or Standard MIDI File meta messages).
</td></tr>


<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiMessage">MidiMessage</a>
</td><td>
	The base class for MidiEvents.  This is a STL vector of
	unsigned bytes representing a MIDI (or meta) message.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/Binasc">Binasc</a>
</td><td>
	A helper class for MidiFile that allows reading/writing of MIDI
	files in an ASCII format describing the bytes of the binary Standard
	MIDI Files.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/Options">Options</a>
</td><td>
	A optional convenience class used for parsing command-line options
	in the example programs.  This class can be removed from the library
        since it is not needed for using the MidiFile class.
</td></tr>

</table>


Documentation is under construction at
[http://midifile.sapp.org](http://midifile.sapp.org).
Essential examples for reading and writing MIDI files
are given below.


Downloading
-----------

You can download as a ZIP file from the Github page for the midifile library,
or if you use git, then download with this command:

``` bash
git clone https://github.com/craigsapp/midifile
```

This will create the `midifile` directory with the source code for the library.



Compiling with GCC
------------------

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
This will compile all example programs in the src-programs directory.  Compiled
example programs will be stored in the `bin` directory.  To compile both the
library and the example programs all in one step, type:
``` bash
make
```

To compile only a single program, such as `createmidifile`, type:
``` bash
make createmidifile
```
You can also place your own programs in `src-programs`, such as `myprogram.cpp`
and to compile type:
``` bash
make myprogram
```
The compiled program will be `bin/myprogram`.


Using in your own project
-------------------------

The easiest way to use the midifile library in your own project is to
copy the header files in the `include` directory and the source-code
files in the `src-library` directory into your own project.  You do not
need to copy `Options.h` or `Options.cpp` since the `MidiFile` class is
not dependent on them.  The [verovio](https://github.com/rism-ch/verovio)
and [midiroll](https://github.com/craigsapp/midiroll) projects on Github
both use this method to use the midifile library.  Alternatively, you
can fork the midifile repository and build a compiled library file of
the source code that can be copied with the `include` directory contents
into your project.


MIDI file reading examples
--------------------------

The following program lists all MidiEvents in a MIDI file. The program
iterates over each track, printing a list of all MIDI events in the track.
For each event, the absolute tick timestamp for the performance time of
the MIDI message is given, followed by the message itself as a list of
hex bytes.

You can run the `MidiFile::doTimeAnalysis()` function to convert
the absolute tick timestamps into seconds, according to any tempo
meta-messages in the file (using a default tempo of 120 quarter notes
per minute if there are no tempo meta-messages).  The absolute starting
time of the event is shown in the second column of the program's output.

The `MidiFile::linkNotePairs()` function can be used to match note-ons
and note-offs.  When this is done, you can access the duration of the
note with `MidiEvent::getDurationInSeconds()` for note-on messages. The
note durations are shown in the third column of the program's output.

Note that the midifile library classes are in the `smf` namespace,
so `using namespace smf;` or `smf::` prefixes are needed to access
the classes.

``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) {
      midifile.read(cin);
   } else {
      midifile.read(options.getArg(1));
   }
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();

   int tracks = midifile.getTrackCount();
   cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
   if (tracks > 1) {
      cout << "TRACKS: " << tracks << endl;
   }
   for (int track=0; track<tracks; track++) {
      if (tracks > 1) {
         cout << "\nTrack " << track << endl;
      }
      cout << "Tick\tSeconds\tDur\tMessage" << endl;
      for (int event=0; event<midifile[track].size(); event++) {
         cout << dec << midifile[track][event].tick;
         cout << '\t' << dec << midifile[track][event].seconds;
         cout << '\t';
         if (midifile[track][event].isNoteOn()) {
            cout << midifile[track][event].getDurationInSeconds();
         }
         cout << '\t' << hex;
         for (int i=0; i<midifile[track][event].size(); i++) {
            cout << (int)midifile[track][event][i] << ' ';
         }
         cout << endl;
      }
   }

   return 0;
}
```

The above example program will read the first filename it finds on
the command-line, or it will read from standard input if no arguments
are found.  Both binary standard MIDI files and ASCII representations
of MIDI Files can be input into the program.  For example, save the
following text into a file called `twinkle.txt` to use as input data.
This content represents the hex bytes for a standard MIDI file, which
will automatically be parsed by the `MidiFile` class.

```
4d 54 68 64 00 00 00 06 00 01 00 03 00 78 4d 54 72 6b 00 00 00 04 00 ff 2f
00 4d 54 72 6b 00 00 00 76 00 90 48 40 78 80 48 40 00 90 48 40 78 80 48 40
00 90 4f 40 78 80 4f 40 00 90 4f 40 78 80 4f 40 00 90 51 40 78 80 51 40 00
90 51 40 78 80 51 40 00 90 4f 40 81 70 80 4f 40 00 90 4d 40 78 80 4d 40 00
90 4d 40 78 80 4d 40 00 90 4c 40 78 80 4c 40 00 90 4c 40 78 80 4c 40 00 90
4a 40 78 80 4a 40 00 90 4a 40 78 80 4a 40 00 90 48 40 81 70 80 48 40 00 ff
2f 00 4d 54 72 6b 00 00 00 7d 00 90 30 40 78 80 30 40 00 90 3c 40 78 80 3c
40 00 90 40 40 78 80 40 40 00 90 3c 40 78 80 3c 40 00 90 41 40 78 80 41 40
00 90 3c 40 78 80 3c 40 00 90 40 40 78 80 40 40 00 90 3c 40 78 80 3c 40 00
90 3e 40 78 80 3e 40 00 90 3b 40 78 80 3b 40 00 90 3c 40 78 80 3c 40 00 90
39 40 78 80 39 40 00 90 35 40 78 80 35 40 00 90 37 40 78 80 37 40 00 90 30
40 81 70 80 30 40 00 ff 2f 00
```

Below is the output from the example program given the above input data.  The
TPQ value is the ticks-per-quarter-note value from the MIDI header.  In
this example, each quarter note has a duration of 120 MIDI file ticks.  The
above MIDI file contains three tracks, with the first track (the expression
track, having no content other than the end-of-track meta message, `ff 2f 00`
in hex bytes.  The second track starts with a MIDI note-on message `90 48 40`
(in hex) which will start playing MIDI note 72 (C pitch one octave above
middle C) with a medium loudness (40 hex = 64 in decimal notation).

<pre>
TPQ: 120
TRACKS: 3

Track 0
Tick	Seconds	Dur	Message
0	0		ff 2f 0

Track 1
Tick	Seconds	Dur	Message
0	0	0.5	90 48 40
120	0.5		80 48 40
120	0.5	0.5	90 48 40
240	1		80 48 40
240	1	0.5	90 4f 40
360	1.5		80 4f 40
360	1.5	0.5	90 4f 40
480	2		80 4f 40
480	2	0.5	90 51 40
600	2.5		80 51 40
600	2.5	0.5	90 51 40
720	3		80 51 40
720	3	1	90 4f 40
960	4		80 4f 40
960	4	0.5	90 4d 40
1080	4.5		80 4d 40
1080	4.5	0.5	90 4d 40
1200	5		80 4d 40
1200	5	0.5	90 4c 40
1320	5.5		80 4c 40
1320	5.5	0.5	90 4c 40
1440	6		80 4c 40
1440	6	0.5	90 4a 40
1560	6.5		80 4a 40
1560	6.5	0.5	90 4a 40
1680	7		80 4a 40
1680	7	1	90 48 40
1920	8		80 48 40
1920	8		ff 2f 0

Track 2
Tick	Seconds	Dur	Message
0	0	0.5	90 30 40
120	0.5		80 30 40
120	0.5	0.5	90 3c 40
240	1		80 3c 40
240	1	0.5	90 40 40
360	1.5		80 40 40
360	1.5	0.5	90 3c 40
480	2		80 3c 40
480	2	0.5	90 41 40
600	2.5		80 41 40
600	2.5	0.5	90 3c 40
720	3		80 3c 40
720	3	0.5	90 40 40
840	3.5		80 40 40
840	3.5	0.5	90 3c 40
960	4		80 3c 40
960	4	0.5	90 3e 40
1080	4.5		80 3e 40
1080	4.5	0.5	90 3b 40
1200	5		80 3b 40
1200	5	0.5	90 3c 40
1320	5.5		80 3c 40
1320	5.5	0.5	90 39 40
1440	6		80 39 40
1440	6	0.5	90 35 40
1560	6.5		80 35 40
1560	6.5	0.5	90 37 40
1680	7		80 37 40
1680	7	1	90 30 40
1920	8		80 30 40
1920	8		ff 2f 0
</pre>

The default behavior of the `MidiFile` class is to store the absolute
tick times of MIDI events, available in `MidiEvent::tick`, which is the
tick time from the start of the file to the current event.  In standard
MIDI files, tick are stored as delta values, where the tick indicates the
duration to wait since the previous message in a track.  To access the
delta tick values, you can either (1) subtrack the current tick time from
the previous tick time in the list, or call `MidiFile::makeDeltaTime()`
to convert the absolute tick values into delta tick values.

The `MidiFile::joinTracks()` function can be used to convert multi-track
data into a single time sequence.  The `joinTrack()` operation can be
reversed by calling the `MidiFile::splitTracks()` function.  Here is a sample
of program that joins the `MidiEvents` into a single track so that the
data can be processed in a single loop:

``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() > 0) {
      midifile.read(options.getArg(1));
   } else {
      midifile.read(cin);
   }

   cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
   cout << "TRACKS: " << midifile.getTrackCount() << endl;

   midifile.joinTracks();
   // midifile.getTrackCount() will now return "1", but original
   // track assignments can be seen in .track field of MidiEvent.

   cout << "TICK    DELTA   TRACK   MIDI MESSAGE\n";
   cout << "____________________________________\n";

   MidiEvent* mev;
   int deltatick;
   for (int event=0; event < midifile[0].size(); event++) {
      mev = &midifile[0][event];
      if (event == 0) {
         deltatick = mev->tick;
      } else {
         deltatick = mev->tick - midifile[0][event-1].tick;
      }
      cout << dec << mev->tick;
      cout << '\t' << deltatick;
      cout << '\t' << mev->track;
      cout << '\t' << hex;
      for (int i=0; i < mev->size(); i++) {
         cout << (int)(*mev)[i] << ' ';
      }
      cout << endl;
   }

   return 0;
}
```

Below is the new single-track output.  The first column is the absolute
tick timestamp of the message; the second column is the delta tick value;
the third column is the original track value; and the last column
contains the MIDI message (in hex bytes).

<pre style="font-family:Courier,Lucidatypewriter,monospace; -moz-tab-size: 8; -o-tab-size: 8; -webkit-tab-size: 8; tab-size:8;">
TPQ: 120
TRACKS: 3
TICK    DELTA   TRACK   MIDI MESSAGE
____________________________________
0	0	1	90 48 40
0	0	2	90 30 40
0	0	0	ff 2f 0
120	120	1	80 48 40
120	0	2	80 30 40
120	0	2	90 3c 40
120	0	1	90 48 40
240	120	2	80 3c 40
240	0	1	80 48 40
240	0	2	90 40 40
240	0	1	90 4f 40
360	120	2	80 40 40
360	0	1	80 4f 40
360	0	1	90 4f 40
360	0	2	90 3c 40
480	120	2	80 3c 40
480	0	1	80 4f 40
480	0	2	90 41 40
480	0	1	90 51 40
600	120	2	80 41 40
600	0	1	80 51 40
600	0	1	90 51 40
600	0	2	90 3c 40
720	120	1	80 51 40
720	0	2	80 3c 40
720	0	2	90 40 40
720	0	1	90 4f 40
840	120	2	80 40 40
840	0	2	90 3c 40
960	120	2	80 3c 40
960	0	1	80 4f 40
960	0	2	90 3e 40
960	0	1	90 4d 40
1080	120	1	80 4d 40
1080	0	2	80 3e 40
1080	0	2	90 3b 40
1080	0	1	90 4d 40
1200	120	1	80 4d 40
1200	0	2	80 3b 40
1200	0	2	90 3c 40
1200	0	1	90 4c 40
1320	120	1	80 4c 40
1320	0	2	80 3c 40
1320	0	1	90 4c 40
1320	0	2	90 39 40
1440	120	1	80 4c 40
1440	0	2	80 39 40
1440	0	1	90 4a 40
1440	0	2	90 35 40
1560	120	1	80 4a 40
1560	0	2	80 35 40
1560	0	2	90 37 40
1560	0	1	90 4a 40
1680	120	1	80 4a 40
1680	0	2	80 37 40
1680	0	2	90 30 40
1680	0	1	90 48 40
1920	240	1	80 48 40
1920	0	2	80 30 40
1920	0	1	ff 2f 0
1920	0	2	ff 2f 0
</pre>



MIDI file writing example
--------------------------

Below is an example program to create a MIDI file.  This program will
generate a random sequence of notes and append them to the end of
the track.  By default a `MidiFile` object contains a single track and
will be written as a type-0 MIDI file unless more tracks are added.  After 
adding notes to the track, it must be sorted into time sequence
before being written to a file.


``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <random>
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.define("n|note-count=i:10", "How many notes to randomly play");
   options.define("o|output-file=s",   "Output filename (stdout if none)");
   options.define("i|instrument=i:0",  "General MIDI instrument number");
   options.define("x|hex=b",           "Hex byte-code output");
   options.process(argc, argv);

   random_device rd;
   mt19937 mt(rd());
   uniform_int_distribution<int> starttime(0, 100);
   uniform_int_distribution<int> duration(1, 8);
   uniform_int_distribution<int> pitch(36, 84);
   uniform_int_distribution<int> velocity(40, 100);

   MidiFile midifile;
   int track   = 0;
   int channel = 0;
   int instr   = options.getInteger("instrument");
   midifile.addTimbre(track, 0, channel, instr);

   int tpq     = midifile.getTPQ();
   int count   = options.getInteger("note-count");
   for (int i=0; i<count; i++) {
      int starttick = int(starttime(mt) / 4.0 * tpq);
      int key       = pitch(mt);
      int endtick   = starttick + int(duration(mt) / 4.0 * tpq);
      midifile.addNoteOn (track, starttick, channel, key, velocity(mt));
      midifile.addNoteOff(track, endtick,   channel, key);
   }
   midifile.sortTracks();  // Need to sort tracks since added events are 
                           // appended to track in random tick order.
   string filename = options.getString("output-file");
   if (filename.empty()) {
      if (options.getBoolean("hex")) {
         midifile.writeHex(cout);
      } else {
         cout << midifile;
      }
   } else {
      midifile.write(filename);
   }

   return 0;
}
```

If no output file is specified, the MIDI file contents will be printed
in the Binasc format to standard output, which can be read back into a
MidiFile object and converted into a Standard MIDI file (see the read/write
example further down the page for how to do that):

```
"MThd"			; MIDI header chunk marker
4'6			; bytes to follow in header chunk
2'0			; file format: Type-0 (single track)
2'1			; number of tracks
2'120			; ticks per quarter note

;;; TRACK 0 ----------------------------------
"MTrk"			; MIDI track chunk marker
4'89			; bytes to follow in track chunk
v30	90 '74 '72	; note-on D5
v150	90 '68 '88	; note-on G#4
v0	90 '79 '83	; note-on G5
v60	90 '74 '0	; note-off D5
v150	90 '79 '0	; note-off G5
v30	90 '68 '0	; note-off G#4
v990	90 '60 '100	; note-on C4
v90	90 '60 '0	; note-off C4
v630	90 '83 '69	; note-on B5
v60	90 '83 '0	; note-off B5
v30	90 '56 '51	; note-on G#3
v90	90 '56 '0	; note-off G#3
v390	90 '78 '46	; note-on F#5
v30	90 '60 '78	; note-on C4
v90	90 '78 '0	; note-off F#5
v0	90 '70 '56	; note-on A#4
v60	90 '76 '100	; note-on E5
v90	90 '60 '0	; note-off C4
v30	90 '76 '0	; note-off E5
v60	90 '70 '0	; note-off A#4
v0	ff 2f v0	; end-of-track
```

Here is the MIDI data visualized with the example program
[mid2svg](https://github.com/craigsapp/midifile/blob/master/src-programs/mid2svg.cpp):

![10 random nots](https://user-images.githubusercontent.com/3487289/39096697-5728558e-4608-11e8-9b02-c29f39d85d0f.png)

The `-x` option can be used to output the data as hex byte-codes, the `-n` option 
controls the number of notes, and `-i #` specifies the instrument number
to be used:

``` bash
myprogram -n 100 -x -i 24
```

produces the hex byte-code MIDI file:

```
4d 54 68 64 00 00 00 06 00 00 00 01 00 78 4d 54 72 6b 00 00 03 27 00 c0 18 
1e 90 4d 2f 1e 90 31 5e 00 90 40 42 1e 90 47 55 1e 90 47 00 00 90 31 00 00 
90 32 62 1e 90 43 2d 1e 90 43 00 00 90 3f 5f 1e 90 32 00 1e 90 4d 00 00 90 
47 38 1e 90 51 33 1e 90 40 00 00 90 31 31 00 90 35 3a 1e 90 24 41 00 90 4d 
4f 00 90 4e 32 1e 90 31 00 00 90 51 00 1e 90 4e 00 00 90 48 51 1e 90 3f 00 
00 90 24 00 1e 90 47 00 1e 90 35 00 00 90 2c 61 1e 90 4d 63 3c 90 4d 00 00 
90 48 00 00 90 33 30 1e 90 2c 00 00 90 4d 00 00 90 40 5f 00 90 45 5f 00 90 
3e 58 00 90 3f 45 00 90 24 4a 1e 90 33 00 00 90 3c 3c 1e 90 32 38 1e 90 39 
40 1e 90 53 43 1e 90 40 00 00 90 3f 00 00 90 3e 00 00 90 45 00 00 90 4d 62 
1e 90 24 00 00 90 32 00 00 90 30 42 00 90 2d 28 1e 90 3c 00 00 90 4d 00 00 
90 53 00 00 90 2a 45 3c 90 36 51 1e 90 39 00 00 90 2a 00 00 90 36 4b 3c 90 
36 00 00 90 3e 5c 1e 90 2d 00 1e 90 30 00 00 90 3e 00 3c 90 41 48 00 90 37 
3f 00 90 36 3a 00 90 41 51 00 90 46 38 3c 90 36 00 1e 90 36 00 00 90 46 00 
00 90 36 58 1e 90 41 00 00 90 32 44 00 90 47 2a 1e 90 37 00 00 90 45 2b 1e 
90 41 53 00 90 3e 2d 1e 90 3e 00 00 90 33 28 00 90 29 4b 1e 90 41 00 00 90 
41 00 1e 90 29 00 00 90 49 51 1e 90 47 00 00 90 35 49 00 90 49 43 1e 90 35 
00 00 90 45 00 00 90 36 00 00 90 27 5c 1e 90 32 00 1e 90 49 00 00 90 28 4d 
1e 90 49 00 00 90 33 00 00 90 2e 44 1e 90 29 2b 3c 90 27 00 00 90 24 3e 00 
90 28 53 00 90 52 51 1e 90 4d 4f 00 90 26 5c 1e 90 28 00 00 90 29 00 00 90 
27 32 1e 90 28 00 00 90 2e 00 00 90 4d 00 1e 90 2f 28 1e 90 4a 5a 00 90 47 
43 1e 90 4a 00 00 90 52 00 00 90 24 00 00 90 34 2f 1e 90 2f 00 00 90 3c 5e 
00 90 28 4f 00 90 32 2d 1e 90 26 00 00 90 3c 00 00 90 27 00 00 90 53 63 1e 
90 47 00 1e 90 32 00 00 90 44 5d 00 90 32 40 1e 90 28 00 00 90 46 46 1e 90 
34 00 1e 90 2a 3e 1e 90 53 00 00 90 3a 3f 00 90 53 31 1e 90 28 5f 1e 90 28 
00 00 90 46 00 00 90 53 00 00 90 2a 00 1e 90 32 00 00 90 3a 00 00 90 44 00 
3c 90 33 4d 1e 90 53 57 00 90 54 30 1e 90 38 45 1e 90 2f 46 1e 90 2f 00 00 
90 33 00 1e 90 3a 61 00 90 38 40 1e 90 54 00 00 90 27 37 1e 90 3a 00 00 90 
53 00 00 90 32 29 1e 90 38 00 00 90 40 2b 00 90 36 41 1e 90 38 00 00 90 34 
46 1e 90 27 00 5a 90 36 00 00 90 32 00 00 90 40 00 00 90 48 4a 5a 90 34 00 
00 90 48 00 1e 90 29 62 00 90 3e 4d 1e 90 39 3e 00 90 4a 2d 00 90 40 2d 3c 
90 39 00 00 90 4a 00 00 90 25 58 1e 90 29 00 00 90 41 37 00 90 45 4c 00 90 
4d 64 1e 90 41 00 00 90 3e 00 1e 90 4d 00 3c 90 40 00 00 90 25 00 00 90 39 
4a 3c 90 32 43 1e 90 45 4a 1e 90 45 00 3c 90 45 00 3c 90 32 00 00 90 39 00 
00 ff 2f 00 
```

Visualization with `bin/mid2svg -s 6 -a 12 -v`:

![100 random notes](https://user-images.githubusercontent.com/3487289/39097422-bd085ac2-4610-11e8-832d-f9a1239ff560.png)


Read/Write example
------------------

Here is an example minimal program that converts the above hex byte-codes 
into a standard MIDI file:


```cpp
#include "MidiFile.h"
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   if (argc != 3) {
      return 1;
   }
   MidiFile midifile;
   midifile.read(argv[1]);
   if (midifile.status()) {
      midifile.write(argv[2]);
   } else {
      cerr << "Problem reading MIDI file " << argv[1] << endl;
   }
}
```

The `MidiFile::read()` function will automatically identify if the
input is a binary standard MIDI file, a hex byte-code representation,
or a generalized binasc syntax file (which includes byte-codes).
The `MidiFile::status()` function can be checked after reading a MIDI
file to determine if the file was read without problems.


Code snippets
-------------


### How to extract text meta-messages from a file. ###

The `MidiMessage::isText()` function will return true if the message
is a text meta-message.  The following program merges all tracks into 
a single list and does one loop checking for text meta-messages, printing
them out when found.  The `MidiMessage::getMetaContent()` function extracts
the text string of the message from the raw MIDI file bytes.

```cpp
#include "MidiFile.h"
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   MidiFile midifile;
   if (argc == 1) midifile.read(cin);
   else midifile.read(argv[1]);
   if (!midifile.status()) {
      cerr << "Problem reading MIDI file" << endl;
      return 1;
   }

   midifile.joinTracks();
   for (int i=0; i<midifile[0].getEventCount(); i++) {
      if (midifile[0][i].isText()) {
         string content = midifile[0][i].getMetaContent();
         cout << content << endl;
      }
   }

   return 0;
}
```

Extracting lyrics would work the same by using `.isLyricText()` instead of
`.isText()`, and a track-name meta-message is identified by `.isTrackName()`.

### How to convert a Type-1 MIDI file into a Type-0 one ###

Here is a demonstration of converting a multi-track MIDI file into
a single-track MIDI file:


```cpp
#include "MidiFile.h"
#include <iostream>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   if (argc != 3) {
      cerr << "Usage: " << argv[0] << " input output" << endl;
      return 1;
   }
   MidiFile midifile;
   midifile.read(argv[1]);
   if (!midifile.status()) {
      cerr << "Problem reading MIDI file" << endl;
      return 1;
   }

   midifile.joinTracks();
   midifile.write(argv[2]);

   return 0;
}

```

The `.joinTracks()` function merges all tracks into a single track.  And if 
a `MidiFile` object has only one track when it is being written, it will be
written as a type-0 (single-track) MIDI file.



### How to check for a drum track ###

In General MIDI files, the drum track is on the 10th channel, which is
represented by the integer 9.  The following example searches through
the MIDI events in each track until it finds a note on channel 9:

```cpp
#include "MidiFile.h"
#include <iostream>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   MidiFile midifile;
   if (argc == 1) midifile.read(cin);
   else midifile.read(argv[1]);
   if (!midifile.status()) {
      cerr << "Problem reading MIDI file" << endl;
      return 1;
   }

   bool found = false;
   for (int i=0; i<midifile.getTrackCount(); i++) {
      for (int j=0; j<midifile[i].getEventCount(); j++) {
         if (midifile[i][j].isNote()) {
            int channel = midifile[i][j].getChannelNibble();
            if (channel == 9) {
               found = true;
               break;
            }
         }
      }
      if (found == true) {
         break;
      }
   }
   if (found) cout << "Has a percussion part." << endl;
   else cout << "Does not have a percussion part." << endl;

   return 0;
}
```



### Deleting percussion notes ###

For some music-analysis applications, it is useful to remove percussion
notes from a MIDI file.  Here is an example of how that can be done with
the midifile library.

```cpp
#include "MidiFile.h"
#include <iostream>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   if (argc != 3) {
      cerr << "Usage: " << argv[0] << " input output" << endl;
      return 1;
   }
   MidiFile midifile;
   midifile.read(argv[1]);
   if (!midifile.status()) {
      cerr << "Problem reading MIDI file" << endl;
      return 1;
   }

   for (int i=0; i<midifile.getTrackCount(); i++) {
      for (int j=0; j<midifile[i].getEventCount(); j++) {
         if (midifile[i][j].isNote()) {
            int channel = midifile[i][j].getChannelNibble();
            if (channel == 9) {
               midifile[i][j].clear();
            }
         }
      }
   }

   midifile.removeEmpties();  // optional

   midifile.write(argv[2]);
   return 0;
}
```

The `MidiFile::removeEmpties()` function is optional, since 
the `MidiFile::write()` function will ignore any empty `MidiMessage`s.



### List instrument numbers used in a MIDI file ###

The following example lists all of the instrument numbers
used in a MIDI file.  It does not analyze the drum track.

```cpp
#include "MidiFile.h"
#include "Options.h"
#include <set>
#include <utility>
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) {
      midifile.read(cin);
   } else {
      midifile.read(options.getArg(1));
   }
   if (!midifile.status()) {
      cerr << "Could not read MIDI file" << endl;
      return 1;
   }

   pair<int, int> trackinst;
   set<pair<int, int>> iset;
   for (int i=0; i<midifile.getTrackCount(); i++) {
      for (int j=0; j<midifile[i].getEventCount(); j++) {
         if (midifile[i][j].isTimbre()) {
            trackinst.first = i;
            trackinst.second = midifile[i][j].getP1();
            iset.insert(trackinst);
         }
      }
   }
   for (auto it : iset) {
      cout << "Track:" << it.first << "\tInstrument:" << it.second << endl;
   }
   return 0;
}
```

