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

<script>
window.addEventListener('load', function(event) {
   var list = document.querySelectorAll('a');
   for (var i=0; i<list.length; i++) {
      var attrib = list[i].getAttribute('href');
      if (attrib.match(/^Binasc/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      } else if (attrib.match(/^Options/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      } else if (attrib.match(/^MidiMessage/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      } else if (attrib.match(/^MidiEvent/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      } else if (attrib.match(/^MidiEventList/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      } else if (attrib.match(/^MidiFile/)) {
         list[i].setAttribute('href', 'class/' + attrib);
      }
   }
});
</script>

{% include class/classlist.html %}


