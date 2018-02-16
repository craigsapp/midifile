//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Copyright:     Copyright 2002 Craig Stuart Sapp
// Creation Date: Fri Jan 18 22:25:27 PST 2002
// Last Modified: Wed Jan 23 20:24:34 PST 2002
// Last Modified: Sat Feb  2 12:22:17 PST 2002 (fixed looping and goto)
// Filename:      SoundFileRead.cpp
// Based On:      http://sig.sapp.org/src/sigSignal/SoundFileIn.cpp
// Syntax:        C++ 
//

#include "SoundFileRead.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
using namespace std;


//////////////////////////////
//
// SoundFileRead::SoundFileRead --
//    defaults: aFileName = "", start = -1, dur = -1
//

SoundFileRead::SoundFileRead(const char* aFileName, double start, double dur) {
   filename = NULL;
   curIndex = 0;
   begIndex = 0;
   endIndex = 0;
   loopingQ = 0;
   endianType = 0;
   
   if (strlen(aFileName) > 0) {
      setFile(aFileName, start, dur);
      // set the size of the current array to the number of channels
   } else {
      current.setSize(4);
      current.allowGrowth(1);
      current.zero();
   }
}



//////////////////////////////
//
// SoundFileRead::~SoundFileRead --  What to do when destroying a
//     SoundFileRead object.
//

SoundFileRead::~SoundFileRead() { 
   if (filename != NULL) {
      delete [] filename;
      filename = NULL;
   }
}



/////////////////////////////
//
// SoundFileRead::getCurrentSample16Bit -- return a 16 bit
//     version of the current channel sample.
//     default value: channel = 0
//

int SoundFileRead::getCurrentSample16Bit(int channel) {
   return (int)((long)0x8000 * getCurrentSampleDouble(channel)); 
   // rounding necessary?
}



//////////////////////////////
//
// SoundFileRead::getNextSample16Bit -- increment the current sample array 
//     and return the requested channel value.  Returns
//     default value: channel = 0
//

int SoundFileRead::getNextSample16Bit(int channel) {
   incrementSample();
   return getCurrentSample16Bit(channel);
}



/////////////////////////////
//
// SoundFileRead::getCurrentSample24Bit -- return a 24 bit
//     version of the current channel sample.
//     default value: channel = 0
//

int SoundFileRead::getCurrentSample24Bit(int channel) {
   return (int)((long)0x800000 * getCurrentSampleDouble(channel)); 
}



//////////////////////////////
//
// SoundFileRead::getNextSample24Bit -- increment the current sample array 
//     and return the requested channel value.  Returns
//     default value: channel = 0
//

int SoundFileRead::getNextSample24Bit(int channel) {
   incrementSample();
   return getCurrentSample24Bit(channel);
}



/////////////////////////////
//
// SoundFileRead::getCurrentSampleDouble -- return a double-precision
//     floating-point version of the current channel sample.
//     default value: channel = 0
//

double SoundFileRead::getCurrentSampleDouble(int channel) {
   return current[channel];
}

/////////////////////////////
//
// SoundFileRead::getCurrentMixDouble -- return a double-precision
//     floating-point version of a mix of all the channels
//


double SoundFileRead::getCurrentMixDouble(bool doAverage) const {
	double accum = 0.f;
	for (int c = 0; c < channels; ++c) {
		accum += current[c];
	}
	if (doAverage) {
		accum /= channels;
	}
	return accum;
}


//////////////////////////////
//
// SoundFileRead::getNextSampleDouble -- increment the current sample 
//     array and return the requested channel value.  Returns
//     default value: channel = 0
//

double SoundFileRead::getNextSampleDouble(int channel) {
   incrementSample();
   return getCurrentSampleDouble(channel);
}



//////////////////////////////
//
// SoundFileRead::getCurrentSampleIndex -- return the sample index of
//     the current sample data.
//

int SoundFileRead::getCurrentSampleIndex(void) {
   return curIndex;
}



//////////////////////////////
//
// SoundFileRead::incrementSample -- read the next sample set and
//     store in current array.
//

void SoundFileRead::incrementSample(void) {
   curIndex++;

   if (curIndex > endIndex && loopingQ) {
      gotoSample(begIndex);
      return;
   } else if (curIndex < 0 || curIndex > endIndex) {
      for (int i=0; i<getChannels(); i++) {
         current[i] = 0;
      }
      return;
   }

   fillCurrentArray();
}



//////////////////////////////
//
// SoundFileRead::getFilename --
//

const char* SoundFileRead::getFilename(void) {
   if (filename == NULL) {
      return "";
   } else {
      return filename;
   }
}



//////////////////////////////
//
// SoundFileRead::setStartAndDuration --
//

void SoundFileRead::setStartAndDuration(double start, double dur) {
   // determine the beginning and ending samples according to start and dur
   int maxIndex = getSamples() - 1; 
   if (maxIndex < 0) {
      cerr << "Error: sound file " << filename << " has no data!" << endl;
      exit(1);
   }
   if (start >= 0) {
      begIndex = (long)(start * getSrate());
   } else {
      begIndex = 0;
   }
   if (begIndex > maxIndex) begIndex = 0;
   if (dur >= 0) {
      endIndex = begIndex + (long)(dur * getSrate());
   } else {
      endIndex = maxIndex;
   }
   if (endIndex > maxIndex) {
      endIndex = maxIndex;
   }
 
}



//////////////////////////////
//
// SoundFileRead::reset --  Go to the starting point in the file,
//     either the start of the file, or the start specified in
//     the setFile() function.
//     default: sampleIndex = -1
//

void SoundFileRead::reset(int sampleIndex) {
   if (sampleIndex < 0) {
      gotoSample(begIndex);
   } else {
      gotoSample(sampleIndex);
   }
}



//////////////////////////////
//
// SoundFileRead::gotoSample -- Move to the current sample position
//     in the soundfile and start reading from that point.
//

void SoundFileRead::gotoSample(int sampleIndex) {
   int offset = 0;
   if (sampleIndex < 0) {
      offset = getDataByteOffset();
      sndfile.seekg(offset); // used to be: sndfile.seekp(offset);
      sampleIndex = 0; 
   } else {
      offset = (int)(getDataByteOffset() +
         (sampleIndex + begIndex) * getChannels() *
         getBitsPerSample() / 8.0);
      sndfile.seekg(offset);  // used to be: sndfile.seekp(offset);
   }

   curIndex = sampleIndex;
   fillCurrentArray();
}



//////////////////////////////
//
// SoundFileRead::setFile -- start reading a soundfile at the current
//     start time and for the given duration.
//

void SoundFileRead::setFile(const char* aFileName, double start, double dur) {
   if (filename != NULL) delete [] filename;
   filename = new char[strlen(aFileName)+1];
   strcpy(filename, aFileName);

   setHeader(filename);

   if (sndfile.is_open()) sndfile.close();
   sndfile.open(filename, ios::in | ios::binary);
   if(!sndfile.is_open()) {
      cerr << "Error: sound file " << filename 
           << " could not be opened!" << endl;
      exit(1);
   }

   setStartAndDuration(start, dur);
   determineReadingFunction();

   // set the file to the current sample
   gotoSample(begIndex); 
}



//////////////////////////////
//
// SoundFileRead::setLoopOption -- turn looping on or off
// SoundFileRead::setLoopOn     -- turn looping on 
// SoundFileRead::setLoopOff    -- turn looping off
//

void SoundFileRead::setLoopOption(int mode) {
   if (mode == 0) {
      loopingQ = 0;
   } else {
      loopingQ = 1;
   }
}


void SoundFileRead::setLoopOn(void)  { setLoopOption(1); }
void SoundFileRead::setLoopOff(void) { setLoopOption(0); }



///////////////////////////////////////////////////////////////////////////
//
// private functions
//

// The following functions are used by the readSample function.
// These functions have to be outside of the SoundFileRead class
// because Microsoft's Visual C++ 5.0 compiler cannot handle pointers
// to class member functions.

// big endian samples
double         soundReadSample8M_B         (FileIO& soundFile);
double         soundReadSample8L_B         (FileIO& soundFile);
double         soundReadSample16L_B        (FileIO& soundFile);
double         soundReadSample24L_B        (FileIO& soundFile);
double         soundReadSample32L_B        (FileIO& soundFile);
double         soundReadSample32F_B        (FileIO& soundFile);
double         soundReadSample64F_B        (FileIO& soundFile);

// little endian samples
double        soundReadSample8M_L          (FileIO& soundFile);
double        soundReadSample8L_L          (FileIO& soundFile);
double        soundReadSample16L_L         (FileIO& soundFile);
double        soundReadSample24L_L         (FileIO& soundFile);
double        soundReadSample32L_L         (FileIO& soundFile);
double        soundReadSample32F_L         (FileIO& soundFile);
double        soundReadSample64F_L         (FileIO& soundFile);




//////////////////////////////
//
// SoundFileRead::determineSampleReadingFunction --
//

void SoundFileRead::determineReadingFunction(void) {

   switch (getInputType()) {
      case TYPE_SND:                        // big endian samples
         endianType = SIG_BIG_ENDIAN;
         switch (getNextFormat()) {
            case SND_FORMAT_MULAW_8:
               readSample = soundReadSample8M_B;
               break;
            case SND_FORMAT_LINEAR_8:
               readSample = soundReadSample8L_B;
               break;
            case SND_FORMAT_LINEAR_16:
               readSample = soundReadSample16L_B;
               break;
            case SND_FORMAT_LINEAR_24:
               readSample = soundReadSample24L_B;
               break;
            case SND_FORMAT_LINEAR_32:
               readSample = soundReadSample32L_B;
               break;
            case SND_FORMAT_FLOAT:
               readSample = soundReadSample32F_B;
               break;
            case SND_FORMAT_DOUBLE:
               readSample = soundReadSample64F_B;
               break;
            default:
               cerr << "Error: unknown input soundfile format: " 
                    << getNextFormat() << endl;
               exit(1);
         }
         break;
      case TYPE_WAV_PCM:                    // little endian samples
         endianType = SIG_LITTLE_ENDIAN;
         switch (getNextFormat()) {
            case SND_FORMAT_MULAW_8:
               readSample = soundReadSample8M_L;
               break;
            case SND_FORMAT_LINEAR_8:
               readSample = soundReadSample8L_L;
               break;
            case SND_FORMAT_LINEAR_16:
               readSample = soundReadSample16L_L;
               break;
            case SND_FORMAT_LINEAR_24:
               readSample = soundReadSample24L_L;
               break;
            case SND_FORMAT_LINEAR_32:
               readSample = soundReadSample32L_L;
               break;
            case SND_FORMAT_FLOAT:
               readSample = soundReadSample32F_L;
               break;
            case SND_FORMAT_DOUBLE:
               readSample = soundReadSample64F_L;
               break;
            default:
               cerr << "Error: unknown input soundfile format: " 
                    << getNextFormat() << endl;
               exit(1);
         }
         break;
      default: ;
         // the soundfile name is probably a dummy, and will not be used
         // cerr << "Error: unknown soundfile type: "
         //      << getInputType() << endl;
         // exit(1);
   }
}



//////////////////////////////
//
// SoundFileRead::fillCurrentArray -- read the next set of samples
//     from the soundfile and store in the current array.
//

void SoundFileRead::fillCurrentArray(void) {
   for (int i=0; i<getChannels(); i++) {
      current[i] = readSample(sndfile);
   }
}



///////////////////////////////////////////////////////////////////////////
//
// Helper functions which are intended to be private
// These functions have to be outside of the SoundFileRead class
// because Microsoft's Visual C++ 5.0 compiler cannot handle pointers
// to class member functions.
//



//////////////////////////////
//
// mulaw8ToLinear16 -- converts 8-bit mulaw to 
//	16-bit linear quantization
//

short mulaw8ToLinear16(uchar index) {

   static const short MuLaw[256] = {
   // negative values
    (short)0x8284, (short)0x8684, (short)0x8a84, (short)0x8e84, 
    (short)0x9284, (short)0x9684, (short)0x9a84, (short)0x9e84, 
    (short)0xa284, (short)0xa684, (short)0xaa84, (short)0xae84, 
    (short)0xb284, (short)0xb684, (short)0xba84, (short)0xbe84, 
    (short)0xc184, (short)0xc384, (short)0xc584, (short)0xc784, 
    (short)0xc984, (short)0xcb84, (short)0xcd84, (short)0xcf84, 
    (short)0xd184, (short)0xd384, (short)0xd584, (short)0xd784, 
    (short)0xd984, (short)0xdb84, (short)0xdd84, (short)0xdf84, 
    (short)0xe104, (short)0xe204, (short)0xe304, (short)0xe404, 
    (short)0xe504, (short)0xe604, (short)0xe704, (short)0xe804, 
    (short)0xe904, (short)0xea04, (short)0xeb04, (short)0xec04, 
    (short)0xed04, (short)0xee04, (short)0xef04, (short)0xf004, 
    (short)0xf0c4, (short)0xf144, (short)0xf1c4, (short)0xf244, 
    (short)0xf2c4, (short)0xf344, (short)0xf3c4, (short)0xf444, 
    (short)0xf4c4, (short)0xf544, (short)0xf5c4, (short)0xf644, 
    (short)0xf6c4, (short)0xf744, (short)0xf7c4, (short)0xf844, 
    (short)0xf8a4, (short)0xf8e4, (short)0xf924, (short)0xf964, 
    (short)0xf9a4, (short)0xf9e4, (short)0xfa24, (short)0xfa64, 
    (short)0xfaa4, (short)0xfae4, (short)0xfb24, (short)0xfb64, 
    (short)0xfba4, (short)0xfbe4, (short)0xfc24, (short)0xfc64, 
    (short)0xfc94, (short)0xfcb4, (short)0xfcd4, (short)0xfcf4, 
    (short)0xfd14, (short)0xfd34, (short)0xfd54, (short)0xfd74, 
    (short)0xfd94, (short)0xfdb4, (short)0xfdd4, (short)0xfdf4, 
    (short)0xfe14, (short)0xfe34, (short)0xfe54, (short)0xfe74, 
    (short)0xfe8c, (short)0xfe9c, (short)0xfeac, (short)0xfebc, 
    (short)0xfecc, (short)0xfedc, (short)0xfeec, (short)0xfefc, 
    (short)0xff0c, (short)0xff1c, (short)0xff2c, (short)0xff3c, 
    (short)0xff4c, (short)0xff5c, (short)0xff6c, (short)0xff7c, 
    (short)0xff88, (short)0xff90, (short)0xff98, (short)0xffa0, 
    (short)0xffa8, (short)0xffb0, (short)0xffb8, (short)0xffc0, 
    (short)0xffc8, (short)0xffd0, (short)0xffd8, (short)0xffe0, 
    (short)0xffe8, (short)0xfff0, (short)0xfff8, (short)0x0000, 
   // positive values
    (short)0x7d7c, (short)0x797c, (short)0x757c, (short)0x717c, 
    (short)0x6d7c, (short)0x697c, (short)0x657c, (short)0x617c, 
    (short)0x5d7c, (short)0x597c, (short)0x557c, (short)0x517c, 
    (short)0x4d7c, (short)0x497c, (short)0x457c, (short)0x417c, 
    (short)0x3e7c, (short)0x3c7c, (short)0x3a7c, (short)0x387c, 
    (short)0x367c, (short)0x347c, (short)0x327c, (short)0x307c, 
    (short)0x2e7c, (short)0x2c7c, (short)0x2a7c, (short)0x287c, 
    (short)0x267c, (short)0x247c, (short)0x227c, (short)0x207c, 
    (short)0x1efc, (short)0x1dfc, (short)0x1cfc, (short)0x1bfc, 
    (short)0x1afc, (short)0x19fc, (short)0x18fc, (short)0x17fc, 
    (short)0x16fc, (short)0x15fc, (short)0x14fc, (short)0x13fc, 
    (short)0x12fc, (short)0x11fc, (short)0x10fc, (short)0x0ffc, 
    (short)0x0f3c, (short)0x0ebc, (short)0x0e3c, (short)0x0dbc, 
    (short)0x0d3c, (short)0x0cbc, (short)0x0c3c, (short)0x0bbc, 
    (short)0x0b3c, (short)0x0abc, (short)0x0a3c, (short)0x09bc, 
    (short)0x093c, (short)0x08bc, (short)0x083c, (short)0x07bc, 
    (short)0x075c, (short)0x071c, (short)0x06dc, (short)0x069c, 
    (short)0x065c, (short)0x061c, (short)0x05dc, (short)0x059c, 
    (short)0x055c, (short)0x051c, (short)0x04dc, (short)0x049c, 
    (short)0x045c, (short)0x041c, (short)0x03dc, (short)0x039c, 
    (short)0x036c, (short)0x034c, (short)0x032c, (short)0x030c, 
    (short)0x02ec, (short)0x02cc, (short)0x02ac, (short)0x028c, 
    (short)0x026c, (short)0x024c, (short)0x022c, (short)0x020c, 
    (short)0x01ec, (short)0x01cc, (short)0x01ac, (short)0x018c, 
    (short)0x0174, (short)0x0164, (short)0x0154, (short)0x0144, 
    (short)0x0134, (short)0x0124, (short)0x0114, (short)0x0104, 
    (short)0x00f4, (short)0x00e4, (short)0x00d4, (short)0x00c4, 
    (short)0x00b4, (short)0x00a4, (short)0x0094, (short)0x0084, 
    (short)0x0078, (short)0x0070, (short)0x0068, (short)0x0060, 
    (short)0x0058, (short)0x0050, (short)0x0048, (short)0x0040, 
    (short)0x0038, (short)0x0030, (short)0x0028, (short)0x0020, 
    (short)0x0018, (short)0x0010, (short)0x0008, (short)0x0000
   };

   return MuLaw[index];
}



//////////////////////////////
//
// soundReadSample8M -- read channel samples as
//	8-bit Mulaw values.  Both of the following
//	functions do exactly the same thing.
//

double soundReadSample8M_B(FileIO& soundfile) {
   uchar sample;
   soundfile.readBigEndian(sample);
   return mulaw8ToLinear16(sample) / (long)0x8000;
}


double soundReadSample8M_L(FileIO& soundfile) {
   uchar sample;
   soundfile.readLittleEndian(sample);
   return mulaw8ToLinear16(sample) / (long)0x8000;
}



//////////////////////////////
//
// soundReadSample8L -- both functions do exactly the same thing,
//     which is to read an 8-bit linear sound code.
//

double soundReadSample8L_B(FileIO& soundfile) {
   char sample;
   soundfile.readBigEndian(sample);
   return (double)sample / (long)0x80;
}

double soundReadSample8L_L(FileIO& soundfile) {
   char sample;
   soundfile.readLittleEndian(sample);
   return (double)sample / (long)0x80;
}



//////////////////////////////
//
// soundReadSample16L -- Read samples as 16 bit linear int values.
//
 
double soundReadSample16L_B(FileIO& soundfile) {
   short sample;
   soundfile.readBigEndian(sample);
   return (double)sample / (long)0x8000;
}

 
double soundReadSample16L_L(FileIO& soundfile) {
   short sample;
   soundfile.readLittleEndian(sample);
   return (double)sample / (long)0x8000;
}



//////////////////////////////
//
// soundReadSample24L -- Read samples as 24 bit linear long int values.
//

double soundReadSample24L_B(FileIO& soundfile) {
   unsigned char temp[3];
   long sample;
   soundfile.readLittleEndian(temp[0]);
   soundfile.readLittleEndian(temp[1]);
   soundfile.readLittleEndian(temp[2]);
   sample = temp[0];
   sample = (sample << 8) | temp[1];
   sample = (sample << 8) | temp[2];
   if (temp[0] & 0x80) {
      sample = sample | 0xff000000;
   }
   return (double)sample / (long)0x800000;
}


double soundReadSample24L_L(FileIO& soundfile) {
   unsigned char temp[3];
   long sample;
   soundfile.readLittleEndian(temp[2]);
   soundfile.readLittleEndian(temp[1]);
   soundfile.readLittleEndian(temp[0]);
   sample = temp[0];
   sample = (sample << 8) | temp[1];
   sample = (sample << 8) | temp[2];
   if (temp[0] & 0x80) {
      sample = sample | 0xff000000;
   }
   return (double)sample / (long)0x800000;
}



//////////////////////////////
//
// soundReadSample32L -- Read samples as 32 bit linear long int values.
//

double soundReadSample32L_B(FileIO& soundfile) {
   long sample;
   soundfile.readBigEndian(sample);
   return (double)sample / (ulong)0x80000000;
}


double soundReadSample32L_L(FileIO& soundfile) {
   long sample;
   soundfile.readLittleEndian(sample);
   return (double)sample / (ulong)0x80000000;
}



//////////////////////////////
//
// soundReadSample32F -- Read samples a float values.
//

double soundReadSample32F_B(FileIO& soundfile) {
   float sample;
   soundfile.readBigEndian(sample);
   return (double)sample;
}


double soundReadSample32F_L(FileIO& soundfile) {
   float sample;
   soundfile.readLittleEndian(sample);
   return (double)sample;
}



//////////////////////////////
//
// soundReadSample64F -- Read samples as double float values.
//


double soundReadSample64F_B(FileIO& soundfile) {
   double sample;
   soundfile.readBigEndian(sample);
   return sample;
}


double soundReadSample64F_L(FileIO& soundfile) {
   double sample;
   soundfile.readLittleEndian(sample);
   return sample;
}



// md5sum: b5400cc45a110b33a3aae40cae759522 SoundFileRead.cpp [20050403]
