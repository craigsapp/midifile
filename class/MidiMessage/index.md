---
layout: default
breadcrumbs: [
		['/','home'], 
		['/doc','documentation'], 
		['/class', 'classes']
	]
title:  MidiMessage class
grouptitle: ['https://github.com/craigsapp/midifile/blob/master/src-library', 'Source Code']
group: [
   ['https://github.com/craigsapp/midifile/blob/master/src-library/MidiMessage.cpp', 'MidiMessage.cpp'],
]
---

{% include docslot/docslot.html %}

The MIDI message class contains the raw MIDI bytes which are stored in
a Standard MIDI File.  The class is inherited from vector<uchar>, so all
[STL vector class functions](http://www.cplusplus.com/reference/vector/vector)
can be used in this class.  The 
   <span class="class-link" cname="MidiEvent">MidiEvent class</span> 
inherits these function from the MidiMessage class.

The first byte in the MIDI message is expected to be a command byte, which is
a byte in the range from 0x80 to 0xff.  Running status is not allowed in 
MidiMessage class data, and any missing running status byte will be inserted
into the message when reading from a MIDI file.  Currently MIDI files cannot be
written in running status mode by the MidiFile class, but if that is needed, then
the MidiFile class will be responsible for removing the running status command
byte as necessary from the MidiMessage data as it is being written to a file.

The command byte of a MIDI message is in the range from 0x80 to 0xff hexadecimal
(128-255 decimal).  Each command converts 16 bytes, such as 0x80 to 0x8f for
the note-off command.  The "<b>8</b>" for note-offs is the *command nibble", which
are the top four bits of the byte.  The bottom nibble indicates the MIDI channel
of the command, and ranges from 0x0 to 0xf hex (0 to 15 decimal).  Thus, 0x80 is 
a note-off command on the first channel, 0x81 is the note-off command on the
second channel, ..., and 0x8f is the note-off command on the sixteenth channel.

There are seven MIDI commands from 0x80 to 0xe0 command nibbles, plus 16 
miscellaneous commands starting with a 0xf0 nibble that don't refer to 
MIDI channels in the bottom nibble.  Each command has an expected number
of parameter bytes after it, which are in the range from 0x00 to 0x7f hexadecimal
(0 to 127 decimal).  Here is a table summarizing the seven main MIDI commands
and their required parameter count.

<style>

table.messagelist, 
table.messagelist tr,
table.messagelist td {
   padding: 0;
   border: 0;
}

</style>

<table class="messagelist" cellpadding="0" cellspacing="0">

<tr>
	<td style="font-weight:bold;">Command nibble</td>
	<td style="font-weight:bold;">Command name</td>
	<td style="font-weight:bold;">Parameter count</td>
	<td style="font-weight:bold;">Parameter meanings</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">8</span>0</td>
	<td>Note off</td>
	<td>2</td>
	<td>key, off velocity</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">9</span>0</td>
	<td>Note on</td>
	<td>2</td>
	<td>key, on velocity</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">A</span>0</td>
	<td>Aftertouch</td>
	<td>2</td>
	<td>key, pressure</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">B</span>0</td>
	<td>Continuous controller</td>
	<td>2</td>
	<td>controller number, controller value</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">C</span>0</td>
	<td>Patch change</td>
	<td>1</td>
	<td>instrument number</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">D</span>0</td>
	<td>Channel pressure</td>
	<td>2</td>
	<td>key, off velocity</td>
</tr>

<tr>
	<td>0x<span style="color:#aa0000; font-weight:bold; font-size:110%;">E</span>0</td>
	<td>Pitch-bend</td>
	<td>2</td>
	<td>LSB, MSB</td>
</tr>

</table>


&nbsp;

Public functions
----------------

{% include docslot/MidiMessage_other.html %}

&nbsp;

Functions related to meta-messages
----------------------------------

Meta messages are not strictly MIDI messages, but pose as such within
MIDI files.  A meta message start with the command byte 0xff, followed by 
the meta message type and then the number of data bytes to follow in the
message.  The following functions relate to processing meta messages.

{% include docslot/MidiMessage_meta.html %}

{% include docslot/keyboardCommands.html %}

