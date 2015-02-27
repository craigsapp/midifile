---
layout: default
title: programming tutorials
vim: ts=3 hlsearch
breadcrumbs: [['/','home'], 
		['/doc', 'documentation']
	]
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
<tt>midifile/src-programs</tt> directory to compile them.  For example if
the program is named "src-programs/myprogram.cpp", then type <tt>make
myprogram</tt> in the <tt>midifile</tt> base directory to compile it.  The compiled
program will be named in the <tt>myprogram</tt> in the <tt>midifile/bin</tt> directory.

{% include docslot/docslot.html %}

{% include tutorial/contents.html %}



