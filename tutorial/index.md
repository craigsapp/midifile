---
layout: default
title: programming tutorial
vim: ts=3 hlsearch
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

The code examples in the following tutorials can be placed into the
`midifile/src-programs` directory to compile them.  For example if
the program is named "src-programs/myprogram.cpp", then type `make
myprogram` in the `midifile` base directory to compile it.  The compiled
program will be named in the `myprogram` in the `midifile/bin` directory.

{% include docslot/docslot.html %}

{% include tutorial/contents.html %}



