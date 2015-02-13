---
layout: default
title: programming tutorial
---

<script>

document.addEventListener("DOMContentLoaded", function(event) {
	var links = document.querySelectorAll('a');
	var matches = window.location.href.match(/#(.*)\s*$/);
	if (!matches) {
		return;
	}
	var target = matches[1];
	var anchor;
	var i;
	for (i=0; i<links.length; i++) {
		matches = links[i].outerHTML.match(/name="([^"]+)"/);
		if (matches) {
			anchor = matches[1];
			if (anchor === target) {
				var element = links[i].parentNode.nextSibling;
				if (element.tagName === 'DETAILS') {
					element.setAttribute('open', '');
					element.scrollIntoViewIfNeeded();
				}
			}
		} 
	}
});

</script>

The code in the following tutorials can be placed into the
`midifile/src-programs` directory.  If the program file is named
"myprogram.cpp", then type `make myprogram` in the `midifile`
directory to compile it.  The compiled program will be placed in
the `midifile/bin` directory.

{% include docslot/docslot.html %}

{% include tutorial/contents.html %}



