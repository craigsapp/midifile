//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Sep  8 11:28:37 PDT 2002
// Last Modified: Sun Sep  8 11:28:40 PDT 2002
// Filename:      soundfilehelpers.h
// Syntax:        C++ 
// Copyright:     Copyright 2002 Craig Stuart Sapp
//

#ifndef _SOUNDFILEHELPERS_H_INCLUDED
#define _SOUNDFILEHELPERS_H_INCLUDED

#include "FileIO.h"

short SampleToShortSFW(double sample);
char  SampleToCharSFW(double aSample);
long  SampleToLongSFW(double aSample);

// big endian samples
void  soundWriteSample8M_B      (FileIO& outFile, double aSample);
void  soundWriteSample8L_B      (FileIO& outFile, double aSample);
void  soundWriteSample16L_B     (FileIO& outFile, double aSample);
void  soundWriteSample24L_B     (FileIO& outFile, double aSample);
void  soundWriteSample32L_B     (FileIO& outFile, double aSample);
void  soundWriteSample32F_B     (FileIO& outFile, double aSample);
void  soundWriteSample64F_B     (FileIO& outFile, double aSample);

// little endian samples
void  soundWriteSample8M_L      (FileIO& outFile, double aSample);
void  soundWriteSample8L_L      (FileIO& outFile, double aSample);
void  soundWriteSample16L_L     (FileIO& outFile, double aSample);
void  soundWriteSample24L_L     (FileIO& outFile, double aSample);
void  soundWriteSample32L_L     (FileIO& outFile, double aSample);
void  soundWriteSample32F_L     (FileIO& outFile, double aSample);
void  soundWriteSample64F_L     (FileIO& outFile, double aSample);

#endif  /* _SOUNDFILEHELPERS_H_INCLUDED */

// md5sum: 7c520e40d7a5d716572a5767bd7b8969 soundfilehelpers.h [20050403]
