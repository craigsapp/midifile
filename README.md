Midifile: C++ MIDI file parsing library
=======================================

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
	<a href="http://midifile.sapp.org/class/MidiEvent">MidiEvent</a>
</td><td>
	The primary storage unit for MidiMessages in a MidiFile.  The class
	consists of a (delta)tick timestamp and a MIDI (or meta) message.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiEventList">MidiEventList</a>
</td><td>
	A data structure that manages the list of MidiEvents in a track.
</td></tr>


<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/MidiMessage">MidiMessage</a>
</td><td>
	The base class for MidiEvents.  This is a vector&lt;uchar&gt; of 
	bytes in a MIDI (or meta) message.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/Binasc">Binasc</a>
</td><td>
	A helper class for MidiFile which allows reading/writing of MIDI
	files in an ASCII format describing the bytes of the binary Standard
	MIDI Files.
</td></tr>

<tr valign="top"><td>
	<a href="http://midifile.sapp.org/class/Options">Options</a>
</td><td>
	A convenience class used for parsing command-line options in 
	the example programs.  This class can be removed from the library
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
or

``` bash
git clone https://cm-github.stanford.edu/craig/midifile
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






MIDI file reading examples
--------------------------

The following program lists all MidiEvents in an input MIDI file. The
program iterates over each track, printing a list of MIDI events
in the track.  For each event, the tick timestamp for the performance
time of the MIDI message is given, followed by the message itself
as a list of hex bytes.

``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() > 0) {
      midifile.read(options.getArg(1));
   } else {
      midifile.read(cin);
   }

   int tracks = midifile.getTrackCount();
   cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
   if (tracks > 1) {
      cout << "TRACKS: " << tracks << endl;
   }
   for (int track=0; track < tracks; track++) {
      if (tracks > 1) {
         cout << "\nTrack " << track << endl;
      }
      for (int event=0; event < midifile[track].size(); event++) {
         cout << dec << midifile[track][event].tick;
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

The program will read the first filename it finds on the command-line, 
or it will read from standard input if no arguments are found.  Both binary
Standard MIDI Files and ASCII representations of MIDI Files can be input 
into the program.  For example, save the following text in a file called
twinkle.txt to use as input data.  This content represents the hex bytes 
for a MIDI file, which will automatically be parsed into a Standard MIDI 
File by the MidiFile class.

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
track, having no content other than the end-of-track meta message, "ff 2f 0"
in hex bytes.  The second track starts with a MIDI note-on message "90 48 40"
(in hex) which will start playing MIDI note 72 (C pitch one octave above
middle C) with a medium loudness (40 hex = 64 in decimal notation).

<pre>
TPQ: 120
TRACKS: 3

Track 0
0	ff 2f 0 

Track 1
0	90 48 40 
120	80 48 40 
120	90 48 40 
240	80 48 40 
240	90 4f 40 
360	80 4f 40 
360	90 4f 40 
480	80 4f 40 
480	90 51 40 
600	80 51 40 
600	90 51 40 
720	80 51 40 
720	90 4f 40 
960	80 4f 40 
960	90 4d 40 
1080	80 4d 40 
1080	90 4d 40 
1200	80 4d 40 
1200	90 4c 40 
1320	80 4c 40 
1320	90 4c 40 
1440	80 4c 40 
1440	90 4a 40 
1560	80 4a 40 
1560	90 4a 40 
1680	80 4a 40 
1680	90 48 40 
1920	80 48 40 
1920	ff 2f 0 

Track 2
0	90 30 40 
120	80 30 40 
120	90 3c 40 
240	80 3c 40 
240	90 40 40 
360	80 40 40 
360	90 3c 40 
480	80 3c 40 
480	90 41 40 
600	80 41 40 
600	90 3c 40 
720	80 3c 40 
720	90 40 40 
840	80 40 40 
840	90 3c 40 
960	80 3c 40 
960	90 3e 40 
1080	80 3e 40 
1080	90 3b 40 
1200	80 3b 40 
1200	90 3c 40 
1320	80 3c 40 
1320	90 39 40 
1440	80 39 40 
1440	90 35 40 
1560	80 35 40 
1560	90 37 40 
1680	80 37 40 
1680	90 30 40 
1920	80 30 40 
1920	ff 2f 0 
</pre>

The default behavior of the MidiFile class is to store the absolute tick
times of MIDI events.  Within Standard MIDI files, the tick values are
in delta format, where the tick value indicates the duration to wait
since the last message.  To access the delta tick values, you can either
(1) subtrack the current tick time from the previous tick time in the list,
or call MidiFile::deltaTime() to convert the absolute tick values into
delta tick values.  

The MidiFile::joinTracks() function can be used to convert multi-track
data into a single time sequence.  The joinTrack() operation can be 
reversed with the MidiFile::splitTracks() function.  Here is a sample
of output data for the same example if joinTracks() is called before
printing the MIDI events:

``` cpp
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;

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



MIDI file writing examples
--------------------------

Here are some examples of MIDI file writing.  MidiFiles can be created 
in either delta or absolute tick timestamp modes.  For now, see the
[createmidifile](https://github.com/craigsapp/midifile/blob/master/src-programs/createmidifile.cpp) 
example program source code, or a higher-level example using convenience functions for creating MIDI events:
[createmidifile2](https://github.com/craigsapp/midifile/blob/master/src-programs/createmidifile2.cpp). 


