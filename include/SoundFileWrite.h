//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jan 20 20:23:19 PST 2002
// Last Modified: Wed Jan 23 20:23:29 PST 2002
// Filename:      SoundFileWrite.h
// Based On:      http://sig.sapp.org/include/sigSignal/SoundFileOut.h
// Syntax:        C++ 
// Copyright:     Copyright 2002 Craig Stuart Sapp
//

#ifndef _SOUNDFILEWRITE_H_INCLUDED
#define _SOUNDFILEWRITE_H_INCLUDED

#include "SoundHeader.h"

#ifndef OLDCPP
   #include <fstream>
   using namespace std;
#else
   #include <fstream.h>
#endif


class SoundFileWrite {
   public:
                   SoundFileWrite       (const char* aFileName, 
                                         SoundHeader& header);
                  ~SoundFileWrite       ();

      void         close                (void);
      const char*  getFilename          (void);
      void         writeSample16Bit     (short sample);
      void         writeSampleDouble    (double sample);
 
   protected:
      char        *filename;
      int          endianType;
      SoundHeader  sndHeader;
      FileIO       outFile;
      long         sampleCount;
      ushort       outSample;


      void         determineWritingFunction (void);
      void         (*writeSample)           (FileIO& outFile, double aSample);

};


#endif  /* _SOUNDFILEWRITE_H_INCLUDED */
// md5sum: 33c2475b3178760ecc2390a95051a1de SoundFileWrite.h [20050403]
