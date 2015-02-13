---
layout: default
title: C++ class for parsing Standard MIDI Files
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

<h2> Resources </h2>

<ul>
<li> <a href="tutorial">Tutorial</a> </li>

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

<ul>
<li>   <a href="class">Class documentation</a>

<script>
document.addEventListener("DOMContentLoaded", function(event) {
   var classes = document.querySelectorAll("ul.class-list > li");
   var i;
   for (i=0; i<classes.length; i++) {
      var target = classes[i].textContent.replace(/\s/g, '');
      var newcontent = "";
      newcontent += '<a href="class/' + target + '">';
      newcontent += target;
      newcontent += '</a>';
      classes[i].innerHTML = newcontent;
   }
});
</script>
<style>

ul.class-list {
	-webkit-column-count: 6;
	-moz-column-count: 6;
	column-count: 6;
	margin-left: 10px;
}
</style>

<ul class="class-list">
<li> MidiFile </li>
<li> MFEvent </li>
<li> Options </li>
</ul>

</li>

</ul>





