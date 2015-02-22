#
# Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
# Creation Date: Wed Feb 18 23:06:16 PST 2015
# Last Modified: Wed Feb 18 23:06:18 PST 2015
# Filename:      scorelib/Makefile
# Syntax:        GNU Makefile
#
# Description:   Do preparation of website files.
#

.PHONY: class

all: class

class:
	(cd class && $(MAKE))
	(cd _includes/class && $(MAKE))
	(cd _includes/tutorial && $(MAKE))
	(cd _includes/docslot && $(MAKE))

