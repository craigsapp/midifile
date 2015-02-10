##
## Main GNU makefile for Midifile library.
##
## Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
## Creation Date: Sun Apr  3 00:44:44 PST 2005
## Last Modified: Mon Feb  9 12:59:54 PST 2015
## Filename:      midifile/Makefile
## Website:       http://midifile.sapp.org
## Syntax:        GNU Makefile
## Description:   This Makefile can create the Midifile library and/or
##                example programs which use the Midifile library.
##
## To run this makefile, type:
##     make library
## then:
##     make programs
## Or type:
##     make
## to compile both the library and the programs at the same time.
##



##############################
##
## Targets:
##

# targets which don't actually refer to files
.PHONY : all info library examples programs bin options clean lib


all: info library programs lib


info:
	@echo ""
	@echo This makefile will compile the Midifile library and/or
	@echo the Midifile programs.  You may have to make the library
	@echo first if compiling the programs. Type one of the following:
	@echo "   make library"
	@echo or
	@echo "   make programs"
	@echo ""
	@echo To compile a specific program called xxx, type:
	@echo "   make xxx"
	@echo ""
	@echo Typing \"make\" alone will compile both the library and all programs.
	@echo ""


lib: library
library:
	$(MAKE) -f Makefile.library


clean:
	$(MAKE) -f Makefile.library clean
	-rm -rf bin
	-rm -rf lib


bin:      programs
examples: programs
programs:
	$(MAKE) -f Makefile.programs


options:
# The Options class is borrowed from optionlib:
#     https://github.com/craigsapp/optionlib
# This code downloads the most recent version of the Option class, and the
# Option class should not be modified within this library.  The Option
# class is only needed by some of the example programs.  It is not 
# necessary if you only want to use the compiled MidiFile library.
ifneq ($(shell which wget),)
	wget https://raw.githubusercontent.com/craigsapp/optionlib/master/src/Options.cpp -O src/Options.cpp
	wget https://raw.githubusercontent.com/craigsapp/optionlib/master/include/Options.h -O include/Options.h
else ifneq ($(shell which curl),)
	curl https://raw.githubusercontent.com/craigsapp/optionlib/master/src/Options.cpp -o src/Options.cpp
	curl https://raw.githubusercontent.com/craigsapp/optionlib/master/include/Options.h -O include/Options.h
endif



##############################
##
## Default target: compile a particular program:
##

%:
	@-mkdir -p bin
	@echo compiling file $@
	$(MAKE) -f Makefile.programs $@


