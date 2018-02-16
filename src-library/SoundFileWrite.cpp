//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Jan 19 10:38:26 PST 2002
// Last Modified: Sat Jan 19 10:38:29 PST 2002
// Filename:      SoundFileWrite.cpp
// Based On:      http://sig.sapp.org/src/sigSignal/SoundFileOut.cpp
// Syntax:        C++ 
// Copyright:     Copyright 2002 Craig Stuart Sapp
//

#include "SoundFileWrite.h"
#include "soundfilehelpers.h"

#include <stdlib.h>
#include <string.h>

#ifndef OLDCPP
   #include <iostream>
   using namespace std;
#else
   #include <iostream.h>
#endif



//////////////////////////////
//
// SoundFileWrite::SoundFileWrite --
//

SoundFileWrite::SoundFileWrite(const char* aFileName, SoundHeader& header) {
   filename = new char[strlen(aFileName)+1];
   strcpy(filename, aFileName);
   outSample = 0;
   #ifndef OLDCPP
      #ifdef VISUAL            /* for stupid LF-CR prevention in Windows */
         outFile.open(filename, ios::out | ios::binary);
      #else
         outFile.open(filename, ios::out);
      #endif
   #else
      #ifdef VISUAL            /* for stupid LF-CR prevention in Windows */
         outFile.open(filename, ios::out | ios::noreplace | ios::binary);
      #else
         outFile.open(filename, ios::out | ios::noreplace);
      #endif
   #endif
   if (!outFile.is_open()) {
      cerr << "Error: cannot write soundfile " << filename 
           << ". Perhaps the file already exists?" << endl;
      exit(1);
   }
   sndHeader = header;
   sndHeader.modifyFormat(aFileName);
   outFile << sndHeader;
   outFile.seekp(sndHeader.getDataByteOffset(), ios::beg);
   sampleCount = 0;
   determineWritingFunction();
}



//////////////////////////////
//
// SoundFileWrite::~SoundFileWrite -- close the sound file.
//


SoundFileWrite::~SoundFileWrite() {
   close();
}



//////////////////////////////
//
// SoundFileWrite::close -- write the size of the file in the header
//     and close the sound file.
//

void SoundFileWrite::close(void) {
   if (sndHeader.getSamples() != sampleCount/sndHeader.getChannels()) {
      switch (sndHeader.getOutputType()) {
         case TYPE_SND:
            // write the number of bytes in the data:
            outFile.seekp(8, ios::beg);   
            outFile.writeBigEndian((long)
               (sampleCount/sndHeader.getChannels()*sndHeader.getBlockAlign()));
            break;
         case TYPE_WAV_PCM:
            outFile.seekp(8, ios::beg);
            // write the number of bytes in the file minus a few:
            outFile.seekp(4, ios::beg);   
            outFile.writeLittleEndian((long)
               (36 + sampleCount/sndHeader.getChannels() * 
                  sndHeader.getBlockAlign()));
            // write the number of sound data bytes:
            outFile.seekp(40, ios::beg);
            outFile.writeLittleEndian((long)
               (sampleCount/sndHeader.getChannels()*sndHeader.getBlockAlign()));
            break;
         default:
           cerr << "Unknown output soundfile type: "
                << sndHeader.getOutputType() << endl;
           exit(1);
      }
   }
   outFile.close();
   sampleCount = 0;
}



//////////////////////////////
//
// SoundFileWrite::getFilename -- will never return a NULL pointer.
//

const char* SoundFileWrite::getFilename(void) {
   if (filename == NULL) {
      return "";
   } else {
      return filename;
   }
}



//////////////////////////////
//
// SoundFileWrite::writeSample16Bit --
//

void SoundFileWrite::writeSample16Bit(short sample) {
   switch (endianType) {
      case SIG_BIG_ENDIAN:    outFile.writeBigEndian(sample);    break;
      case SIG_LITTLE_ENDIAN: outFile.writeLittleEndian(sample); break;
   }
   sampleCount++;
}
     


//////////////////////////////
//
// SoundFileWrite::writeSampleDouble -- convert to a 16-bit value and
//    write to file.
//

void SoundFileWrite::writeSampleDouble(double sample) {
   writeSample(outFile, sample);
   sampleCount++;
}
     


///////////////////////////////////////////////////////////////////////////
//
// private functions:
//

//////////////////////////////
//
// SoundFileIn::determineWritingFunction --
//

void SoundFileWrite::determineWritingFunction(void) {
   switch (sndHeader.getOutputType()) {
      case TYPE_SND:                           // bigendian samples
         endianType = SIG_BIG_ENDIAN;
         switch (sndHeader.getNextFormat()) {
            case SND_FORMAT_MULAW_8:
               writeSample = soundWriteSample8M_B;
               break;
            case SND_FORMAT_LINEAR_8:
               writeSample = soundWriteSample8L_B;
               break;
            case SND_FORMAT_LINEAR_16:
               writeSample = soundWriteSample16L_B;
               break;
            case SND_FORMAT_LINEAR_24:
               writeSample = soundWriteSample24L_B;
               break;
            case SND_FORMAT_LINEAR_32:
               writeSample = soundWriteSample32L_B;
               break;
            case SND_FORMAT_FLOAT:
               writeSample = soundWriteSample32F_B;
               break;
            case SND_FORMAT_DOUBLE:
               writeSample = soundWriteSample64F_B;
               break;
            default:
               cerr << "Error: unknown output soundfile format: "
                    << sndHeader.getNextFormat() << endl;
               exit(1);
         }
         break;
      case TYPE_WAV_PCM:                      // little endian samples
         endianType = SIG_LITTLE_ENDIAN;
         switch (sndHeader.getNextFormat()) {
            case SND_FORMAT_MULAW_8:
               writeSample = soundWriteSample8M_L;
               break;
            case SND_FORMAT_LINEAR_8:
               writeSample = soundWriteSample8L_L;
               break;
            case SND_FORMAT_LINEAR_16:
               writeSample = soundWriteSample16L_L;
               break;
            case SND_FORMAT_LINEAR_24:
               writeSample = soundWriteSample24L_L;
               break;
            case SND_FORMAT_LINEAR_32:
               writeSample = soundWriteSample32L_L;
               break;
            case SND_FORMAT_FLOAT:
               writeSample = soundWriteSample32F_L;
               break;
            case SND_FORMAT_DOUBLE:
               writeSample = soundWriteSample64F_L;
               break;
            default:
               cerr << "Error: unknown output soundfile format: "
                    << sndHeader.getNextFormat() << endl;
               exit(1);
         }
         break;
      default:
         cerr << "Error: unknown soundfile type: "
              << sndHeader.getOutputType() << endl;
         exit(1);
   }
}


// md5sum: 06161b6117e89de8930c67188a6b9f9f SoundFileWrite.cpp [20050403]
