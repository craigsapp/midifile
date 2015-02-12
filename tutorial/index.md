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

{% include docslot/docslot.html %}

{% include tutorial/contents.html %}



