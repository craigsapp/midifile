## main Midifile GNU makefile for Linux/Cygwin and OS X on Intel computers.
##
## Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
## Creation Date: Sun Apr  3 00:44:44 PST 2005
## Last Modified: Sun Jun 21 12:17:38 PDT 2009
## Filename:      ...midifile/Makefile
##
## Description: This Makefile can create the Midifile library or 
##              example programs which use the Midifile library with 
##              linux/cygwin or OS X using g++ (gcc 2.7.2.1 or later).
##
## To run this makefile, type (without quotes) "make library", then
## "make examples".
##

MAKE = make

# targets which don't actually refer to files
.PHONY : src lib examples include bin clean info

###########################################################################
#                                                                         #
#                                                                         #

all: info library examples

info:
	@echo ""
	@echo This makefile will create either the Midifile library or will
	@echo compile the Midifile programs.  You may have to make the library
	@echo first if it does not exist. Type one of the following:
	@echo "   $(MAKE) library"
	@echo or
	@echo "   $(MAKE) examples"
	@echo ""
	@echo To compile a specific program called xxx, type:
	@echo "   $(MAKE) xxx"
	@echo ""
	@echo Typing \"make\" alone will compile both the library and all examples.
	@echo ""

library: 
	$(MAKE) -f Makefile.library

clean:
	$(MAKE) -f Makefile.library clean
	-rm -rf bin
	-rm -rf lib

examples:
programs:
	$(MAKE) -f Makefile.examples

%: 
	-mkdir -p bin
	@echo compiling file $@
	$(MAKE) -f Makefile.examples $@
	

#                                                                         #
#                                                                         #
###########################################################################



