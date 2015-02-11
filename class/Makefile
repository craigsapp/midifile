#
# Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
# Creation Date: Sun Feb  8 23:56:35 PST 2015
# Last Modified: Sun Feb  8 23:56:37 PST 2015
# Filename:      scorelib/class/Makefile
# Syntax:        GNU Makefile
#
# Description:   Adds source-code lines for functions in ATON files for
#                class documentation.
#

all: lines

lines:
	@for i in */Makefile;						\
	do								\
		DIRNAME=`echo $$i | sed 's/.Makefile//'`;		\
		(cd $$DIRNAME; $(MAKE) lines);				\
	done


