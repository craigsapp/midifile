---
layout: default
breadcrumbs: [['/doc','documentation'], ['/class', 'classes']]
title:  MidiEvent class
grouptitle: ['https://github.com/craigsapp/midifile/blob/master/src-library', 'Source Code']
group: [
   ['https://github.com/craigsapp/midifile/blob/master/src-library/MidiEvent.cpp', 'MidiEvent.cpp'],
]
---

The MidiEvent class is a <a href="../MidiMessage">MidiMessage</a> plus an 
added timestamp public variable, <b>.time</b>.  The MidiEvent class also
adds other support variables such as <b>.track</b> which store the 
track number in the MIDI file that the MidiEvent occurs in.

{% include docslot/docslot.html %}

&nbsp;

Public functions
----------------

{% include docslot/MidiEvent.html %}

&nbsp;

Functions inherited from <a href="../MidiMessage">MidiMessage</a>
---------------------------------------------------------------

{% include docslot/MidiMessage_other.html %}
{% include docslot/MidiMessage_meta.html %}



{% include docslot/keyboardCommands.html %}

