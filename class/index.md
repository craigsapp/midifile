---
layout: default
breadcrumbs: [['/doc', 'documentation']]
title: Class documentation
---

The Midifile library contains six classes:


<style>

table.classlist, 
table.classlist tr,
table.classlist td {
   padding: 0;
   border: 0;
}

table.classlist tr td:nth-child(1) {
   text-align: right;
   font-weight: 400;
   width: 140px;
}

table.classlist tr td:nth-child(1)::after {
   content: ":";
}

table.classlist tr td:nth-child(2) {
   text-align: left;
   padding-left: 10px;
}

</style>

<table class="classlist" cellpadding="0" cellspacing="0">

<tr valign=top>
   <td> <a href="MidiFile">MidiFile</a> </td> <td> 
	Primary class, responsible for reading and writing MIDI files.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiEventList">MidiEventList</a> </td> <td> 
	Responsible for managing MidiEvent lists for tracks within the MidiFile class.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiEvent">MidiEvent</a> </td> <td> 
	MidiMessage plus a timestamp and other bookkeeping 
	variables.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiMessage">MidiMessage</a> </td> <td> 
	Raw MIDI protocol message.  Inherited by the MidiEvent class.
   </td>
</tr>

<tr valign=top>
   <td> <a href="Binasc">Binasc</a> </td> <td> 
	MidiFile Support class for reading/writing
	ASCII byte codes representing MIDI files.
   </td>
</tr>

<tr valign=top>
   <td> <a href="Options">Options</a> </td> <td> 
	Independent helper class for processing command-line options.
   </td>
</tr>

</table>

&nbsp;


{% include class/classlist.html %}


