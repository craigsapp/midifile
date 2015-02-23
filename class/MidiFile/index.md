---
layout: default
breadcrumbs: [['/doc','documentation'], ['/class', 'classes']]
title:  MidiFile class
grouptitle: ['https://github.com/craigsapp/midifile/blob/master/src-library', 'Source Code']
group: [
   ['https://github.com/craigsapp/midifile/blob/master/src-library/MidiFile.cpp', 'MidiFile.cpp'],
]
vim: ts=3 hlsearch
---

{% include stylesheets/function.css %}
{% include docslot/docslot.html %}

The MidiFile class is an interface for reading and writing Standard
MIDI files.  MIDI file tracks are stored as a list of <a
href="../MidiEventList">MidiEventList</a> objects, which in turn
are lists of <a href="../MidiEvent">MidiEvents</a>.  The MidiFile
class can be considered to be a two-dimensional array of
events that are accessible with the [] operator.  The
first dimension is the track index, and the second dimension is the
event index for the given track.  Thus, `midifile[2][15]` would return
the 16th event in the third track.

<a href="../MidiEvent">MidiEvents</a> consist of a list of MIDI message
bytes along with a timing variables.  For example, the construct
`midifile[2][15][0]` would return the MIDI command byte for the 16th
message in the third track, and `midifile[2][15].tick` would return the
timestamp for the event (either in delta or absolute tick values, see
the tick-related functions described further below).


Reading/writing functions
-------------------------

{% include docslot/MidiFile_readwrite.html %}


<br/>
Track-related functions
-----------------------

{% include docslot/MidiFile_track.html %}


<br/>
Time-related functions
----------------------

<a href="../MidiEvent">MidiEvents</a> stored in a MidiFile structure contain
two public variables related to time:
<ol style="margin-left:40px; margin-right:40px;">
<li> int MidiEvent::tick &mdash; Quanta time-units describing durations in
     Standard MIDI Files.
</li>
<li> double MidiEvent::seconds &mdash; Interpreted physical time units in
     seconds calculated by
     <a href="#doTimeInSecondsAnalysis()">doTimeInSecondsAnalysis()</a>
     from .tick data and tempo meta-messages stored in the MidiFile.
</li>
</ol>

{% include docslot/MidiFile_time.html %}

<br/>
Other functions
----------------

{% include docslot/MidiFile_other.html %}


<br/>
Static functions
----------------

{% include docslot/MidiFile_static.html %}

<br/>
Private functions
-----------------

{% include docslot/MidiFile_private.html %}


{% include docslot/keyboardCommands.html %}

