//
// Copyright 1997 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Dec  1 18:16:57 GMT-0800 1997
// Last Modified: Mon Dec  1 18:16:57 GMT-0800 1997
// Filename:      ...sig/maint/code/misc/sigConfiguration.h
// Web Address:   http://sig.sapp.org/include/sig/sigConfiguration.h
// Syntax:        C 
//
// Description:  This file defines flags for different code setups: 
//	         You specify the OS in a compiler flag and this file takes care
//	         of the rest of the defines for that OS.  This file should
//         	 automatically be included in any file which uses any of the 
//	         define flags below.
//
// OS setup defines to use in compiling files:
//
// -DLINUX:  Linux running on intel computers
// -DNEXTI:  NeXT OS on Intel Hardware
// -DNEXTM:  NeXT OS on Motorola Hardware
// -DSUN:    Sun SPARCstations
// -DVISUAL: Windows 95/NT using Microsoft Visual C++ 5.0
// -DHPUX:   Hewlett-Packard Unix Workstations.
// -DIRIX:   SGI computers with IRIX OS.
//
//
// Various options that can be defined for each OS.  These
// defines may be mixed and matched in different OSes:
//
// -DOTHEREND: If the computer is little-endian, then this
//	define switches the byte ordering behavior for writing/reading
//	soundfiles.  Intel computers need this define, most others
//	will not.need this define.
//
// -DSHORTRAND: Indicates that the rand() function generates
//	numbers between 0 and 0x7fff.  The default without this
//	option is a range between 0 and 0x7fffffff.
//
// -DINTEL: Indicates that the computer hardware uses an
//	intel x86 CPU.  Not used for anything right now except to
//	define the endian flag (OTHEREND).
//
// -DMOTOROLA: Indicates that the computer hardware uses a
//      Motorola 68k CPU.  Not used for anything right now.
//
//

#ifndef _SIGCONFIGURATION_H_INCLUDED
#define _SIGCONFIGURATION_H_INCLUDED


#ifdef LINUX
   #define INTEL
#endif


#ifdef NEXTI
   #define INTEL
#endif


#ifdef NEXT
   #define MOTOROLA
#endif


#ifdef VISUAL
   #define INTEL
#endif

#ifdef OSXOLD
   #define MOTOROLA
#endif

#ifdef OSXPC
   #define INTEL
#endif


#ifdef SUN
   #define SHORTRAND
#endif


#ifdef HPUX
   #define SHORTRAND
#endif


#ifdef IRIX
   #define SHORTRAND
#endif


// These defines must come after the previous defines:


#ifdef INTEL
   #define OTHEREND
#endif



#endif  /* _SIGCONFIGURATION_H_INCLUDED */



// md5sum: 32f74a7c264b158b83ff38db1ea885f8 sigConfiguration.h [20030102]
