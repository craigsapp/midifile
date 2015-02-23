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
MIDI files.  MIDI files contain one or more
<a href="../MidiEventList">lists of MidiEvents</a>, called <i>tracks</i>.
The MidiFile class can be considered a two-dimensional array of
<a href="../MidiEvent">events</a> acessible with the [] operator.  The
first dimension is the track index, and the second dimension is the
event index for the given track.

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

