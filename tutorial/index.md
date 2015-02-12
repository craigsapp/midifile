---
layout: default
title: programming tutorial
---

{% include docslot/docslot.html %}

<h2> Print a list of notes attacks in a MIDI file </h2>

The following example program will print a list of all note-ons
in the MIDI file.


``` cpp
{% include code/notelist.cpp %}
```

Functions used in this example:

<div class="docslot" id="MidiFile::read"><span class="docslot-long-title" style="padding-left:19px;">MidiFile::read</span><span class="short-desc"> &mdash; Read in a Standard MIDI File.</span></div>
<div class="docslot" id="MidiFile::absoluteTime"><span class="docslot-long-title" style="padding-left:19px;">MidiFile::absoluteTime</span><span class="short-desc"> &mdash; Convert event timestamps into Absolute time.</span></div>
<div class="docslot" id="MidiFile::joinTracks"><span class="docslot-long-title" style="padding-left:19px;">MidiFile::joinTracks</span><span class="short-desc"> &mdash; Merge all tracks into a single stream of events.</span></div>

