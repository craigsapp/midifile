---
layout: default
title: C++ library for parsing Standard MIDI Files
---

<h2> Downloading and Installing </h2>

Download the zip or tar file from the buttons at the top of the page
and unpack and type `make` in the base directory of the midifile 
package.  Or if you use git, download on the terminal with these commands:

``` bash
git pull https://github.com/craigsapp/midifile
cd midifile
make
```

<h2> <a href="tutorial">Tutorial</a> </h2>

<!-- TUTORIAL LIST ------------------------------------------------- -->

<script>
document.addEventListener("DOMContentLoaded", function(event) {
   var tutorials = document.querySelectorAll("ul.tutorial-list > li");
   var i;
   for (i=0; i<tutorials.length; i++) {
      var target = tutorials[i].textContent.replace(/\s/g, '');
      var newcontent = "";
      newcontent += '<a href="tutorial/#' + target + '">';
      newcontent += target;
      newcontent += '</a>';
      tutorials[i].innerHTML = newcontent;
   }
});
</script>

<style>
ul.tutorial-list {
	-webkit-column-count: 6;
	-moz-column-count: 6;
	column-count: 6;
	margin-left: 30px;
}
</style>

<ul class="tutorial-list">
<li> fixsize </li>
<li> noteattacks </li>
<li> offswitch </li>
</ul>


<!-- CLASS LIST ---------------------------------------------------- -->

<h2>   <a href="class">Class documentation</h2> 

<style>
ul.function-listing {
	-webkit-column-count: 3;
	-moz-column-count: 3;
	column-count: 3;
	margin-left: 30px;
}
</style>


<h4> <a href="class/MidiFile">MidiFile</a>:: Main class for parsing MIDI files</h4>

<ul class="function-listing">
<li> <a href="class/MidiFile#absoluteTime">absoluteTime</a> </li>
<li> <a href="class/MidiFile#addEvent">addEvent</a> </li>
<li> <a href="class/MidiFile#addMetaEvent">addMetaEvent</a> </li>
<li> <a href="class/MidiFile#addPitchBend">addPitchBend</a> </li>
<li> <a href="class/MidiFile#addTrack">addTrack</a> </li>
<li> <a href="class/MidiFile#allocateEvents">allocateEvents</a> </li>
<li> <a href="class/MidiFile#buildTimeMap">buildTimeMap</a> </li>
<li> <a href="class/MidiFile#clear">clear</a> </li>
<li> <a href="class/MidiFile#deleteTrack">deleteTrack</a> </li>
<li> <a href="class/MidiFile#deltaTime">deltaTime</a> </li>
<li> <a href="class/MidiFile#doTimeInSecondsAnalysis">doTimeInSecondsAnalysis</a> </li>
<li> <a href="class/MidiFile#extractMidiData">extractMidiData</a> </li>
<li> <a href="class/MidiFile#extractVlvTime">extractVlvTime</a> </li>
<li> <a href="class/MidiFile#getAbsoluteTickTime">getAbsoluteTickTime</a> </li>
<li> <a href="class/MidiFile#getEvent">getEvent</a> </li>
<li> <a href="class/MidiFile#getEventCount">getEventCount</a> </li>
<li> <a href="class/MidiFile#getFilename">getFilename</a> </li>
<li> <a href="class/MidiFile#getTicksPerQuarterNote">getTicksPerQuarterNote</a> </li>
<li> <a href="class/MidiFile#getTimeInSeconds">getTimeInSeconds</a> </li>
<li> <a href="class/MidiFile#getTrack">getTrack</a> </li>
<li> <a href="class/MidiFile#getTrackCount">getTrackCount</a> </li>
<li> <a href="class/MidiFile#getTrackState">getTrackState</a> </li>
<li> <a href="class/MidiFile#joinTracks">joinTracks</a> </li>
<li> <a href="class/MidiFile#linearTickInterpolationAtSecond">linearTickInterpolationAtSecond</a> </li>
<li> <a href="class/MidiFile#makeVLV">makeVLV</a> </li>
<li> <a href="class/MidiFile#mergeTracks">mergeTracks</a> </li>
<li> <a href="class/MidiFile#printHex">printHex</a> </li>
<li> <a href="class/MidiFile#read">read</a> </li>
<li> <a href="class/MidiFile#readByte">readByte</a> </li>
<li> <a href="class/MidiFile#readLittleEndian2Bytes">readLittleEndian2Bytes</a> </li>
<li> <a href="class/MidiFile#readLittleEndian4Bytes">readLittleEndian4Bytes</a> </li>
<li> <a href="class/MidiFile#secondsearch">secondsearch</a> </li>
<li> <a href="class/MidiFile#setFilename">setFilename</a> </li>
<li> <a href="class/MidiFile#setMilliseconds">setMilliseconds</a> </li>
<li> <a href="class/MidiFile#setTicksPerQuarterNote">setTicksPerQuarterNote</a> </li>
<li> <a href="class/MidiFile#sortTrack">sortTrack</a> </li>
<li> <a href="class/MidiFile#sortTracks">sortTracks</a> </li>
<li> <a href="class/MidiFile#splitTracks">splitTracks</a> </li>
<li> <a href="class/MidiFile#ticksearch">ticksearch</a> </li>
<li> <a href="class/MidiFile#timeState">timeState</a> </li>
<li> <a href="class/MidiFile#unpackVLV">unpackVLV</a> </li>
<li> <a href="class/MidiFile#write">write</a> </li>
<li> <a href="class/MidiFile#writeVLValue">writeVLValue</a> </li>
</ul>

<h4> <a href="class/MidiEventList">MidiEventList</a>:: MIDI file track data </h4>
<ul class="function-listing">
<li> <a href="class/MidiEventList#append">append</a> </li>
<li> <a href="class/MidiEventList#clear">clear</a> </li>
<li> <a href="class/MidiEventList#getSize">getSize</a> </li>
<li> <a href="class/MidiEventList#reserve">reserve</a> </li>
</ul>

<h4> <a href="class/MidiEvent">MidiEvent</a>:: MIDI message + timestamp </h4>
<ul class="function-listing">
</ul>

<h4> <a href="class/MidiMessage">MidiMessage</a>:: MIDI message bytes </h4>
<ul class="function-listing">
<li> <a href="class/MidiMessage#getChannelNibble">getChannelNibble</a> </li>
<li> <a href="class/MidiMessage#getCommandByte">getCommandByte</a> </li>
<li> <a href="class/MidiMessage#getCommandNibble">getCommandNibble</a> </li>
<li> <a href="class/MidiMessage#getSize">getSize</a> </li>
<li> <a href="class/MidiMessage#getTempoBPM">getTempoBPM</a> </li>
<li> <a href="class/MidiMessage#getTempoMicro">getTempoMicro</a> </li>
<li> <a href="class/MidiMessage#getTempoSPT">getTempoSPT</a> </li>
<li> <a href="class/MidiMessage#getTempoSeconds">getTempoSeconds</a> </li>
<li> <a href="class/MidiMessage#getTempoTPS">getTempoTPS</a> </li>
<li> <a href="class/MidiMessage#isAftertouch">isAftertouch</a> </li>
<li> <a href="class/MidiMessage#isController">isController</a> </li>
<li> <a href="class/MidiMessage#isMeta">isMeta</a> </li>
<li> <a href="class/MidiMessage#isNoteOff">isNoteOff</a> </li>
<li> <a href="class/MidiMessage#isNoteOn">isNoteOn</a> </li>
<li> <a href="class/MidiMessage#isPitchbend">isPitchbend</a> </li>
<li> <a href="class/MidiMessage#isPressure">isPressure</a> </li>
<li> <a href="class/MidiMessage#isTempo">isTempo</a> </li>
<li> <a href="class/MidiMessage#isTimbre">isTimbre</a> </li>
<li> <a href="class/MidiMessage#operatorEQUALS">operator=</a> </li>
<li> <a href="class/MidiMessage#setChannelNibble">setChannelNibble</a> </li>
<li> <a href="class/MidiMessage#setCommand">setCommand</a> </li>
<li> <a href="class/MidiMessage#setCommandByte">setCommandByte</a> </li>
<li> <a href="class/MidiMessage#setCommandNibble">setCommandNibble</a> </li>
<li> <a href="class/MidiMessage#setMetaTempo">setMetaTempo</a> </li>
<li> <a href="class/MidiMessage#setParameters">setParameters</a> </li>
<li> <a href="class/MidiMessage#setSize">setSize</a> </li>
<li> <a href="class/MidiMessage#setSizeToCommand">setSizeToCommand</a> </li>
</ul>

<h4> <a href="class/Options">Options</a>:: Command-line option parser</h4>

<ul class="function-listing">
<li> <a href="class/Options#appendOptions">appendOptions</a> </li>
<li> <a href="class/Options#argc">argc</a> </li>
<li> <a href="class/Options#argv">argv</a> </li>
<li> <a href="class/Options#define">define</a> </li>
<li> <a href="class/Options#getArg">getArg</a> </li>
<li> <a href="class/Options#getArgCount">getArgCount</a> </li>
<li> <a href="class/Options#getArgList">getArgList</a> </li>
<li> <a href="class/Options#getBoolean">getBoolean</a> </li>
<li> <a href="class/Options#getChar">getChar</a> </li>
<li> <a href="class/Options#getCommand">getCommand</a> </li>
<li> <a href="class/Options#getCommandLine">getCommandLine</a> </li>
<li> <a href="class/Options#getDouble">getDouble</a> </li>
<li> <a href="class/Options#getFlag">getFlag</a> </li>
<li> <a href="class/Options#getFloat">getFloat</a> </li>
<li> <a href="class/Options#getInt">getInt</a> </li>
<li> <a href="class/Options#getRegIndex">getRegIndex</a> </li>
<li> <a href="class/Options#getString">getString</a> </li>
<li> <a href="class/Options#isDefined">isDefined</a> </li>
<li> <a href="class/Options#optionQ">optionQ</a> </li>
<li> <a href="class/Options#optionsArg">optionsArg</a> </li>
<li> <a href="class/Options#printOptionBooleanState">printOptionBooleanState</a> </li>
<li> <a href="class/Options#printOptionList">printOptionList</a> </li>
<li> <a href="class/Options#printRegister">printRegister</a> </li>
<li> <a href="class/Options#process">process</a> </li>
<li> <a href="class/Options#setFlag">setFlag</a> </li>
<li> <a href="class/Options#setModified">setModified</a> </li>
<li> <a href="class/Options#setOptions">setOptions</a> </li>
<li> <a href="class/Options#storeOption">storeOption</a> </li>
<li> <a href="class/Options#xverify">xverify</a> </li>
</ul>


</li>
</ul>




