---
layout: default
breadcrumbs: [['/doc','documentation'], ['/class', 'classes']]
title:  Binasc class
grouptitle: ['https://github.com/craigsapp/midifile/blob/master/src-library', 'Source Code']
group: [
   ['https://github.com/craigsapp/midifile/blob/master/src-library/Binasc.cpp', 'Binasc.cpp'],
]
---

<details open>
<summary><span style="font-weight:600; color:black; font-size:130%;">Description<span></summary>

The Binasc class manages conversion between ASCII representation of bytes
and their conversion into bytes.  This class is used by the 
<a href="../MidiFile">MidiFile</a> class to allow for Standard MIDI 
Files to be read/written in an ASCII format.

Here is a low-level representation of a Standard MIDI file as hex bytes:

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

Even though this data is all ASCII characters, it can be parsed transparently 
by the MidiFile::read() function.  The Binasc ASCII byte code syntax can deal
with bytes in different textual formats.  Here is are equivalent ASCII byte 
codes in a higher-level representation that can also be read by 
MidiFile::read(), making the structure of the binary MIDI file more apparent:

```
+M +T +h +d
4'6
2'1
2'3
2'120

; TRACK 0 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'4
v0	ff 2f '0

; TRACK 1 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'118
v0	90 '72 '64
v120	80 '72 '64
v0	90 '72 '64
v120	80 '72 '64
v0	90 '79 '64
v120	80 '79 '64
v0	90 '79 '64
v120	80 '79 '64
v0	90 '81 '64
v120	80 '81 '64
v0	90 '81 '64
v120	80 '81 '64
v0	90 '79 '64
v240	80 '79 '64
v0	90 '77 '64
v120	80 '77 '64
v0	90 '77 '64
v120	80 '77 '64
v0	90 '76 '64
v120	80 '76 '64
v0	90 '76 '64
v120	80 '76 '64
v0	90 '74 '64
v120	80 '74 '64
v0	90 '74 '64
v120	80 '74 '64
v0	90 '72 '64
v240	80 '72 '64
v0	ff 2f '0

; TRACK 2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'125
v0	90 '48 '64
v120	80 '48 '64
v0	90 '60 '64
v120	80 '60 '64
v0	90 '64 '64
v120	80 '64 '64
v0	90 '60 '64
v120	80 '60 '64
v0	90 '65 '64
v120	80 '65 '64
v0	90 '60 '64
v120	80 '60 '64
v0	90 '64 '64
v120	80 '64 '64
v0	90 '60 '64
v120	80 '60 '64
v0	90 '62 '64
v120	80 '62 '64
v0	90 '59 '64
v120	80 '59 '64
v0	90 '60 '64
v120	80 '60 '64
v0	90 '57 '64
v120	80 '57 '64
v0	90 '53 '64
v120	80 '53 '64
v0	90 '55 '64
v120	80 '55 '64
v0	90 '48 '64
v240	80 '48 '64
v0	ff 2f '0
```

A "+" in front of a character will use the ASCII byte code for that
letter in the compiled file.  <b>4'6</b> means create a 4-byte
integer in big-endian byte order with the decimal value 6.  "v120"
for the tick delta times means to convert the decimal value 120
into a variable-length value.  A string such as <b>'64<b> means to
store the decimal value 64 into a single output byte.

</details>

{% include docslot/docslot.html %}

Primary functions
----------------

{% include docslot/Binasc_main.html %}

&nbsp;

Fucntions for ASCII printing options
------------------------------------

{% include docslot/Binasc_options.html %}

&nbsp;

{% include docslot/keyboardCommands.html %}

