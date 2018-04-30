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

Here is a schematic of how the classes are used together:

![Class organization](https://user-images.githubusercontent.com/3487289/39109564-493bca94-4682-11e8-87c4-991a931ca41b.png)

The `MidiFile` class contains a vector of tracks stored in `MidiEventList`
objects.  The `MidiEventList` is itself a vector of `MidiEvent`s, which stores
each MIDI event in the track.  `MidiEvent`s contain a timestamp and a `MidiMessage`
which is a vector of unsigned char values, storing the raw bytes of a MIDI message
(or meta-message).


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
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();

   int tracks = midifile.getTrackCount();
   cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
   if (tracks > 1) cout << "TRACKS: " << tracks << endl;
   for (int track=0; track<tracks; track++) {
      if (tracks > 1) cout << "\nTrack " << track << endl;
      cout << "Tick\tSeconds\tDur\tMessage" << endl;
      for (int event=0; event<midifile[track].size(); event++) {
         cout << dec << midifile[track][event].tick;
         cout << '\t' << dec << midifile[track][event].seconds;
         cout << '\t';
         if (midifile[track][event].isNoteOn())
            cout << midifile[track][event].getDurationInSeconds();
         cout << '\t' << hex;
         for (int i=0; i<midifile[track][event].size(); i++)
            cout << (int)midifile[track][event][i] << ' ';
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
   if (options.getArgCount() > 0) midifile.read(options.getArg(1));
   else midifile.read(cin);
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
      if (event == 0) deltatick = mev->tick;
      else deltatick = mev->tick - midifile[0][event-1].tick;
      cout << dec << mev->tick;
      cout << '\t' << deltatick;
      cout << '\t' << mev->track;
      cout << '\t' << hex;
      for (int i=0; i < mev->size(); i++)
         cout << (int)(*mev)[i] << ' ';
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
      if (options.getBoolean("hex")) midifile.writeHex(cout);
      else cout << midifile;
   } else
      midifile.write(filename);

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
   if (argc != 3) return 1;
   MidiFile midifile;
   midifile.read(argv[1]);
   if (midifile.status()) midifile.write(argv[2]);
   else cerr << "Problem reading MIDI file " << argv[1] << endl;
}
```

The `MidiFile::read()` function will automatically identify if the
input is a binary standard MIDI file, a hex byte-code representation,
or a generalized binasc syntax file (which includes byte-codes).
The `MidiFile::status()` function can be checked after reading a MIDI
file to determine if the file was read without problems.


Code snippets
-------------


### How to process multiple input files and get duration of MIDI files ###

This example uses the `MidiFile::getFileDurationInSeconds()` to calculate the
duration of a MIDI file.  Also, this example shows how to process multiple
input files when using the Options class.

```cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) {
      midifile.read(cin);
      cout << midifile.getFileDurationInSeconds() << " seconds" << endl;
   } else {
      int count = options.getArgCount();
      for (int i=0; i<count; i++) {
         string filename = options.getArg(i+1);
         if (count > 1) cout << filename << "\t";
         midifile.read(filename);
         cout << midifile.getFileDurationInSeconds() << " seconds" << endl;
      }
   }
   return 0;
}
```



### How to extract text meta-messages from a MIDI file. ###

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



### How to convert a Type-1 MIDI file into a Type-0 MIDI file ###

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



### How to check for a drum track in a MIDI file ###

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
      if (found == true) break;
   }
   if (found) cout << "Has a percussion part." << endl;
   else cout << "Does not have a percussion part." << endl;

   return 0;
}
```



### How to delete percussion notes in a MIDI file ###

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

To delete a MIDI message, clear its vector base class.  This will
leave an empty `MidiEvent` in the track, but the `MidiFile::write()`
function will ignore any empty `MidiMessage`s.  The
`MidiFile::removeEmpties()` function can be called to explicitly remove
any empty `MidiEvents` from the track.



### How to transpose pitches in a MIDI file ###

This example shows how to transpose notes in a MIDI file.  Care should be
taken to avoid transposing channel 10 in General MIDI, since this is
reserved for the drum track (and most MIDI files use the General MIDI
convention).

```cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.define("t|transpose=i:0", "Semitones to transpose by");
   options.process(argc, argv);

   MidiFile midifile;
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
   if (!midifile.status()) {
      cerr << "Could not read MIDI file" << endl;
      return 1;
   }

   int transpose = options.getInteger("transpose");
   for (int i=0; i<midifile.getTrackCount(); i++) {
      for (int j=0; j<midifile[i].getEventCount(); j++) {
         if (!midifile[i][j].isNote()) continue;
         if (midifile[i][j].getChannel() == 9) continue;
         int newkey = transpose + midifile[i][j].getP1();
         midifile[i][j].setP1(newkey);
      }
   }

   if (options.getArgCount() < 2) cout << midifile;
   else midifile.write(options.getArg(2));
   return 0;
}
```



### How to list instrument numbers used in a MIDI file ###

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
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
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
   for (auto it : iset)
      cout << "Track:" << it.first << "\tInstrument:" << it.second << endl;
   return 0;
}
```



### How to emulate temperaments in a MIDI file ###

If you want to simulate temperaments in a Standard MIDI file without
a synthesizer that specifically knows about temperaments, then this
example is useful.  Each pitch-class is placed into a separate track
and MIDI channel.  A pitch-bend message is then added to the start
of each track in each channel to control the temperament. Care is taken
to avoid MIDI channel 10, which is reserved for percussion timbres
in General MIDI.

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
   for (int i=0; i<midifile[0].getEventCount(); i++) {
      midifile[0][i].seq = 2;
      if (!midifile[0][i].isNote()) {
         midifile[0][i].track = 0;
         continue;
      }
      int pc = midifile[0][i].getP1() % 12;
      int channel = midifile[0][i].getChannelNibble();
      if (channel != 9) {
         midifile[0][i].track = pc + 1;
         if (pc >= 9) pc++;
         midifile[0][i].setChannelNibble(pc);
      } else midifile[0][i].track = 13;
   }
   midifile.splitTracks();

   double maxbend = 200.0;  // typical pitch-bend depth in cents on synthesizers
   // pythagorean tuning deviations from equal temperament in cents.
   vector<double> pythagorean = {-3.91, 9.78, 0.00, -9.78, 3.91, -5.87, 7.82,
         -1.96, -11.73, 1.96, -7.82, 5.87};

   for (int i=0; i<12; i++) {
      int maxtrack = midifile.getTrackCount();
      int track = i+1;
      if (track >= maxtrack) break;
      int channel = i;
      if (i >= 9) channel++;
      double bend = pythagorean[i] / maxbend;
      MidiEvent* me = midifile.addPitchBend(track, 0, channel, bend);
      me->seq = 1;
   }

   midifile.sortTracks();
   midifile.write(argv[2]);

   return 0;
}
```

The `MidiFile::splitTracks()` function will generate 13 or 14 tracks.  Track 0
will contain all non-note MIDI messages from the original file, while tracks
1 to 12 will contain notes of a specific pitch-class on MIDI channels 1-12,
skipping channel 10 (the General MIDI percussion channel).  Percussion notes
will be placed in track 13, but remain on channel 10.

The use of `MidiEvent::seq` being set to 1 and 2 in the program is used to
force the first notes at tick time 0 to be placed after the pitch bend
messages inserted at the same timestamp when `MidiFile::sortTracks()`
is called (events with a lower sequence number will be placed before those
with a higher number if they occur at the same time in a track when sorting
the events in the track).  The pitch-bend messages would probably be sorted
before the notes anyway, but using `seq` should guarantee they are placed
before the first notes.

Try this program on Bach's Well-Tempered Clavier, Book I, Fugue No. 4
in C-sharp minor:

```
4d 54 68 64 00 00 00 06 00 01 00 06 00 78 4d 54 72 6b 00 00 00 13 00 ff 51 03 08 8e 6c 00 ff 58 04 02 01 30 08 00 ff 2f
00 4d 54 72 6b 00 00 09 bd b2 50 90 49 40 81 70 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80 4c 40 00 90 4b
40 83 60 80 4b 40 00 90 49 40 82 68 80 49 40 00 90 4b 40 78 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4b 40 78 80 4b 40 00
90 49 40 81 70 80 49 40 00 90 47 40 81 70 80 47 40 00 90 49 40 81 70 80 49 40 00 90 4b 40 81 70 80 4b 40 82 68 90 4c 40
78 80 4c 40 00 90 4b 40 78 80 4b 40 00 90 49 40 78 80 49 40 00 90 47 40 78 80 47 40 00 90 4b 40 78 80 4b 40 00 90 50 40
82 68 80 50 40 00 90 4e 40 78 80 4e 40 00 90 50 40 78 80 50 40 00 90 51 40 78 80 51 40 00 90 53 40 84 58 80 53 40 00 90
51 40 78 80 51 40 00 90 50 40 78 80 50 40 00 90 4e 40 78 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90
4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 83 60 80 49 40 00 90 47 40 82 68 80 47 40 00 90 49 40 78 80 49 40
00 90 47 40 78 80 47 40 00 90 45 40 78 80 45 40 00 90 44 40 81 70 80 44 40 00 90 46 40 78 80 46 40 00 90 47 40 78 80 47
40 00 90 49 40 81 70 80 49 40 00 90 47 40 83 60 80 47 40 00 90 46 40 81 70 80 46 40 00 90 47 40 84 58 80 47 40 00 90 49
40 78 80 49 40 00 90 4b 40 78 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4c 40 81 70 80 4c 40 00 90 4b 40 81 70 80 4b 40 00
90 4c 40 78 80 4c 40 00 90 4b 40 78 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4e 40 78 80 4e 40 00 90 50 40 3c 80 50 40 00
90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 51 40 3c 80 51 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00
90 4c 40 3c 80 4c 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00
90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4e 40 3c 80 4e 40 00
90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00
90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00
90 4b 40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00
90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00
90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 46 40 3c 80 46 40 00 90 49 40 3c 80 49 40 00 90 47 40 82 2c 80 47 40
00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 47 40 3c 80 47 40 00 90 49 40 82 68 80 49 40 00 90 4c 40 78 80 4c
40 00 90 4b 40 82 68 80 4b 40 00 90 4e 40 78 80 4e 40 00 90 4c 40 84 58 80 4c 40 00 90 4b 40 81 70 80 4b 40 00 90 49 40
81 70 80 49 40 00 90 48 40 3c 80 48 40 00 90 46 40 3c 80 46 40 00 90 48 40 78 80 48 40 00 90 4b 40 78 80 4b 40 00 90 50
40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 51 40 3c 80 51 40 00 90 50 40 3c 80 50 40 00 90 4e
40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e
40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4e
40 3c 80 4e 40 00 90 4d 40 81 70 80 4d 40 00 90 51 40 81 70 80 51 40 00 90 50 40 84 58 80 50 40 00 90 4e 40 3c 80 4e 40
00 90 4c 40 3c 80 4c 40 00 90 4a 40 81 70 80 4a 40 78 90 49 40 78 80 49 40 00 90 4e 40 78 80 4e 40 00 90 4e 40 78 80 4e
40 00 90 4e 40 78 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 4c 40 82 68 80 4c 40 00 90 4a 40 3c 80
4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40 83 60 80 4a 40 00 90 49 40 81 70 80 49 40 00 90 4e 40 81 70 80 4e 40 00 90 4c
40 81 70 80 4c 40 00 90 4c 40 81 34 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00
90 49 40 3c 80 49 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40 3c 80 4a 40 00
90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 4a 40 3c 80 4a 40 00
90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80 4c 40 00 90 4b 40 84 58 80 4b 40 00 90 44 40 78
80 44 40 00 90 49 40 78 80 49 40 00 90 49 40 78 80 49 40 00 90 49 40 78 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c
80 45 40 00 90 47 40 83 60 80 47 40 00 90 45 40 81 70 80 45 40 00 90 44 40 81 70 80 44 40 81 70 90 4b 40 83 60 80 4b 40
00 90 4a 40 81 70 80 4a 40 00 90 4e 40 81 70 80 4e 40 00 90 4d 40 81 70 80 4d 40 00 90 4c 40 81 70 80 4c 40 00 90 4b 40
3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40
3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 49 40
3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40
3c 80 49 40 00 90 48 40 81 70 80 48 40 00 90 47 40 81 70 80 47 40 00 90 46 40 81 70 80 46 40 00 90 45 40 81 70 80 45 40
00 90 44 40 81 70 80 44 40 89 30 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80 4c 40 00 90 4b
40 85 50 80 4b 40 00 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 78 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40
00 90 4c 40 78 80 4c 40 00 90 4e 40 78 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40
00 90 51 40 3c 80 51 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 50 40 3c 80 50 40
00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40
00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40
00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40
00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40 3c 80 4a 40 00 90 4c 40 3c 80 4c 40
00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40
00 90 47 40 3c 80 47 40 00 90 49 40 3c 80 49 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40
00 90 45 40 3c 80 45 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c 80 45 40 00 90 47 40 3c 80 47 40
00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c 80 45 40 00 90 44 40 3c 80 44 40 00 90 47 40 3c 80 47 40
00 90 45 40 81 70 80 45 40 81 70 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80 4c 40 00 90 4b
40 84 58 80 4b 40 00 90 49 40 78 80 49 40 00 90 50 40 78 80 50 40 00 90 50 40 78 80 50 40 00 90 50 40 78 80 50 40 00 90
4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 81 70 80 4e 40 00 90 4c 40 83 60 80 4c 40 00 90 4b 40 81 70 80 4b
40 00 90 4f 40 81 70 80 4f 40 00 90 4e 40 81 70 80 4e 40 00 90 42 40 81 70 80 42 40 00 90 41 40 81 70 80 41 40 00 90 45
40 81 70 80 45 40 00 90 44 40 78 80 44 40 83 60 90 48 40 78 80 48 40 00 90 4e 40 78 80 4e 40 00 90 4e 40 78 80 4e 40 00
90 4e 40 78 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4b 40 3c 80 4b 40 00
90 49 40 3c 80 49 40 00 90 4b 40 78 80 4b 40 00 90 48 40 78 80 48 40 00 90 49 40 85 50 80 49 40 00 90 48 40 81 70 80 48
40 00 90 47 40 81 70 80 47 40 00 90 46 40 81 70 80 46 40 00 90 45 40 81 70 80 45 40 00 90 44 40 83 60 80 44 40 00 90 46
40 81 70 80 46 40 00 90 48 40 81 70 80 48 40 00 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 83 60 80
4c 40 00 90 4b 40 78 80 4b 40 00 90 49 40 78 80 49 40 00 90 48 40 81 70 80 48 40 00 90 49 40 83 60 80 49 40 00 90 48 40
81 70 80 48 40 00 90 49 40 8f 00 80 49 40 77 90 00 00 00 ff 2f 00 4d 54 72 6b 00 00 09 40 ab 10 91 44 40 81 70 81 44 40
00 91 41 40 81 70 81 41 40 00 91 45 40 81 70 81 45 40 00 91 44 40 81 70 81 44 40 00 91 42 40 83 60 81 42 40 00 91 49 40
83 60 81 49 40 00 91 47 40 83 60 81 47 40 00 91 45 40 81 70 81 45 40 00 91 44 40 81 70 81 44 40 00 91 45 40 82 68 81 45
40 00 91 44 40 78 81 44 40 00 91 42 40 78 81 42 40 00 91 40 40 78 81 40 40 00 91 3f 40 81 70 81 3f 40 00 91 44 40 81 70
81 44 40 00 91 44 40 81 70 81 44 40 00 91 43 40 81 70 81 43 40 00 91 44 40 81 70 81 44 40 00 91 3b 40 81 70 81 3b 40 00
91 3d 40 82 68 81 3d 40 00 91 3d 40 78 81 3d 40 00 91 3f 40 78 81 3f 40 00 91 3d 40 78 81 3d 40 00 91 3f 40 78 81 3f 40
00 91 41 40 78 81 41 40 00 91 42 40 81 70 81 42 40 00 91 45 40 81 70 81 45 40 00 91 44 40 81 70 81 44 40 3c 91 47 40 3c
81 47 40 00 91 45 40 3c 81 45 40 00 91 44 40 3c 81 44 40 00 91 42 40 78 81 42 40 00 91 45 40 78 81 45 40 00 91 44 40 78
81 44 40 00 91 42 40 78 81 42 40 00 91 41 40 81 70 81 41 40 00 91 42 40 82 68 81 42 40 00 91 40 40 84 58 81 40 40 00 91
3f 40 81 70 81 3f 40 00 91 40 40 84 58 81 40 40 00 91 3f 40 78 81 3f 40 00 91 40 40 78 81 40 40 00 91 42 40 3c 81 42 40
00 91 44 40 3c 81 44 40 00 91 42 40 81 70 81 42 40 00 91 47 40 82 68 81 47 40 00 91 49 40 78 81 49 40 00 91 47 40 78 81
47 40 00 91 45 40 78 81 45 40 00 91 44 40 81 70 81 44 40 a1 60 91 49 40 81 70 81 49 40 00 91 48 40 81 70 81 48 40 00 91
4c 40 81 70 81 4c 40 00 91 4b 40 85 50 81 4b 40 00 91 49 40 83 60 81 49 40 00 91 48 40 78 81 48 40 00 91 47 40 81 34 81
47 40 00 91 47 40 3c 81 47 40 00 91 49 40 3c 81 49 40 00 91 4a 40 3c 81 4a 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81
47 40 00 91 45 40 3c 81 45 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81 47 40 00 91 45 40 3c 81 45 40 00 91 47 40 3c 81
47 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81 47 40 00 91 45 40 3c 81 45 40 00 91 44 40 3c 81 44 40 00 91 47 40 3c 81
47 40 00 91 45 40 82 2c 81 45 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81 47 40 00 91 45 40 3c 81 45 40 00 91 44 40 81
70 81 44 40 87 40 91 45 40 81 70 81 45 40 00 91 44 40 81 70 81 44 40 00 91 49 40 81 70 81 49 40 00 91 47 40 84 58 81 47
40 00 91 40 40 78 81 40 40 00 91 45 40 78 81 45 40 00 91 45 40 78 81 45 40 00 91 45 40 78 81 45 40 00 91 44 40 3c 81 44
40 00 91 42 40 3c 81 42 40 00 91 44 40 82 68 81 44 40 00 91 3d 40 3c 81 3d 40 00 91 3f 40 3c 81 3f 40 00 91 40 40 78 81
40 40 00 91 42 40 78 81 42 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 44 40 3c 81 44 40 00 91 45 40 3c 81
45 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81
42 40 00 91 40 40 3c 81 40 40 00 91 42 40 3c 81 42 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81
40 40 00 91 3f 40 3c 81 3f 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81 3f 40 00 91 40 40 3c 81
40 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81 3f 40 00 91 3d 40 3c 81 3d 40 00 91 40 40 3c 81
40 40 00 91 3f 40 3c 81 3f 40 00 91 3d 40 3c 81 3d 40 00 91 3f 40 3c 81 3f 40 00 91 40 40 3c 81 40 40 00 91 3e 40 3c 81
3e 40 00 91 3d 40 3c 81 3d 40 00 91 3b 40 3c 81 3b 40 00 91 3e 40 3c 81 3e 40 00 91 3d 40 3c 81 3d 40 00 91 3b 40 3c 81
3b 40 00 91 3d 40 3c 81 3d 40 00 91 3e 40 3c 81 3e 40 00 91 3d 40 3c 81 3d 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81
3f 40 00 91 3d 40 3c 81 3d 40 00 91 48 40 81 70 81 48 40 00 91 3d 40 81 70 81 3d 40 00 91 3f 40 78 81 3f 40 00 91 3f 40
78 81 3f 40 00 91 44 40 3c 81 44 40 00 91 46 40 3c 81 46 40 00 91 47 40 81 70 81 47 40 00 91 46 40 78 81 46 40 00 91 4b
40 78 81 4b 40 00 91 4b 40 78 81 4b 40 00 91 4b 40 78 81 4b 40 00 91 49 40 3c 81 49 40 00 91 48 40 3c 81 48 40 00 91 49
40 83 60 81 49 40 00 91 47 40 81 70 81 47 40 00 91 46 40 81 70 81 46 40 00 91 45 40 82 68 81 45 40 00 91 3f 40 78 81 3f
40 00 91 44 40 78 81 44 40 00 91 44 40 81 70 81 44 40 00 91 42 40 3c 81 42 40 00 91 41 40 3c 81 41 40 00 91 42 40 83 60
81 42 40 00 91 40 40 81 70 81 40 40 00 91 3f 40 81 70 81 3f 40 00 91 3d 40 81 70 81 3d 40 00 91 3f 40 83 60 81 3f 40 84
58 91 44 40 78 81 44 40 00 91 49 40 78 81 49 40 00 91 49 40 78 81 49 40 00 91 49 40 78 81 49 40 00 91 48 40 3c 81 48 40
00 91 46 40 3c 81 46 40 00 91 48 40 3c 81 48 40 00 91 49 40 3c 81 49 40 00 91 4b 40 3c 81 4b 40 00 91 48 40 3c 81 48 40
00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 44 40 3c 81 44 40 00 91 45 40 3c 81 45 40 00 91 44 40 3c 81 44 40
00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40
00 91 42 40 3c 81 42 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81 3f 40
00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 44 40 3c 81 44 40 00 91 45 40 3c 81 45 40 00 91 47 40 3c 81 47 40
00 91 49 40 3c 81 49 40 00 91 4b 40 3c 81 4b 40 00 91 48 40 3c 81 48 40 00 91 49 40 3c 81 49 40 00 91 4b 40 78 81 4b 40
8b 20 91 42 40 78 81 42 40 00 91 47 40 78 81 47 40 00 91 47 40 78 81 47 40 00 91 47 40 78 81 47 40 00 91 45 40 3c 81 45
40 00 91 44 40 3c 81 44 40 00 91 45 40 81 70 81 45 40 00 91 44 40 84 1c 81 44 40 00 91 44 40 3c 81 44 40 00 91 42 40 3c
81 42 40 00 91 41 40 3c 81 41 40 00 91 42 40 81 70 81 42 40 00 91 44 40 82 2c 81 44 40 00 91 44 40 3c 81 44 40 00 91 42
40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81 3f 40 00 91 45 40 3c 81 45 40 00 91 44 40 3c 81 44 40 00 91 42
40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 44 40 3c 81 44 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81 47 40 00 91 45
40 3c 81 45 40 00 91 44 40 3c 81 44 40 00 91 45 40 3c 81 45 40 00 91 42 40 3c 81 42 40 00 91 44 40 82 68 81 44 40 00 91
49 40 3c 81 49 40 00 91 48 40 3c 81 48 40 00 91 49 40 82 68 81 49 40 00 91 46 40 78 81 46 40 00 91 4b 40 78 81 4b 40 00
91 4b 40 78 81 4b 40 00 91 4b 40 78 81 4b 40 00 91 49 40 3c 81 49 40 00 91 47 40 3c 81 47 40 00 91 49 40 81 70 81 49 40
00 91 47 40 81 70 81 47 40 00 91 47 40 81 70 81 47 40 00 91 46 40 81 70 81 46 40 00 91 4a 40 81 70 81 4a 40 00 91 49 40
83 60 81 49 40 78 91 44 40 78 81 44 40 00 91 49 40 78 81 49 40 00 91 49 40 78 81 49 40 00 91 49 40 78 81 49 40 00 91 48
40 3c 81 48 40 00 91 46 40 3c 81 46 40 00 91 48 40 3c 81 48 40 00 91 49 40 3c 81 49 40 00 91 4b 40 81 70 81 4b 40 00 91
49 40 3c 81 49 40 00 91 48 40 3c 81 48 40 00 91 49 40 81 70 81 49 40 00 91 42 40 81 70 81 42 40 00 91 44 40 83 60 81 44
40 00 91 42 40 82 68 81 42 40 82 68 91 44 40 78 81 44 40 00 91 42 40 3c 81 42 40 00 91 40 40 3c 81 40 40 00 91 42 40 78
81 42 40 00 91 42 40 78 81 42 40 00 91 42 40 78 81 42 40 00 91 40 40 3c 81 40 40 00 91 3f 40 3c 81 3f 40 00 91 40 40 82
68 81 40 40 00 91 42 40 3c 81 42 40 00 91 44 40 3c 81 44 40 00 91 45 40 78 81 45 40 00 91 44 40 81 70 81 44 40 00 91 40
40 78 81 40 40 00 91 45 40 78 81 45 40 00 91 45 40 78 81 45 40 00 91 45 40 78 81 45 40 00 91 44 40 3c 81 44 40 00 91 42
40 3c 81 42 40 00 91 44 40 78 81 44 40 00 91 46 40 3c 81 46 40 00 91 48 40 3c 81 48 40 00 91 49 40 81 70 81 49 40 00 91
46 40 81 70 81 46 40 00 91 44 40 81 70 81 44 40 00 91 44 40 85 50 81 44 40 00 91 44 40 81 70 81 44 40 00 91 42 40 81 70
81 42 40 00 91 41 40 81 70 81 41 40 00 91 45 40 81 70 81 45 40 00 91 44 40 87 40 81 44 40 77 90 00 00 00 ff 2f 00 4d 54
72 6b 00 00 08 ba 98 30 92 3d 40 81 70 82 3d 40 00 92 3c 40 81 70 82 3c 40 00 92 40 40 81 70 82 40 40 00 92 3f 40 83 60
82 3f 40 00 92 3d 40 81 70 82 3d 40 00 92 42 40 82 68 82 42 40 00 92 40 40 78 82 40 40 00 92 3f 40 78 82 3f 40 00 92 3d
40 78 82 3d 40 00 92 3d 40 81 70 82 3d 40 00 92 3b 40 81 70 82 3b 40 00 92 3d 40 81 70 82 3d 40 00 92 42 40 82 68 82 42
40 00 92 40 40 78 82 40 40 00 92 3f 40 78 82 3f 40 00 92 3d 40 78 82 3d 40 00 92 3f 40 81 70 82 3f 40 00 92 44 40 81 70
82 44 40 78 92 45 40 78 82 45 40 00 92 44 40 78 82 44 40 00 92 42 40 78 82 42 40 00 92 41 40 78 82 41 40 00 92 3d 40 78
82 3d 40 00 92 42 40 81 70 82 42 40 00 92 40 40 85 50 82 40 40 00 92 44 40 81 70 82 44 40 00 92 46 40 81 70 82 46 40 92
60 92 3d 40 81 70 82 3d 40 00 92 3c 40 81 70 82 3c 40 00 92 40 40 81 70 82 40 40 00 92 3f 40 83 60 82 3f 40 00 92 3d 40
87 40 82 3d 40 00 92 36 40 78 82 36 40 00 92 38 40 78 82 38 40 00 92 3a 40 78 82 3a 40 00 92 3b 40 78 82 3b 40 00 92 3d
40 83 60 82 3d 40 81 70 92 40 40 81 70 82 40 40 00 92 3f 40 81 70 82 3f 40 00 92 44 40 81 70 82 44 40 00 92 42 40 83 60
82 42 40 00 92 40 40 81 70 82 40 40 8b 20 92 44 40 81 70 82 44 40 00 92 43 40 81 70 82 43 40 00 92 47 40 81 70 82 47 40
00 92 46 40 84 58 82 46 40 00 92 3f 40 78 82 3f 40 00 92 44 40 82 68 82 44 40 00 92 42 40 3c 82 42 40 00 92 40 40 3c 82
40 40 00 92 42 40 82 68 82 42 40 00 92 40 40 3c 82 40 40 00 92 42 40 3c 82 42 40 00 92 44 40 89 30 82 44 40 8c 18 92 3d
40 78 82 3d 40 00 92 42 40 78 82 42 40 00 92 42 40 78 82 42 40 00 92 42 40 78 82 42 40 00 92 41 40 3c 82 41 40 00 92 3f
40 3c 82 3f 40 00 92 41 40 81 70 82 41 40 00 92 42 40 78 82 42 40 00 92 36 40 3c 82 36 40 00 92 38 40 3c 82 38 40 00 92
39 40 78 82 39 40 00 92 3b 40 78 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92
3e 40 3c 82 3e 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 3d 40 3c 82 3d 40 00 92
3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 3b 40 3c 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92
39 40 3c 82 39 40 00 92 38 40 3c 82 38 40 00 92 3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 38 40 3c 82 38 40 00 92
39 40 3c 82 39 40 00 92 3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 3b 40 3c 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92
3e 40 3c 82 3e 40 00 92 40 40 3c 82 40 40 00 92 3e 40 3c 82 3e 40 00 92 40 40 3c 82 40 40 00 92 42 40 3c 82 42 40 00 92
40 40 3c 82 40 40 00 92 3e 40 3c 82 3e 40 00 92 3d 40 3c 82 3d 40 00 92 40 40 3c 82 40 40 00 92 3e 40 3c 82 3e 40 00 92
3d 40 3c 82 3d 40 00 92 3e 40 3c 82 3e 40 00 92 40 40 3c 82 40 40 00 92 3e 40 3c 82 3e 40 00 92 3d 40 3c 82 3d 40 00 92
3b 40 3c 82 3b 40 00 92 3e 40 3c 82 3e 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92
3e 40 3c 82 3e 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 3d 40 3c 82 3d 40 00 92
3b 40 83 60 82 3b 40 00 92 39 40 3c 82 39 40 00 92 38 40 3c 82 38 40 00 92 36 40 3c 82 36 40 00 92 34 40 3c 82 34 40 00
92 33 40 3c 82 33 40 00 92 31 40 3c 82 31 40 00 92 30 40 3c 82 30 40 00 92 31 40 3c 82 31 40 00 92 33 40 81 70 82 33 40
8e 08 92 31 40 78 82 31 40 00 92 36 40 78 82 36 40 00 92 36 40 78 82 36 40 00 92 36 40 78 82 36 40 00 92 34 40 3c 82 34
40 00 92 33 40 3c 82 33 40 00 92 34 40 81 70 82 34 40 00 92 37 40 81 70 82 37 40 00 92 38 40 81 70 82 38 40 00 92 35 40
78 82 35 40 87 40 92 33 40 78 82 33 40 00 92 38 40 78 82 38 40 00 92 38 40 78 82 38 40 00 92 38 40 78 82 38 40 00 92 36
40 3c 82 36 40 00 92 34 40 3c 82 34 40 00 92 36 40 82 2c 82 36 40 00 92 39 40 3c 82 39 40 00 92 38 40 3c 82 38 40 00 92
36 40 3c 82 36 40 00 92 35 40 3c 82 35 40 00 92 33 40 3c 82 33 40 00 92 35 40 3c 82 35 40 00 92 31 40 3c 82 31 40 00 92
36 40 3c 82 36 40 00 92 35 40 3c 82 35 40 00 92 36 40 3c 82 36 40 00 92 38 40 3c 82 38 40 00 92 39 40 3c 82 39 40 00 92
38 40 3c 82 38 40 00 92 39 40 3c 82 39 40 00 92 3b 40 3c 82 3b 40 00 92 3d 40 3c 82 3d 40 00 92 3c 40 3c 82 3c 40 00 92
3d 40 3c 82 3d 40 00 92 3f 40 3c 82 3f 40 00 92 40 40 3c 82 40 40 00 92 3f 40 3c 82 3f 40 00 92 40 40 3c 82 40 40 00 92
42 40 3c 82 42 40 00 92 44 40 3c 82 44 40 00 92 42 40 3c 82 42 40 00 92 44 40 3c 82 44 40 00 92 45 40 3c 82 45 40 00 92
44 40 3c 82 44 40 00 92 42 40 3c 82 42 40 00 92 40 40 3c 82 40 40 00 92 44 40 3c 82 44 40 00 92 42 40 3c 82 42 40 00 92
40 40 3c 82 40 40 00 92 42 40 3c 82 42 40 00 92 44 40 3c 82 44 40 00 92 42 40 3c 82 42 40 00 92 40 40 3c 82 40 40 00 92
3f 40 3c 82 3f 40 00 92 42 40 3c 82 42 40 00 92 40 40 3c 82 40 40 00 92 3f 40 3c 82 3f 40 00 92 40 40 3c 82 40 40 00 92
42 40 3c 82 42 40 00 92 40 40 3c 82 40 40 00 92 3f 40 3c 82 3f 40 00 92 3d 40 3c 82 3d 40 00 92 40 40 3c 82 40 40 00 92
3f 40 81 70 82 3f 40 91 68 92 38 40 78 82 38 40 00 92 3d 40 78 82 3d 40 00 92 3d 40 78 82 3d 40 00 92 3d 40 78 82 3d 40
00 92 3c 40 3c 82 3c 40 00 92 3a 40 3c 82 3a 40 00 92 3c 40 81 70 82 3c 40 00 92 3d 40 83 60 82 3d 40 00 92 3b 40 82 68
82 3b 40 00 92 42 40 78 82 42 40 00 92 41 40 81 70 82 41 40 00 92 42 40 81 70 82 42 40 00 92 3b 40 81 70 82 3b 40 00 92
3d 40 84 1c 82 3d 40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 3c 82 3b 40 00 92 39 40 3c 82 39 40 00 92 38 40 83 60 82 38 40
93 58 92 3b 40 78 82 3b 40 00 92 40 40 78 82 40 40 00 92 40 40 78 82 40 40 00 92 40 40 78 82 40 40 00 92 3e 40 3c 82 3e
40 00 92 3d 40 3c 82 3d 40 00 92 3b 40 81 70 82 3b 40 78 92 3d 40 78 82 3d 40 00 92 42 40 78 82 42 40 00 92 42 40 78 82
42 40 00 92 42 40 78 82 42 40 00 92 40 40 3c 82 40 40 00 92 3f 40 3c 82 3f 40 00 92 40 40 81 70 82 40 40 00 92 42 40 83
24 82 42 40 00 92 44 40 3c 82 44 40 00 92 45 40 78 82 45 40 00 92 44 40 3c 82 44 40 00 92 42 40 3c 82 42 40 00 92 44 40
78 82 44 40 00 92 42 40 3c 82 42 40 00 92 40 40 3c 82 40 40 00 92 3f 40 81 70 82 3f 40 00 92 40 40 83 60 82 40 40 00 92
3f 40 78 82 3f 40 00 92 3d 40 78 82 3d 40 00 92 3f 40 78 82 3f 40 00 92 41 40 3c 82 41 40 00 92 42 40 3c 82 42 40 00 92
44 40 78 82 44 40 00 92 44 40 78 82 44 40 78 92 3d 40 78 82 3d 40 00 92 3f 40 78 82 3f 40 00 92 3d 40 78 82 3d 40 00 92
3c 40 81 70 82 3c 40 00 92 3d 40 83 60 82 3d 40 00 92 3f 40 81 70 82 3f 40 00 92 3d 40 78 82 3d 40 00 92 3d 40 78 82 3d
40 00 92 42 40 78 82 42 40 00 92 42 40 78 82 42 40 00 92 42 40 78 82 42 40 00 92 40 40 3c 82 40 40 00 92 3f 40 3c 82 3f
40 00 92 40 40 78 82 40 40 00 92 42 40 3c 82 42 40 00 92 44 40 3c 82 44 40 00 92 46 40 81 70 82 46 40 00 92 3f 40 83 60
82 3f 40 00 92 40 40 81 70 82 40 40 00 92 3f 40 82 68 82 3f 40 00 92 40 40 3c 82 40 40 00 92 42 40 3c 82 42 40 00 92 41
40 78 82 41 40 00 92 3d 40 83 60 82 3d 40 00 92 3d 40 78 82 3d 40 00 92 42 40 78 82 42 40 00 92 42 40 78 82 42 40 00 92
42 40 81 70 82 42 40 00 92 41 40 78 82 41 40 00 92 3f 40 78 82 3f 40 00 92 41 40 83 60 82 41 40 77 90 00 00 00 ff 2f 00
4d 54 72 6b 00 00 07 ac 8b 20 93 38 40 83 60 83 38 40 00 93 37 40 81 70 83 37 40 00 93 3b 40 81 70 83 3b 40 00 93 3a 40
83 60 83 3a 40 00 93 38 40 81 70 83 38 40 00 93 39 40 82 68 83 39 40 00 93 38 40 3c 83 38 40 00 93 36 40 3c 83 36 40 00
93 38 40 78 83 38 40 00 93 3d 40 78 83 3d 40 00 93 36 40 78 83 36 40 00 93 38 40 3c 83 38 40 00 93 39 40 3c 83 39 40 00
93 3b 40 82 68 83 3b 40 00 93 39 40 78 83 39 40 00 93 38 40 78 83 38 40 00 93 36 40 78 83 36 40 00 93 38 40 81 70 83 38
40 00 93 36 40 78 83 36 40 00 93 34 40 78 83 34 40 00 93 33 40 82 68 83 33 40 00 93 32 40 78 83 32 40 00 93 31 40 87 40
83 31 40 90 70 93 38 40 81 70 83 38 40 00 93 37 40 81 70 83 37 40 00 93 3b 40 81 70 83 3b 40 00 93 3a 40 83 60 83 3a 40
00 93 38 40 78 83 38 40 00 93 39 40 78 83 39 40 00 93 38 40 78 83 38 40 00 93 36 40 78 83 36 40 00 93 35 40 81 70 83 35
40 00 93 39 40 81 70 83 39 40 00 93 38 40 83 60 83 38 40 00 93 36 40 84 58 83 36 40 00 93 38 40 3c 83 38 40 00 93 36 40
3c 83 36 40 00 93 34 40 78 83 34 40 00 93 36 40 3c 83 36 40 00 93 38 40 3c 83 38 40 00 93 39 40 78 83 39 40 00 93 36 40
78 83 36 40 00 93 38 40 81 70 83 38 40 00 93 31 40 89 30 83 31 40 00 93 36 40 81 70 83 36 40 00 93 34 40 78 83 34 40 00
93 33 40 78 83 33 40 00 93 34 40 78 83 34 40 00 93 36 40 78 83 36 40 00 93 38 40 78 83 38 40 00 93 36 40 78 83 36 40 00
93 38 40 78 83 38 40 00 93 39 40 78 83 39 40 00 93 3b 40 89 30 83 3b 40 00 93 3d 40 81 70 83 3d 40 00 93 3c 40 81 70 83
3c 40 00 93 40 40 81 70 83 40 40 00 93 3f 40 83 60 83 3f 40 00 93 3d 40 84 58 83 3d 40 00 93 3b 40 78 83 3b 40 00 93 3a
40 78 83 3a 40 00 93 38 40 81 70 83 38 40 00 93 37 40 3c 83 37 40 00 93 35 40 3c 83 35 40 00 93 37 40 81 70 83 37 40 00
93 38 40 3c 83 38 40 00 93 3a 40 3c 83 3a 40 00 93 38 40 3c 83 38 40 00 93 37 40 3c 83 37 40 00 93 38 40 3c 83 38 40 00
93 3a 40 3c 83 3a 40 00 93 3b 40 3c 83 3b 40 00 93 38 40 3c 83 38 40 00 93 39 40 3c 83 39 40 00 93 3b 40 3c 83 3b 40 00
93 39 40 3c 83 39 40 00 93 38 40 3c 83 38 40 00 93 3a 40 3c 83 3a 40 00 93 3b 40 3c 83 3b 40 00 93 3d 40 3c 83 3d 40 00
93 3a 40 3c 83 3a 40 00 93 3b 40 3c 83 3b 40 00 93 3d 40 3c 83 3d 40 00 93 3b 40 3c 83 3b 40 00 93 3a 40 3c 83 3a 40 00
93 3c 40 3c 83 3c 40 00 93 3d 40 3c 83 3d 40 00 93 3f 40 3c 83 3f 40 00 93 3c 40 3c 83 3c 40 00 93 3d 40 83 60 83 3d 40
98 30 93 36 40 81 70 83 36 40 00 93 35 40 81 70 83 35 40 00 93 39 40 81 70 83 39 40 00 93 38 40 83 60 83 38 40 00 93 36
40 83 60 83 36 40 93 58 93 38 40 78 83 38 40 00 93 3d 40 78 83 3d 40 00 93 3d 40 78 83 3d 40 00 93 3d 40 78 83 3d 40 00
93 3c 40 3c 83 3c 40 00 93 3a 40 3c 83 3a 40 00 93 3c 40 81 70 83 3c 40 00 93 3d 40 81 70 83 3d 40 00 93 39 40 81 70 83
39 40 00 93 36 40 81 70 83 36 40 00 93 38 40 81 70 83 38 40 00 93 35 40 81 70 83 35 40 00 93 36 40 81 70 83 36 40 00 93
38 40 83 60 83 38 40 00 93 33 40 83 60 83 33 40 9b 18 93 2c 40 78 83 2c 40 00 93 31 40 78 83 31 40 00 93 31 40 78 83 31
40 00 93 31 40 78 83 31 40 00 93 30 40 3c 83 30 40 00 93 2e 40 3c 83 2e 40 00 93 30 40 81 70 83 30 40 00 93 31 40 81 70
83 31 40 90 70 93 31 40 83 60 83 31 40 00 93 30 40 81 70 83 30 40 00 93 34 40 81 70 83 34 40 00 93 33 40 83 60 83 33 40
00 93 31 40 78 83 31 40 00 93 34 40 78 83 34 40 00 93 39 40 78 83 39 40 00 93 39 40 78 83 39 40 00 93 39 40 78 83 39 40
00 93 38 40 3c 83 38 40 00 93 36 40 3c 83 36 40 00 93 38 40 83 60 83 38 40 00 93 36 40 81 70 83 36 40 00 93 38 40 83 60
83 38 40 00 93 39 40 81 70 83 39 40 00 93 36 40 82 2c 83 36 40 00 93 36 40 3c 83 36 40 00 93 34 40 3c 83 34 40 00 93 33
40 3c 83 33 40 00 93 34 40 78 83 34 40 00 93 31 40 78 83 31 40 00 93 38 40 83 60 83 38 40 00 93 36 40 81 70 83 36 40 00
93 33 40 81 70 83 33 40 00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33 40 00 93 34 40 3c 83 34 40 00 93 36 40 3c 83 36 40
00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33 40 00 93 31 40 3c 83 31 40 00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33 40
00 93 31 40 3c 83 31 40 00 93 33 40 3c 83 33 40 00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33 40 00 93 31 40 3c 83 31 40
00 93 30 40 3c 83 30 40 00 93 33 40 3c 83 33 40 00 93 31 40 78 83 31 40 00 93 34 40 78 83 34 40 00 93 39 40 78 83 39 40
00 93 39 40 78 83 39 40 00 93 39 40 78 83 39 40 00 93 37 40 3c 83 37 40 00 93 36 40 3c 83 36 40 00 93 34 40 81 70 83 34
40 78 93 36 40 78 83 36 40 00 93 3b 40 78 83 3b 40 00 93 3b 40 78 83 3b 40 00 93 3b 40 78 83 3b 40 00 93 39 40 3c 83 39
40 00 93 38 40 3c 83 38 40 00 93 39 40 3c 83 39 40 00 93 38 40 3c 83 38 40 00 93 36 40 3c 83 36 40 00 93 34 40 3c 83 34
40 00 93 33 40 81 70 83 33 40 78 93 38 40 78 83 38 40 00 93 3f 40 78 83 3f 40 00 93 3f 40 78 83 3f 40 00 93 3f 40 78 83
3f 40 00 93 3d 40 3c 83 3d 40 00 93 3c 40 3c 83 3c 40 00 93 3d 40 83 60 83 3d 40 00 93 3c 40 81 70 83 3c 40 00 93 40 40
82 68 83 40 40 00 93 31 40 78 83 31 40 00 93 36 40 78 83 36 40 00 93 36 40 78 83 36 40 00 93 36 40 78 83 36 40 00 93 35
40 3c 83 35 40 00 93 33 40 3c 83 33 40 00 93 31 40 83 60 83 31 40 00 93 33 40 82 68 83 33 40 00 93 33 40 78 83 33 40 00
93 38 40 78 83 38 40 00 93 38 40 78 83 38 40 00 93 38 40 78 83 38 40 00 93 36 40 3c 83 36 40 00 93 34 40 3c 83 34 40 00
93 36 40 78 83 36 40 00 93 36 40 78 83 36 40 00 93 36 40 78 83 36 40 00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33 40 00
93 34 40 78 83 34 40 00 93 33 40 3c 83 33 40 00 93 31 40 3c 83 31 40 00 93 33 40 78 83 33 40 00 93 2c 40 78 83 2c 40 00
93 31 40 78 83 31 40 00 93 31 40 78 83 31 40 00 93 31 40 78 83 31 40 00 93 2f 40 3c 83 2f 40 00 93 2e 40 3c 83 2e 40 00
93 33 40 82 2c 83 33 40 00 93 34 40 3c 83 34 40 00 93 36 40 81 34 83 36 40 00 93 34 40 3c 83 34 40 00 93 33 40 3c 83 33
40 00 93 31 40 3c 83 31 40 00 93 38 40 83 60 83 38 40 00 93 39 40 82 68 83 39 40 00 93 38 40 3c 83 38 40 00 93 39 40 3c
83 39 40 00 93 3b 40 78 83 3b 40 00 93 39 40 78 83 39 40 00 93 38 40 78 83 38 40 00 93 36 40 78 83 36 40 00 93 3d 40 78
83 3d 40 00 93 3b 40 3c 83 3b 40 00 93 39 40 3c 83 39 40 00 93 38 40 78 83 38 40 00 93 36 40 78 83 36 40 00 93 38 40 83
60 83 38 40 77 90 00 00 00 ff 2f 00 4d 54 72 6b 00 00 07 cd 00 90 31 40 83 60 80 31 40 00 90 30 40 81 70 80 30 40 00 90
34 40 81 70 80 34 40 00 90 33 40 83 60 80 33 40 00 90 31 40 78 80 31 40 00 90 33 40 78 80 33 40 00 90 34 40 82 68 80 34
40 00 90 33 40 3c 80 33 40 00 90 31 40 3c 80 31 40 00 90 33 40 78 80 33 40 00 90 38 40 78 80 38 40 00 90 31 40 78 80 31
40 00 90 33 40 3c 80 33 40 00 90 34 40 3c 80 34 40 00 90 36 40 82 68 80 36 40 00 90 34 40 78 80 34 40 00 90 33 40 78 80
33 40 00 90 31 40 78 80 31 40 00 90 33 40 81 70 80 33 40 00 90 31 40 82 68 80 31 40 00 90 2f 40 78 80 2f 40 00 90 2d 40
78 80 2d 40 00 90 2c 40 78 80 2c 40 00 90 2d 40 81 70 80 2d 40 00 90 2e 40 81 70 80 2e 40 00 90 30 40 81 70 80 30 40 00
90 31 40 81 70 80 31 40 00 90 2c 40 78 80 2c 40 00 90 2d 40 78 80 2d 40 00 90 2f 40 82 68 80 2f 40 00 90 2d 40 78 80 2d
40 00 90 2c 40 78 80 2c 40 00 90 2a 40 78 80 2a 40 00 90 31 40 81 70 80 31 40 00 90 2d 40 82 68 80 2d 40 00 90 2c 40 78
80 2c 40 00 90 2a 40 78 80 2a 40 00 90 28 40 78 80 28 40 00 90 2a 40 81 70 80 2a 40 00 90 2c 40 81 70 80 2c 40 00 90 2d
40 78 80 2d 40 00 90 2c 40 78 80 2c 40 00 90 2d 40 78 80 2d 40 00 90 2f 40 78 80 2f 40 00 90 31 40 78 80 31 40 00 90 2f
40 78 80 2f 40 00 90 31 40 78 80 31 40 00 90 33 40 78 80 33 40 00 90 34 40 81 70 80 34 40 00 90 31 40 82 68 80 31 40 00
90 2f 40 78 80 2f 40 00 90 2e 40 78 80 2e 40 00 90 2c 40 78 80 2c 40 00 90 31 40 81 70 80 31 40 00 90 33 40 81 70 80 33
40 00 90 2c 40 81 70 80 2c 40 9a 20 90 2f 40 81 70 80 2f 40 00 90 2e 40 81 70 80 2e 40 00 90 33 40 81 70 80 33 40 00 90
31 40 83 60 80 31 40 00 90 2f 40 84 58 80 2f 40 00 90 38 40 78 80 38 40 00 90 36 40 78 80 36 40 00 90 34 40 78 80 34 40
00 90 3b 40 81 70 80 3b 40 00 90 2f 40 81 70 80 2f 40 00 90 34 40 81 70 80 34 40 00 90 39 40 81 70 80 39 40 00 90 38 40
81 70 80 38 40 00 90 3d 40 83 60 80 3d 40 00 90 3c 40 81 70 80 3c 40 00 90 3d 40 81 70 80 3d 40 95 48 90 31 40 3c 80 31
40 00 90 33 40 3c 80 33 40 00 90 34 40 78 80 34 40 00 90 36 40 78 80 36 40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36
40 00 90 38 40 3c 80 38 40 00 90 39 40 3c 80 39 40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34
40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34 40 00 90 36 40 3c 80 36 40 00 90 38 40 3c 80 38
40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34 40 00 90 33 40 3c 80 33 40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34
40 00 90 33 40 3c 80 33 40 00 90 34 40 3c 80 34 40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34 40 00 90 39 40 3c 80 39
40 00 90 38 40 3c 80 38 40 00 90 39 40 3c 80 39 40 00 90 33 40 3c 80 33 40 00 90 31 40 3c 80 31 40 00 90 33 40 3c 80 33
40 00 90 34 40 3c 80 34 40 00 90 33 40 3c 80 33 40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36 40 00 90 38 40 3c 80 38
40 00 90 31 40 81 70 80 31 40 95 48 90 34 40 78 80 34 40 00 90 39 40 78 80 39 40 00 90 39 40 78 80 39 40 00 90 39 40 78
80 39 40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36 40 00 90 38 40 81 70 80 38 40 00 90 39 40 81 70 80 39 40 00 90 36
40 81 70 80 36 40 00 90 2f 40 81 70 80 2f 40 00 90 34 40 81 70 80 34 40 00 90 2d 40 83 60 80 2d 40 00 90 2c 40 81 70 80
2c 40 91 68 90 2c 40 78 80 2c 40 00 90 31 40 78 80 31 40 00 90 31 40 78 80 31 40 00 90 31 40 78 80 31 40 00 90 2f 40 3c
80 2f 40 00 90 2e 40 3c 80 2e 40 00 90 2f 40 3c 80 2f 40 00 90 2e 40 3c 80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2f 40 3c
80 2f 40 00 90 2e 40 3c 80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2e 40 3c 80 2e 40 00 90 2f 40 3c 80 2f 40 00 90 2e 40 3c
80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c 80 2a 40 00 90 2e 40 3c 80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c
80 2a 40 00 90 2c 40 3c 80 2c 40 00 90 2e 40 3c 80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2f 40 3c 80 2f 40 00 90 2e 40 3c
80 2e 40 00 90 2c 40 3c 80 2c 40 00 90 2b 40 81 70 80 2b 40 00 90 2c 40 81 70 80 2c 40 00 90 2e 40 81 70 80 2e 40 00 90
33 40 81 70 80 33 40 00 90 2c 40 81 70 80 2c 40 85 50 90 25 40 83 60 80 25 40 00 90 30 40 81 70 80 30 40 00 90 28 40 81
70 80 28 40 00 90 27 40 83 60 80 27 40 00 90 25 40 78 80 25 40 00 90 25 40 3c 80 25 40 00 90 27 40 3c 80 27 40 00 90 28
40 78 80 28 40 00 90 2a 40 78 80 2a 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c 80 2a 40 00 90 2c 40 3c 80 2c 40 00 90 2d
40 3c 80 2d 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c 80 2a 40 00 90 28 40 3c 80 28 40 00 90 2c 40 3c 80 2c 40 00 90 2a
40 3c 80 2a 40 00 90 28 40 3c 80 28 40 00 90 2a 40 3c 80 2a 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c 80 2a 40 00 90 28
40 3c 80 28 40 00 90 27 40 3c 80 27 40 00 90 2a 40 3c 80 2a 40 00 90 28 40 78 80 28 40 00 90 34 40 78 80 34 40 00 90 39
40 78 80 39 40 00 90 39 40 78 80 39 40 00 90 39 40 78 80 39 40 00 90 38 40 3c 80 38 40 00 90 36 40 3c 80 36 40 00 90 38
40 78 80 38 40 00 90 2c 40 78 80 2c 40 00 90 31 40 78 80 31 40 00 90 2f 40 78 80 2f 40 00 90 2d 40 81 70 80 2d 40 00 90
2c 40 78 80 2c 40 8f 00 90 31 40 78 80 31 40 00 90 36 40 78 80 36 40 00 90 36 40 78 80 36 40 00 90 36 40 78 80 36 40 00
90 35 40 3c 80 35 40 00 90 33 40 3c 80 33 40 00 90 35 40 81 70 80 35 40 00 90 36 40 81 70 80 36 40 86 48 90 2c 40 78 80
2c 40 00 90 31 40 78 80 31 40 00 90 31 40 78 80 31 40 00 90 31 40 78 80 31 40 00 90 30 40 3c 80 30 40 00 90 2e 40 3c 80
2e 40 00 90 30 40 3c 80 30 40 00 90 31 40 3c 80 31 40 00 90 33 40 3c 80 33 40 00 90 30 40 3c 80 30 40 00 90 28 40 81 70
80 28 40 78 90 2d 40 78 80 2d 40 00 90 27 40 81 70 80 27 40 78 90 2c 40 78 80 2c 40 00 90 25 40 78 80 25 40 8c 18 90 31
40 81 70 80 31 40 00 90 30 40 81 70 80 30 40 00 90 34 40 81 70 80 34 40 00 90 33 40 83 60 80 33 40 00 90 31 40 78 80 31
40 00 90 34 40 78 80 34 40 00 90 39 40 78 80 39 40 00 90 39 40 78 80 39 40 00 90 39 40 78 80 39 40 00 90 38 40 3c 80 38
40 00 90 36 40 3c 80 36 40 00 90 34 40 3c 80 34 40 00 90 33 40 3c 80 33 40 00 90 31 40 3c 80 31 40 00 90 2f 40 3c 80 2f
40 00 90 2e 40 81 70 80 2e 40 00 90 2d 40 82 68 80 2d 40 00 90 2c 40 3c 80 2c 40 00 90 2a 40 3c 80 2a 40 00 90 29 40 81
70 80 29 40 00 90 2a 40 83 60 80 2a 40 00 90 2c 40 8f 00 80 2c 40 00 90 2b 40 83 60 80 2b 40 00 90 2c 40 87 40 80 2c 40
00 90 31 40 8f 00 80 31 40 77 90 00 00 00 ff 2f 00
```


### How to split MIDI tracks into separate MIDI files ###

The following program takes a multi-track MIDI file with three
or more tracks, and splits out each track into a separate MIDI
file.  The expression track of the original MIDI file is copied
into the 0th track of the new MIDI files, and the individual
tracks of the first MIDI file are copied to the 1st track of
the output MIDI files.

```cpp
#include "MidiFile.h"
#include <iostream>
#include <vector>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   if (argc != 3) {
      cerr << "Usage: " << argv[0] << " input output" << endl;
      return 1;
   }
   MidiFile midifile(argv[1]);
   if (!midifile.status()) {
      cerr << "Problem reading input" << endl;
      return 1;
   }
   if (midifile.getTrackCount() < 3) {
      cerr << "Not enough tracks to split" << endl;
      return 1;
   }
   string basename = argv[2];
   if (basename.substr(basename.size() - 4) == ".mid")
      basename = basename.substr(0, basename.size() - 4);
   int outcount = midifile.getTrackCount() - 1;
   vector<MidiFile> outputs(outcount);
   for (int i=0; i<outcount; i++) {
      outputs[i].addTrack();
      for (int j=0; j<midifile[0].getEventCount(); j++)
         outputs[i].addEvent(midifile[0][j]);
      for (int j=0; j<midifile[i+1].getEventCount(); j++)
         outputs[i].addEvent(1, midifile[i+1][j]);
   }
   for (int i=0; i<outcount; i++)
      outputs[i].write(basename + "-" + to_string(i+1) + ".mid");
   return 0;
}
```

Here is the first file extracted from the MIDI file input for the
temperament example:

```
4d 54 68 64 00 00 00 06 00 01 00 02 00 78 4d 54 72 6b 00 00 00 13 00 ff 51
03 08 8e 6c 00 ff 58 04 02 01 30 08 00 ff 2f 00 4d 54 72 6b 00 00 09 bd b2
50 90 49 40 81 70 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80
4c 40 00 90 4b 40 83 60 80 4b 40 00 90 49 40 82 68 80 49 40 00 90 4b 40 78
80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4b 40 78 80 4b 40 00 90 49 40 81 70
80 49 40 00 90 47 40 81 70 80 47 40 00 90 49 40 81 70 80 49 40 00 90 4b 40
81 70 80 4b 40 82 68 90 4c 40 78 80 4c 40 00 90 4b 40 78 80 4b 40 00 90 49
40 78 80 49 40 00 90 47 40 78 80 47 40 00 90 4b 40 78 80 4b 40 00 90 50 40
82 68 80 50 40 00 90 4e 40 78 80 4e 40 00 90 50 40 78 80 50 40 00 90 51 40
78 80 51 40 00 90 53 40 84 58 80 53 40 00 90 51 40 78 80 51 40 00 90 50 40
78 80 50 40 00 90 4e 40 78 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c
80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 83 60
80 49 40 00 90 47 40 82 68 80 47 40 00 90 49 40 78 80 49 40 00 90 47 40 78
80 47 40 00 90 45 40 78 80 45 40 00 90 44 40 81 70 80 44 40 00 90 46 40 78
80 46 40 00 90 47 40 78 80 47 40 00 90 49 40 81 70 80 49 40 00 90 47 40 83
60 80 47 40 00 90 46 40 81 70 80 46 40 00 90 47 40 84 58 80 47 40 00 90 49
40 78 80 49 40 00 90 4b 40 78 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4c 40
81 70 80 4c 40 00 90 4b 40 81 70 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4b
40 78 80 4b 40 00 90 4c 40 78 80 4c 40 00 90 4e 40 78 80 4e 40 00 90 50 40
3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 51 40 3c
80 51 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80
4c 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c
40 00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40
00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4e 40 3c 80 4e 40 00
90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90
4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49
40 3c 80 49 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40
3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c
80 4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 4b 40 3c 80
4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 49 40 3c 80 49
40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40
00 90 46 40 3c 80 46 40 00 90 49 40 3c 80 49 40 00 90 47 40 82 2c 80 47 40
00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 47 40 3c 80 47 40 00
90 49 40 82 68 80 49 40 00 90 4c 40 78 80 4c 40 00 90 4b 40 82 68 80 4b 40
00 90 4e 40 78 80 4e 40 00 90 4c 40 84 58 80 4c 40 00 90 4b 40 81 70 80 4b
40 00 90 49 40 81 70 80 49 40 00 90 48 40 3c 80 48 40 00 90 46 40 3c 80 46
40 00 90 48 40 78 80 48 40 00 90 4b 40 78 80 4b 40 00 90 50 40 3c 80 50 40
00 90 4e 40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 51 40 3c 80 51 40 00
90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90
50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e
40 3c 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40
3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4e 40 3c 80 4e 40 00 90 4d 40 81
70 80 4d 40 00 90 51 40 81 70 80 51 40 00 90 50 40 84 58 80 50 40 00 90 4e
40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 81 70 80 4a 40 78 90 49
40 78 80 49 40 00 90 4e 40 78 80 4e 40 00 90 4e 40 78 80 4e 40 00 90 4e 40
78 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 4c 40 82
68 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40 83
60 80 4a 40 00 90 49 40 81 70 80 49 40 00 90 4e 40 81 70 80 4e 40 00 90 4c
40 81 70 80 4c 40 00 90 4c 40 81 34 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90
4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4c
40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40
3c 80 4a 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c
80 49 40 00 90 47 40 3c 80 47 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 83 60
80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80 4c 40 00 90 4b 40
84 58 80 4b 40 00 90 44 40 78 80 44 40 00 90 49 40 78 80 49 40 00 90 49 40
78 80 49 40 00 90 49 40 78 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c
80 45 40 00 90 47 40 83 60 80 47 40 00 90 45 40 81 70 80 45 40 00 90 44 40
81 70 80 44 40 81 70 90 4b 40 83 60 80 4b 40 00 90 4a 40 81 70 80 4a 40 00
90 4e 40 81 70 80 4e 40 00 90 4d 40 81 70 80 4d 40 00 90 4c 40 81 70 80 4c
40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40
00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00
90 47 40 3c 80 47 40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90
47 40 3c 80 47 40 00 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 49
40 3c 80 49 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 49 40
3c 80 49 40 00 90 48 40 81 70 80 48 40 00 90 47 40 81 70 80 47 40 00 90 46
40 81 70 80 46 40 00 90 45 40 81 70 80 45 40 00 90 44 40 81 70 80 44 40 89
30 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90 4c 40 81 70 80
4c 40 00 90 4b 40 85 50 80 4b 40 00 90 49 40 83 60 80 49 40 00 90 48 40 81
70 80 48 40 78 90 49 40 3c 80 49 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 78
80 4c 40 00 90 4e 40 78 80 4e 40 00 90 50 40 3c 80 50 40 00 90 4e 40 3c 80
4e 40 00 90 50 40 3c 80 50 40 00 90 51 40 3c 80 51 40 00 90 50 40 3c 80 50
40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 50 40 3c 80 50 40
00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00
90 50 40 3c 80 50 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90
4b 40 3c 80 4b 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b
40 3c 80 4b 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 3c 80 4e 40 00 90 4c 40
3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4c 40 3c
80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40 00 90 4a 40 3c 80
4a 40 00 90 4c 40 3c 80 4c 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49
40 00 90 47 40 3c 80 47 40 00 90 4a 40 3c 80 4a 40 00 90 49 40 3c 80 49 40
00 90 47 40 3c 80 47 40 00 90 49 40 3c 80 49 40 00 90 4a 40 3c 80 4a 40 00
90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c 80 45 40 00 90
49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40 3c 80 45 40 00 90 47
40 3c 80 47 40 00 90 49 40 3c 80 49 40 00 90 47 40 3c 80 47 40 00 90 45 40
3c 80 45 40 00 90 44 40 3c 80 44 40 00 90 47 40 3c 80 47 40 00 90 45 40 81
70 80 45 40 81 70 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90
4c 40 81 70 80 4c 40 00 90 4b 40 84 58 80 4b 40 00 90 49 40 78 80 49 40 00
90 50 40 78 80 50 40 00 90 50 40 78 80 50 40 00 90 50 40 78 80 50 40 00 90
4e 40 3c 80 4e 40 00 90 4c 40 3c 80 4c 40 00 90 4e 40 81 70 80 4e 40 00 90
4c 40 83 60 80 4c 40 00 90 4b 40 81 70 80 4b 40 00 90 4f 40 81 70 80 4f 40
00 90 4e 40 81 70 80 4e 40 00 90 42 40 81 70 80 42 40 00 90 41 40 81 70 80
41 40 00 90 45 40 81 70 80 45 40 00 90 44 40 78 80 44 40 83 60 90 48 40 78
80 48 40 00 90 4e 40 78 80 4e 40 00 90 4e 40 78 80 4e 40 00 90 4e 40 78 80
4e 40 00 90 4c 40 3c 80 4c 40 00 90 4b 40 3c 80 4b 40 00 90 4c 40 78 80 4c
40 00 90 4b 40 3c 80 4b 40 00 90 49 40 3c 80 49 40 00 90 4b 40 78 80 4b 40
00 90 48 40 78 80 48 40 00 90 49 40 85 50 80 49 40 00 90 48 40 81 70 80 48
40 00 90 47 40 81 70 80 47 40 00 90 46 40 81 70 80 46 40 00 90 45 40 81 70
80 45 40 00 90 44 40 83 60 80 44 40 00 90 46 40 81 70 80 46 40 00 90 48 40
81 70 80 48 40 00 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40 00 90
4c 40 83 60 80 4c 40 00 90 4b 40 78 80 4b 40 00 90 49 40 78 80 49 40 00 90
48 40 81 70 80 48 40 00 90 49 40 83 60 80 49 40 00 90 48 40 81 70 80 48 40
00 90 49 40 8f 00 80 49 40 77 90 00 00 00 ff 2f 00
```



### How to create vibrato with pitch-bend messages in a MIDI file ###

This example demonstrates the generation of a constant vibrato for notes
in a MIDI file (on a particular channel).  The program adds an extra
track at the end of the file to store the pitch bends.  The vibrato
rate is constant regardless of the tempo setting for the MIDI file,
since the vibrato is calculated in physical time rather than tick time.
The `MidiFile::getAbsoluteTickTime()` function calculates the conversion
between physical time in seconds and tick time in the MIDI file.

```cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <utility>
#include <cmath>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.define("f|frequency=d:4.0",     "vibrato frequency");
   options.define("d|depth=d:20.0",        "vibrato depth in cents");
   options.define("b|bend-max=d:200.0",    "pitch bend depth");
   options.define("s|sample-rate=d:100.0", "sample rate");
   options.define("o|output-file=s",       "output filename");
   options.define("c|channel=i:0",         "output channel");
   options.process(argc, argv);

   MidiFile midifile;
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
   if (!midifile.status()) {
      cerr << "Problem reading file" << endl;
      return 1;
   }

   string filename  = options.getString("output-file");
   int    channel   = options.getInteger("channel");
   double freq      = options.getDouble("frequency");
   double depth     = options.getDouble("depth");
   double bend      = options.getDouble("bend-max");
   double srate     = options.getDouble("sample-rate");
   double phase     = 0.0;
   double twopi     = 2.0 * M_PI;
   double increment = twopi * freq / srate;
   double maxtime   = midifile.getFileDurationInSeconds();
   midifile.addTrack(); // store vibrato in separate track
   pair<int, double> tickbend;
   vector<pair<double, double>> storage;
   int count = maxtime * srate;
   storage.reserve(maxtime * srate + 1000);
   for (int i=0; i<count; i++) {
      tickbend.first = int(midifile.getAbsoluteTickTime(i/srate) + 0.5);
      tickbend.second = depth/bend * sin(phase);
      if ((storage.size() > 0) && (tickbend.first == 0)) break;
      storage.push_back(tickbend);
      phase += increment;
      if (phase > twopi) phase -= twopi;
   }
   int track = midifile.getTrackCount() - 1;
   for (int i=0; i<(int)storage.size(); i++)
      midifile.addPitchBend(track, storage[i].first, channel, storage[i].second);
   if (filename.empty()) cout << midifile;
   else midifile.write(filename);
   return 0;
}
```


### Polyrhythm generator ###

Here is a program that generates polyrhythm patterns. Command line
options are:

| option   | default value | meaning                                    |
|:--------:|:-------------:|:------------------------------------------:|
| `-a`     |   2           | first instrument's division of the cycle   |
| `-b`     |   3           | first instrument's division of the cycle   |
| `-c`     |   10          | number of cycles                           |
| `-d`     |   2.0         | duration of each cycle                     |
| `--key1` |   76          | percussion key number for first instrument |
| `--key2` |   77          | percussion key number for first instrument |
| `-o`     |               | output filename                            |


```cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <utility>
#include <cmath>
using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.define("a=i:2",           "cycle division 1");
   options.define("b=i:3",           "cycle division 2");
   options.define("c|cycle=i:10",    "cycle count");
   options.define("d|dur=d:2.0",     "duration of cycle in seconds");
   options.define("key1=i:76",       "first percussion key number");
   options.define("key2=i:77",       "second percussion key number");
   options.define("o|output-file=s", "output filename");
   options.process(argc, argv);

   int a = options.getInteger("a");
   int b = options.getInteger("b");
   int c = options.getInteger("cycle");
   int key1 = options.getInteger("key1");
   int key2 = options.getInteger("key2");
   double dur = options.getDouble("dur");
   double tempo = 60.0 / dur;

   MidiFile midifile;
   midifile.setTPQ(a*b);
   midifile.addTempo(0, 0, tempo);
   midifile.addTracks(2);
   for (int i=0; i<b*c + 1; i++) {
      midifile.addNoteOn(1, i*a, 9, key1, 64);
      midifile.addNoteOff(1, (i+1)*a, 9, key1);
   }
   for (int i=0; i<a*c + 1; i++) {
      midifile.addNoteOn(2, i*b, 9, key2, 64);
      midifile.addNoteOff(2, (i+1)*b, 9, key2);
   }

   string filename  = options.getString("output-file");
   if (filename.empty()) cout << midifile;
   else midifile.write(filename);
   return 0;
}
```

This program demonstrates how to set the ticks-per-quarter-note value in the
MIDI header.  In this case it is set to the factor of `a*b` which is the duration
of one cycle (so each cycle has the duration of a quarter note).  A tempo meta-message
is also calculated based on the desired duration of a cycle.

For example, try the options `-a 3 -b 4 -c 200 -d 1 -o 3x4.mid`, which will play
a 3-against-4 pattern for 200 cycles with each cycle lasting 1 second.  Or here
is 11-against-13 for 100 cycles: `-a 11 -b 13 -c 100 -d 5 -o 11x13.mid`.



