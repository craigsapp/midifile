---
layout: default
breadcrumbs: [['/doc', 'documentation']]
title: Class documentation
---

The Midifile library contains five classes:


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
	The primary class, responsible for reading and writing MIDI files.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiEventList">MidiEventList</a> </td> <td> 
	Responsible for managing MidiEvent lists for tracks within the MidiFile class.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiEvent">MidiEvent</a> </td> <td> 
	A MidiMessage plus a timestamp and other bookkeeping 
	variables for a MidiMessage.
   </td>
</tr>

<tr valign=top>
   <td> <a href="MidiMessage">MidiMessage</a> </td> <td> 
	A raw MIDI protocol message.  Inherited by the MidiEvent class.
   </td>
</tr>

<tr valign=top>
   <td> <a href="Options">Options</a> </td> <td> 
	An independent helper class for processing command-line options.
   </td>
</tr>

</table>

&nbsp;

<style>
ul.function-listing {
	-webkit-column-count: 3;
	-moz-column-count: 3;
	column-count: 3;
	margin-left: 30px;
}
</style>

<h2> <a href="MidiFile">MidiFile</a>:: </h2>

<ul class="function-listing">
<li> <a href=MidiFile#absoluteTime>absoluteTime</a> </li>
<li> <a href=MidiFile#addEvent>addEvent</a> </li>
<li> <a href=MidiFile#addMetaEvent>addMetaEvent</a> </li>
<li> <a href=MidiFile#addPitchBend>addPitchBend</a> </li>
<li> <a href=MidiFile#addTrack>addTrack</a> </li>
<li> <a href=MidiFile#allocateEvents>allocateEvents</a> </li>
<li> <a href=MidiFile#buildTimeMap>buildTimeMap</a> </li>
<li> <a href=MidiFile#clear>clear</a> </li>
<li> <a href=MidiFile#deleteTrack>deleteTrack</a> </li>
<li> <a href=MidiFile#deltaTime>deltaTime</a> </li>
<li> <a href=MidiFile#doTimeInSecondsAnalysis>doTimeInSecondsAnalysis</a> </li>
<li> <a href=MidiFile#extractMidiData>extractMidiData</a> </li>
<li> <a href=MidiFile#extractVlvTime>extractVlvTime</a> </li>
<li> <a href=MidiFile#getAbsoluteTickTime>getAbsoluteTickTime</a> </li>
<li> <a href=MidiFile#getEvent>getEvent</a> </li>
<li> <a href=MidiFile#getEventCount>getEventCount</a> </li>
<li> <a href=MidiFile#getFilename>getFilename</a> </li>
<li> <a href=MidiFile#getTicksPerQuarterNote>getTicksPerQuarterNote</a> </li>
<li> <a href=MidiFile#getTimeInSeconds>getTimeInSeconds</a> </li>
<li> <a href=MidiFile#getTrack>getTrack</a> </li>
<li> <a href=MidiFile#getTrackCount>getTrackCount</a> </li>
<li> <a href=MidiFile#getTrackState>getTrackState</a> </li>
<li> <a href=MidiFile#joinTracks>joinTracks</a> </li>
<li> <a href=MidiFile#linearTickInterpolationAtSecond>linearTickInterpolationAtSecond</a> </li>
<li> <a href=MidiFile#makeVLV>makeVLV</a> </li>
<li> <a href=MidiFile#mergeTracks>mergeTracks</a> </li>
<li> <a href=MidiFile#printHex>printHex</a> </li>
<li> <a href=MidiFile#read>read</a> </li>
<li> <a href=MidiFile#readByte>readByte</a> </li>
<li> <a href=MidiFile#readLittleEndian2Bytes>readLittleEndian2Bytes</a> </li>
<li> <a href=MidiFile#readLittleEndian4Bytes>readLittleEndian4Bytes</a> </li>
<li> <a href=MidiFile#secondsearch>secondsearch</a> </li>
<li> <a href=MidiFile#setFilename>setFilename</a> </li>
<li> <a href=MidiFile#setMilliseconds>setMilliseconds</a> </li>
<li> <a href=MidiFile#setTicksPerQuarterNote>setTicksPerQuarterNote</a> </li>
<li> <a href=MidiFile#sortTrack>sortTrack</a> </li>
<li> <a href=MidiFile#sortTracks>sortTracks</a> </li>
<li> <a href=MidiFile#splitTracks>splitTracks</a> </li>
<li> <a href=MidiFile#ticksearch>ticksearch</a> </li>
<li> <a href=MidiFile#timeState>timeState</a> </li>
<li> <a href=MidiFile#unpackVLV>unpackVLV</a> </li>
<li> <a href=MidiFile#write>write</a> </li>
<li> <a href=MidiFile#writeVLValue>writeVLValue</a> </li>
</ul>

<h2> <a href="MidiEventList">MidiEventList</a>:: </h2>
<ul class="function-listing">
<li> <a href="MidiEventList#append">append</a> </li>
<li> <a href="MidiEventList#clear">clear</a> </li>
<li> <a href="MidiEventList#getSize">getSize</a> </li>
<li> <a href="MidiEventList#reserve">reserve</a> </li>
</ul>

<h2> <a href="MidiEvent">MidiEvent</a>:: (also inherits from <a href="MidiMessage">MidiMessage</a>) </h2>

<ul class="function-listing">
<li> <a href="MidiEvent#operatorEQUALS">operator=</a> </li>
</ul>

<h2> <a href="MidiMessage">MidiMessage</a>:: (also inherits from 
<a href="http://www.cplusplus.com/reference/vector/vector">vector&lt;uchar&gt;</a>) </h2>

<ul class="function-listing">
<li> <a href="MidiMessage#getChannelNibble">getChannelNibble</a> </li>
<li> <a href="MidiMessage#getCommandByte">getCommandByte</a> </li>
<li> <a href="MidiMessage#getCommandNibble">getCommandNibble</a> </li>
<li> <a href="MidiMessage#getSize">getSize</a> </li>
<li> <a href="MidiMessage#getTempoBPM">getTempoBPM</a> </li>
<li> <a href="MidiMessage#getTempoMicro">getTempoMicro</a> </li>
<li> <a href="MidiMessage#getTempoSPT">getTempoSPT</a> </li>
<li> <a href="MidiMessage#getTempoSeconds">getTempoSeconds</a> </li>
<li> <a href="MidiMessage#getTempoTPS">getTempoTPS</a> </li>
<li> <a href="MidiMessage#isAftertouch">isAftertouch</a> </li>
<li> <a href="MidiMessage#isController">isController</a> </li>
<li> <a href="MidiMessage#isMeta">isMeta</a> </li>
<li> <a href="MidiMessage#isNoteOff">isNoteOff</a> </li>
<li> <a href="MidiMessage#isNoteOn">isNoteOn</a> </li>
<li> <a href="MidiMessage#isPitchbend">isPitchbend</a> </li>
<li> <a href="MidiMessage#isPressure">isPressure</a> </li>
<li> <a href="MidiMessage#isTempo">isTempo</a> </li>
<li> <a href="MidiMessage#isTimbre">isTimbre</a> </li>
<li> <a href="MidiMessage#operatorEQUALS">operator=</a> </li>
<li> <a href="MidiMessage#setChannelNibble">setChannelNibble</a> </li>
<li> <a href="MidiMessage#setCommand">setCommand</a> </li>
<li> <a href="MidiMessage#setCommandByte">setCommandByte</a> </li>
<li> <a href="MidiMessage#setCommandNibble">setCommandNibble</a> </li>
<li> <a href="MidiMessage#setMetaTempo">setMetaTempo</a> </li>
<li> <a href="MidiMessage#setParameters">setParameters</a> </li>
<li> <a href="MidiMessage#setSize">setSize</a> </li>
<li> <a href="MidiMessage#setSizeToCommand">setSizeToCommand</a> </li>
</ul>

<h2> <a href="Options">Options</a>:: </h2>

<ul class="function-listing">
<li> <a href=Options#appendOptions>appendOptions</a> </li>
<li> <a href=Options#argc>argc</a> </li>
<li> <a href=Options#argv>argv</a> </li>
<li> <a href=Options#define>define</a> </li>
<li> <a href=Options#getArg>getArg</a> </li>
<li> <a href=Options#getArgCount>getArgCount</a> </li>
<li> <a href=Options#getArgList>getArgList</a> </li>
<li> <a href=Options#getBoolean>getBoolean</a> </li>
<li> <a href=Options#getChar>getChar</a> </li>
<li> <a href=Options#getCommand>getCommand</a> </li>
<li> <a href=Options#getCommandLine>getCommandLine</a> </li>
<li> <a href=Options#getDouble>getDouble</a> </li>
<li> <a href=Options#getFlag>getFlag</a> </li>
<li> <a href=Options#getFloat>getFloat</a> </li>
<li> <a href=Options#getInt>getInt</a> </li>
<li> <a href=Options#getRegIndex>getRegIndex</a> </li>
<li> <a href=Options#getString>getString</a> </li>
<li> <a href=Options#isDefined>isDefined</a> </li>
<li> <a href=Options#optionQ>optionQ</a> </li>
<li> <a href=Options#optionsArg>optionsArg</a> </li>
<li> <a href=Options#printOptionBooleanState>printOptionBooleanState</a> </li>
<li> <a href=Options#printOptionList>printOptionList</a> </li>
<li> <a href=Options#printRegister>printRegister</a> </li>
<li> <a href=Options#process>process</a> </li>
<li> <a href=Options#setFlag>setFlag</a> </li>
<li> <a href=Options#setModified>setModified</a> </li>
<li> <a href=Options#setOptions>setOptions</a> </li>
<li> <a href=Options#storeOption>storeOption</a> </li>
<li> <a href=Options#xverify>xverify</a> </li>
</ul>



