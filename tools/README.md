# Example programs that use the midifile library

| Program | Description |
| ------- | ----------- |
| [80off.cpp](https://github.com/craigsapp/midifile/blob/master/tools/80off.cpp) | Converts any note-off messages in the form "9? ?? 00" to "8? ?? 40".  Release velocity can be set with the -v option. |
| [asciimidi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/asciimidi.cpp) | Print MIDI file in binasc format, allowing for reconversion into a binary Standard MIDI File. |
| [base642midi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/base642midi.cpp) | Test of base64 import of Standard MIDI files. |
| [binasc.cpp](https://github.com/craigsapp/midifile/blob/master/tools/binasc.cpp) | Binary to hex-byte ASCII converter. |
| [chaninfo.cpp](https://github.com/craigsapp/midifile/blob/master/tools/chaninfo.cpp) | List channels used for notes in each track and list timbres used in each channel. |
| [createmidifile.cpp](https://github.com/craigsapp/midifile/blob/master/tools/createmidifile.cpp) | Demonstration of how to create a Multi-track MIDI file. |
| [createmidifile2.cpp](https://github.com/craigsapp/midifile/blob/master/tools/createmidifile2.cpp) | Demonstration of how to create a Multi-track MIDI file with convenience functions. |
| [deltatimes.cpp](https://github.com/craigsapp/midifile/blob/master/tools/deltatimes.cpp) | Generate list of delta times in a MIDI file, either as MIDI ticks, quarter notes, or at real-time values in seconds or milliseconds (why applying tempo meta messages to the tick values).  Useful for determining if the MIDI file contains a score or a performance. |
| [drumtab.cpp](https://github.com/craigsapp/midifile/blob/master/tools/drumtab.cpp) | Demonstration of how to convert a drum tab into a Multi-track MIDI file with one track for each drum part. |
| [durations.cpp](https://github.com/craigsapp/midifile/blob/master/tools/durations.cpp) | Print note durations in a MidiFile.  Note-ons with no no matching note-offs (such as for rhythm-channel events) will have a duration of 0. |
| [echomidi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/echomidi.cpp) | Read in a MIDI file and output it with minimal processing. |
| [extractlyrics.cpp](https://github.com/craigsapp/midifile/blob/master/tools/extractlyrics.cpp) | Demonstration of how to extract lyrics from a MIDI file. Used the -s option to display the timestamp in seconds that the lyric occurs at. |
| [henonfile.cpp](https://github.com/craigsapp/midifile/blob/master/tools/henonfile.cpp) | Creates a fractal melodic line based on the Henon Map.  Output can be either a MIDI file, Guido Music Notation (GMN), Humdrum, or plain text. |
| [linkinfo.cpp](https://github.com/craigsapp/midifile/blob/master/tools/linkinfo.cpp) | Demo program for linking note-ons and note-offs. |
| [maxtick.cpp](https://github.com/craigsapp/midifile/blob/master/tools/maxtick.cpp) | Calculate the maximum timestamp in a MIDI file. |
| [mid2hex.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mid2hex.cpp) | Convert a binary standard MIDI file (or binasc MIDI file, or hex-byte MIDI file) into a hex byte-code MIDI file. |
| [mid2hum.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mid2hum.cpp) | Converts a MIDI file into a Humdrum file. |
| [mid2mat.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mid2mat.cpp) | Converts a MIDI file into a text-based note matrix. |
| [mid2mtb.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mid2mtb.cpp) | Converts a MIDI file into a MIDI Toolbox compatible text file. |
| [mid2svg.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mid2svg.cpp) | Convert a MIDI file into an SVG piano roll. |
| [midi2base64.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2base64.cpp) | Test of base64 export of Standard MIDI files. |
| [midi2beep.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2beep.cpp) | Linux-only: Play a monophonic midi file on the PC Speaker (i.e. the midifile contains only a single track on a single channel playing a single note at a time). |
| [midi2binasc.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2binasc.cpp) | Converts a MIDI file into an ASCII format which can be converted back into a MIDI file with the binasc program: https://github.com/craigsapp/binasc |
| [midi2chords.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2chords.cpp) | Identify chord sequences in MIDI files.  Whenever there are three or more note attacks at a given time, a chord will be identified (currently notes must all attack all at the same tick time).  Additional chord identifications can be added to the Sonority::identifySonority() function. |
| [midi2melody.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2melody.cpp) | Converts a single melody MIDI file/track into an ASCII text format with starting time and pitch. |
| [midi2notes.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2notes.cpp) | Converts a MIDI file into a text based notelist. |
| [midi2skini.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2skini.cpp) | Converts a Standard MIDI file into the SKINI data format. |
| [midi2text.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midi2text.cpp) | Converts a MIDI file into a text based notelist. |
| [midicat.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midicat.cpp) | Concatenate multiple MIDI files into single type-0 MIDI file. |
| [mididiss.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mididiss.cpp) | Calculate an average dissonance score.  The input MIDI file is expected to be quantized.  Scores: -1 = rest (ignore) 0 = unison / octave / single note (no intervals) 1 = other perfect intervals P4 P5 2 = imperfect intervals m3 M3 m6 M6 3 = weak dissonance M2 m7 4 = strong dissonant M7 m9 A4 (other than minor second) 5 = minor second M2 The score of a sonority is the maximum value of any interval pairing Then the scores a duration-weighted to calculate an average score for all individual sonority scores. |
| [midiexcerpt.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midiexcerpt.cpp) | Extracts a time region from a MIDI file.  Notes starting before the start time will be ignored. Notes not ending before the end time of the file will be turned off at the given end time. |
| [midimean.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midimean.cpp) | Calculate the mean pitch of MIDI notes in a midifile, excluding any notes in drum track.  The mean can be weighted by duration, and a specific track or channel can be selected. |
| [midimixup.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midimixup.cpp) | Reads a standard MIDI file, move the pitches around into a random order. |
| [midirange.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midirange.cpp) | Note pitch range in data, highest note first, then lowest. Ignoring channel 10 (0x09). |
| [midireg.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midireg.cpp) | Categorize the number of notes in low, mid, high register Ignoring channel 10 (0x09).  The default low register is defined as notes lower than C3 (midi key number 48), and the default definition of high notes are notes higher than C5 (midi key number 72). |
| [miditickdur.cpp](https://github.com/craigsapp/midifile/blob/master/tools/miditickdur.cpp) | List notes start times in ticks/seconds in MIDI file. |
| [miditime.cpp](https://github.com/craigsapp/midifile/blob/master/tools/miditime.cpp) | Displays the absolute tick time and absolute time in seconds for MIDI events in a MIDI file, along with the track information. |
| [midiuniq.cpp](https://github.com/craigsapp/midifile/blob/master/tools/midiuniq.cpp) | When notes attacks for the same pitch occur at the same time, remove one of them. |
| [mts-type2.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mts-type2.cpp) | Test of MTS (MIDI Tuning Specification), second type  (real-time remaping of any number of pitches).   This example sets MIDI keys from 60 to 84 to be quarter tones and then plays a one-octave quarter-note scale, repeating the first note which should be one octave below the penultimate note. |
| [mts-type9.cpp](https://github.com/craigsapp/midifile/blob/master/tools/mts-type9.cpp) | Test of MTS (MIDI Tuning Specification), nineth type  (real-time two-byte octave temperaments). |
| [peep2midi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/peep2midi.cpp) | Convert Performance Expression Extraction Program output data into MIDI data. |
| [perfid.cpp](https://github.com/craigsapp/midifile/blob/master/tools/perfid.cpp) | Determine if a MIDI file is a live performance or if it is step edit. |
| [readstatus.cpp](https://github.com/craigsapp/midifile/blob/master/tools/readstatus.cpp) | Demonstration of checking the read status. |
| [redexpress.cpp](https://github.com/craigsapp/midifile/blob/master/tools/redexpress.cpp) | Adds expression information to notes extracted from Red Welte-Mignon piano rolls. |
| [removenote.cpp](https://github.com/craigsapp/midifile/blob/master/tools/removenote.cpp) | Demonstration of how to remove a MIDI message. |
| [retick.cpp](https://github.com/craigsapp/midifile/blob/master/tools/retick.cpp) | Change TPQ to a new value and update timestamps for new TPQ to keep time values the same as before. |
| [shutak.cpp](https://github.com/craigsapp/midifile/blob/master/tools/shutak.cpp) | Convert lines of MIDI note numbers into MIDI files. Multiple lines will be placed in multiple tracks. Each note has the duration of one second (quarter notes at MM60). |
| [smfdur.cpp](https://github.com/craigsapp/midifile/blob/master/tools/smfdur.cpp) | Calcualte the total duration of a MIDI file. |
| [sortnotes.cpp](https://github.com/craigsapp/midifile/blob/master/tools/sortnotes.cpp) | Sort notes that occur at the same tick time in the same track. Note-offs will be placed before Note-ons when they occur at the same time, and note on/off groups will each be sorted further by key number (low to high). |
| [stretch.cpp](https://github.com/craigsapp/midifile/blob/master/tools/stretch.cpp) | Stretches (or shrinks): 1. The position of bars (measures) in tracks, without affecting tempo, and/or 2. The tempo (BPM) itself. |
| [sysextest.cpp](https://github.com/craigsapp/midifile/blob/master/tools/sysextest.cpp) | Demonstration of how to create a MIDI file with system exclusive messages.  A complete sysex message is given in track 1 and the same sysex messages is given in segmented form in track 2.  MIDI files require the size of the sysex message to be give after the first byte of the message (and the count excludes the first byte.  F7 continuation messages also require a size after the initial message byte.  The MidiFile class transparently handles inserting this size parameter into the MIDI file, so you should not add it yourself. |
| [temper.cpp](https://github.com/craigsapp/midifile/blob/master/tools/temper.cpp) | Split a single-timbre MIDI file into 12 channels, one for each pitch class.  Then apply pitch bends to tune each channel to a specific temperament. This method should work on all synthesizers. |
| [text2midi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/text2midi.cpp) | Converts a text based notelist into a MIDI file. |
| [textmidi.cpp](https://github.com/craigsapp/midifile/blob/master/tools/textmidi.cpp) | Reads a MIDI file and converts data to/from ASCII text. |
| [timeinfo.cpp](https://github.com/craigsapp/midifile/blob/master/tools/timeinfo.cpp) | Display the absolute time for note-ons in both ticks and seconds. MIDI file is merged into a type-0 file first for a single timeline. |
| [toascii.cpp](https://github.com/craigsapp/midifile/blob/master/tools/toascii.cpp) | Convert a Standard MIDI file into binasc format (ASCII version of the MIDI file which can be converted back into the binary format without information loss). |
| [tobin.cpp](https://github.com/craigsapp/midifile/blob/master/tools/tobin.cpp) | Convert binary numbers to decimal. |
| [tobinary.cpp](https://github.com/craigsapp/midifile/blob/master/tools/tobinary.cpp) | Convert a Standard MIDI file into binary format from the binasc format (ASCII version of the MIDI file which can be converted back into the binary format without information loss). |
| [todec.cpp](https://github.com/craigsapp/midifile/blob/master/tools/todec.cpp) | Convert hex bytes into decimal equivalents. |
| [tohex.cpp](https://github.com/craigsapp/midifile/blob/master/tools/tohex.cpp) | Convert decimal value to hex. |
| [type0.cpp](https://github.com/craigsapp/midifile/blob/master/tools/type0.cpp) | Convert a type-1 (multi-track) Standard MIDI file into a type=0 (single track) Standard MIDI file. |
| [vlv.cpp](https://github.com/craigsapp/midifile/blob/master/tools/vlv.cpp) | Converts Variable Length Values into integers as well as converts integers into VLVs. |
