---
layout: default
breadcrumbs: [
		['/','home'], 
		['/doc','documentation'], 
		['/class', 'classes']
	]
title:  MidiEvent class
grouptitle: ['https://github.com/craigsapp/midifile/blob/master/src-library', 'Source Code']
group: [
   ['https://github.com/craigsapp/midifile/blob/master/src-library/MidiEvent.cpp', 'MidiEvent.cpp'],
]
---

The MidiEvent class is a
  <span class="class-link">MidiMessage</span>
plus an added timestamp public variable, <b>.tick</b>.  The MidiEvent
class also adds other support variables such as <b>.track</b> which
store the track number in the MIDI file that the MidiEvent occurs
in.

{% include docslot/docslot.html %}

&nbsp;

Public functions
----------------

{% include docslot/MidiEvent.html %}

&nbsp;

Functions inherited from <span class="class-link">MidiMessage</span>
---------------------------------------------------------------

{% include docslot/MidiMessage_other.html %}
{% include docslot/MidiMessage_meta.html %}



{% include docslot/keyboardCommands.html %}

