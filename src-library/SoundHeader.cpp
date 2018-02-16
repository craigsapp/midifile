//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri May 23 21:40:12 GMT-0800 1997
// Last Modified: Wed Jan 14 23:56:16 GMT-0800 1998
// Filename:      ...sig/maint/code/src/base/SoundHeader/SoundHeader.cpp
// Web Address:   http://sig.sapp.org/src/sigBase/SoundHeader.cpp
// Documentation: http://sig.sapp.org/doc/classes/SoundHeader
// Syntax:        C++ 
//

#include "SoundHeader.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

//////////////////////////////
//
// SoundHeader::SoundHeader -- default set to 44100 sampling rate,
//	1 channel, 16-bit linear.
//

SoundHeader::SoundHeader(void) {
   inputType  = TYPE_NONE;
   outputType = TYPE_DEFAULT;

   bitsPerSample = 16;
   channels = 1;
   commentLength = 0;
   commentString = new char[1];
   commentString[0] = '\0';
   NeXT_DataFormat = SND_FORMAT_LINEAR_16;
   samples = 0;
   samplingRate = 44100;
}


SoundHeader::SoundHeader(char* aFilename) {
   commentLength = 0;
   commentString = new char[1];
   commentString[0] = '\0';
   setHeader(aFilename);
}


SoundHeader::SoundHeader(SoundHeader& aHeader) {
   inputType = aHeader.inputType;
   outputType = aHeader.outputType;
   bitsPerSample = aHeader.bitsPerSample;
   channels = aHeader.channels;
   commentLength = aHeader.commentLength;
   commentString = new char[commentLength + 1];
   strcpy(commentString, aHeader.commentString);
   NeXT_DataFormat = aHeader.NeXT_DataFormat;
   samples = aHeader.samples;
   samplingRate = aHeader.samplingRate;
}



//////////////////////////////
//
// SoundHeader::~SoundHeader --
//

SoundHeader::~SoundHeader() {
   if (commentString != NULL) {
      delete [] commentString;
   }
}



//////////////////////////////
//
// SoundHeader::getBitsPerSample --
//

int SoundHeader::getBitsPerSample(void) const {
   return bitsPerSample;
}



//////////////////////////////
//
// SoundHeader::getBlockAlign -- returns the number of bytes
//	in a single sample set.
// 

int SoundHeader::getBlockAlign(void) const {
   return blockAlign;
}
 


//////////////////////////////
//
// SoundHeader::getBytesPerSample  --
//

int SoundHeader::getBytesPerSample(void) const {
   return sampleBytes;
}



//////////////////////////////
//
// SoundHeader::getChannels --
//

int SoundHeader::getChannels(void) const {
   return channels;
}



//////////////////////////////
//
// SoundHeader::getComment --
//

const char* SoundHeader::getComment(void) const {
   return commentString;
}



//////////////////////////////
//
// SoundHeader::getCommentSize --
//

int SoundHeader::getCommentSize(void) const {
   return commentLength;
}



//////////////////////////////
//
// SoundHeader::getDataByteOffset -- returns the byte offset into the
//	soundfile of the data.  Offset is from 0.   Returns -1 if
//	there is an error such as an invalid format type.
//	default value: aType = TYPE_NONE
//

int SoundHeader::getDataByteOffset(int aType) const {
   int realType = aType == TYPE_NONE ? outputType : aType;
   switch (realType) {
      case TYPE_SND:
         if (getCommentSize() < 4) {
            return 28;
         } else if (getCommentSize() % 2 == 0) {
            return 24 + getCommentSize();
         } else {                               // keep data start at even 
            return 24 + getCommentSize() + 1;   // offset for certain NeXTStep
         }                                      // sound programs
         break;
      case TYPE_WAV_PCM:
         return 44;
         break;
      default:
         return -1;
   }
}



//////////////////////////////
//
// SoundHeader::getDataByteSize -- returns the total number of
//	bytes in the sound data area.
//

int SoundHeader::getDataByteSize(void) const {
   return (int)(getBitsPerSample() / 8.0 * getSamples() * getChannels());
}



//////////////////////////////
//
// SoundHeader::getInputType --
//

int SoundHeader::getInputType(void) const {
   return inputType;
}



//////////////////////////////
//
// SoundHeader::getNextFormat --
//

int SoundHeader::getNextFormat(void) const {
   return NeXT_DataFormat;
}



//////////////////////////////
//
// SoundHeader::getOutputType  -- the soundfile format that
//	will be output by the operator<<.
//

int SoundHeader::getOutputType(void) const {
   return outputType;
}



//////////////////////////////
//
// SoundHeader::getSamples -- returns the number of samples
//	in the soundfile per channel.
//

int SoundHeader::getSamples(void) const {
   return samples;
}



//////////////////////////////
//
// SoundHeader::getSrate -- returns the sampling rate
//

int SoundHeader::getSrate(void) const {
   return samplingRate;
}



//////////////////////////////
//
// SoundHeader::modifyFormat -- used for writing out soundfile in
//	a different format than was read in.  You do not want
//	to modify a header for a file you are reading in!  
//	You modify a header to write a new file which has
//	a different format from the input.
//

void SoundHeader::modifyFormat(const char* filename) {
   int aFormat = getFormat(filename);
   switch (aFormat) {
      case FORMAT_SND_MULAW_8:
         NeXT_DataFormat = SND_FORMAT_MULAW_8;
         bitsPerSample = 8;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_LINEAR_8:
         NeXT_DataFormat = SND_FORMAT_LINEAR_8;
         bitsPerSample = 8;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_LINEAR_16:
         NeXT_DataFormat = SND_FORMAT_LINEAR_16;
         bitsPerSample = 16;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_LINEAR_24:
         NeXT_DataFormat = SND_FORMAT_LINEAR_24;
         bitsPerSample = 24;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_LINEAR_32:
         NeXT_DataFormat = SND_FORMAT_LINEAR_32;
         bitsPerSample = 32;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_FLOAT:
         NeXT_DataFormat = SND_FORMAT_FLOAT;
         bitsPerSample = 32;
         outputType = TYPE_SND;
         break;
      case FORMAT_SND_DOUBLE:
         NeXT_DataFormat = SND_FORMAT_DOUBLE;
         bitsPerSample = 64;
         outputType = TYPE_SND;
         break;
      case FORMAT_WAV_LINEAR_8:
         NeXT_DataFormat = SND_FORMAT_LINEAR_8;
         bitsPerSample = 8;
         outputType = TYPE_WAV;
         break;
      case FORMAT_WAV_LINEAR_16:
         NeXT_DataFormat = SND_FORMAT_LINEAR_16;
         bitsPerSample = 16;
         outputType = TYPE_WAV;
         break;
      case FORMAT_WAV_LINEAR_24:
         NeXT_DataFormat = SND_FORMAT_LINEAR_24;
         bitsPerSample = 24;
         outputType = TYPE_WAV;
         break;
      case FORMAT_WAV_LINEAR_32:
         NeXT_DataFormat = SND_FORMAT_LINEAR_32;
         bitsPerSample = 32;
         outputType = TYPE_WAV;
         break;
      default:
         cerr << "Error: unknow output filetype: " << filename << endl;
         exit(1);
   }
   updateDerivedVariables();
}



//////////////////////////////
//
// SoundHeader::operator= --
//

SoundHeader& SoundHeader::operator=(const SoundHeader& h) {
   // don't do anything if assigning to itself:
   if (this == &h) return *this;

   bitsPerSample = h.bitsPerSample;
   channels = h.channels;
   commentLength = h.commentLength;
   NeXT_DataFormat = h.NeXT_DataFormat;
   samples = h.samples;
   samplingRate = h.samplingRate;

   inputType = h.inputType;
   outputType = h.outputType;

   if (commentString != NULL) {
      delete [] commentString;
   }
   commentString = new char[commentLength+1];
   strcpy(commentString, h.commentString);

   blockAlign = h.blockAlign;
   sampleBytes = h.sampleBytes;

   return *this;
}



//////////////////////////////
//
// SoundHeader::setChannels --
//

void SoundHeader::setChannels(int numChannels) {
   channels = numChannels;
   updateDerivedVariables();
}



//////////////////////////////
//
// SoundHeader::setComment --
//

void SoundHeader::setComment(const char* aString) {
   commentLength = strlen(aString);
   if (commentString != NULL) {
      delete [] commentString;
   }
   commentString = new char[commentLength+1];
   strcpy(commentString, aString);
}



//////////////////////////////
//
// SoundHeader::setHeader --
//

void SoundHeader::setHeader(const char* filename) {
   int format = getFormat(filename);
   switch (format) {
      case FORMAT_SND_MULAW_8:
      case FORMAT_SND_LINEAR_8:
      case FORMAT_SND_LINEAR_16:
      case FORMAT_SND_LINEAR_24:
      case FORMAT_SND_LINEAR_32:
      case FORMAT_SND_FLOAT:
      case FORMAT_SND_DOUBLE:
         inputType = TYPE_SND;
         break;
      case FORMAT_WAV_LINEAR_8:
      case FORMAT_WAV_LINEAR_16:
      case FORMAT_WAV_LINEAR_24:
      case FORMAT_WAV_LINEAR_32:
         inputType = TYPE_WAV;
         break;
      default:
         cerr << "Unknown input soundfile type: " << filename << endl;
         exit(1);
   }

   switch (inputType) {
      case TYPE_SND:
         processInSndFormat(filename);
         break;
      case TYPE_WAV:
         processInWavFormat(filename);
         break;
      default:
         cerr << "Unknown soundfile format for file: " << filename << endl;
         exit(1);
   }
}



//////////////////////////////
//
// SoundHeader::setHighMono() -- set the header to 
//	44100 sampling rate, 16-bit samples, 1 channel.
//	default value: comment = ""
//

void SoundHeader::setHighMono(const char* comment) {
   setSrate(44100);
   setChannels(1);
   setComment(comment);
   setNextFormat(SND_FORMAT_LINEAR_16);
}



//////////////////////////////
//
// SoundHeader::setHighStereo() -- set the header to
//	44100 sampling rate, 16-bit samples, 2 channels.
//	default value: comment = ""
//

void SoundHeader::setHighStereo(const char* comment) {
   setSrate(44100);
   setChannels(2);
   setComment(comment);
   setNextFormat(SND_FORMAT_LINEAR_16);
}



//////////////////////////////
//
// SoundHeader::setNextFormat --
//	default value: format = SND_FORMAT_LINEAR_16
//

void SoundHeader::setNextFormat(int format) {

   // update internal variables based on the NeXT_DataFormat:
   switch (format) {
      case SND_FORMAT_MULAW_8:
         bitsPerSample = 8;
         break;
      case SND_FORMAT_LINEAR_8:
         bitsPerSample = 8;
         break;
      case SND_FORMAT_LINEAR_16:
         bitsPerSample = 16;
         break;
      case SND_FORMAT_LINEAR_24:
         bitsPerSample = 24;
         break;
      case SND_FORMAT_LINEAR_32:
         bitsPerSample = 32;
         break;
      case SND_FORMAT_FLOAT:
         bitsPerSample = 32;
         break;
      case SND_FORMAT_DOUBLE:
         bitsPerSample = 64;
         break;
      default:
         cerr << "Unknown format: " << format << endl;
         exit(1);
   }

   NeXT_DataFormat = format;
}



//////////////////////////////
//
// SoundHeader::setOutputType -- returns old format.
//	default value: aType = TYPE_DEFAULT
//

int SoundHeader::setOutputType(int aType) {
   int temp = outputType;
   outputType = aType;
   return temp;
}



//////////////////////////////
//
// SoundHeader::setSamples --
//

void SoundHeader::setSamples(int numSamples) {
   samples = numSamples;
}



//////////////////////////////
//
// SoundHeader::setSrate --
//

void SoundHeader::setSrate(int aSrate) {
   samplingRate = aSrate;
}



//////////////////////////////
//
// (SoundHeader::)operator<< --
//

ostream& operator<<(ostream& output, SoundHeader& header) {

   output << "Bits per sample: " << header.getBitsPerSample() << endl;
   output << "Channels:        " << header.getChannels() <<endl;
   output << "Sampling rate:   " << header.getSrate() << endl;
   output << "Samples:         " << header.getSamples() << endl;
   output << "Data bytes:      " << header.getDataByteSize() << endl;
   output << "Data offset:     " << 
      header.getDataByteOffset(header.getInputType()) << endl;
   output << "Duration:        " << (float)header.getSamples() / 
      header.getSrate() << " seconds" << endl;
   output << "Byte rate:       " << header.getBitsPerSample() / 8.0 *
      header.getSrate() * header.getChannels() << endl;
   output << "Block align:     " << header.getBlockAlign() << endl;
   if (header.getCommentSize() != 0) {
      output << "Comment:" << endl;
      output << "   " << header.getComment() << endl;
   }

   return output;
}


FileIO& operator<<(FileIO& output, SoundHeader& header) {
   switch (header.getOutputType()) {
      case TYPE_TEXT:
         // currently the same as above, but can change.
         output << "Bits per sample: " << header.getBitsPerSample() << endl;
         output << "Channels:        " << header.getChannels() <<endl;
         output << "Sampling rate:   " << header.getSrate() << endl;
         output << "Samples:         " << header.getSamples() << endl;
         output << "Data bytes:      " << header.getDataByteSize() << endl;
         output << "Data offset:     " << header.getDataByteOffset() << endl;
         output << "Duration:        " << (float)header.getSamples() / 
            header.getSrate() << " seconds" << endl;
         output << "Byte rate:       " << header.getBitsPerSample() / 8.0 *
            header.getSrate() * header.getChannels() << endl;
         output << "Block align:     " << header.getBlockAlign() << endl;
         if (header.getCommentSize() != 0) {
            output << "Comment:" << endl;
            output << "   " << header.getComment() << endl;
         }
         break;
      case TYPE_SND:
         output.writeBigEndian(0x2e736e64);     // ".snd"
         output.writeBigEndian(header.getDataByteOffset());
         output.writeBigEndian(header.getDataByteSize());
         output.writeBigEndian(header.getNextFormat());
         output.writeBigEndian(header.getSrate());
         output.writeBigEndian(header.getChannels());
         output.write(header.getComment(), header.getCommentSize());
         if (header.getCommentSize() < 4) {
            for (int i=0; i<4-header.getCommentSize(); i++) {
               output.writeBigEndian((char)0);
            }
         } else if (header.getCommentSize() % 2 != 0) {  // keep header length
            output.writeBigEndian((char)0);              // even for certain
         }                                               // NeXTStep programs
         break;
      case TYPE_WAV_PCM: 
         output.writeBigEndian((ulong)0x52494646);      // "RIFF"
         output.writeLittleEndian((ulong)(36 + header.getBitsPerSample()
            / 8.0  * header.getChannels() * header.getSamples()));
         output.writeBigEndian((ulong)0x57415645);      // "WAVE"
         output.writeBigEndian((ulong)0x666d7420);      // "fmt "
         output.writeLittleEndian((ulong)16);           // fmt subchunk size
         output.writeLittleEndian((ushort)1);           // PCM format
         output.writeLittleEndian((ushort)header.getChannels()); // channels
         output.writeLittleEndian((ulong)header.getSrate());  // sampling rate
         output.writeLittleEndian(
            (ulong)(header.getSrate()* header.getChannels() * 
            header.getBitsPerSample() / 8));  // byte rate
         output.writeLittleEndian((ushort) (header.getBlockAlign()));
         output.writeLittleEndian((ushort)header.getBitsPerSample());
         output.writeBigEndian((ulong)0x64617461);   // "data"
         output.writeLittleEndian((ulong)(header.getSamples() * 
            header.getChannels() * header.getBitsPerSample()/8));
         break;
      default:
         output << "Unkown soundfile format:" << header.getOutputType() << endl;
         exit(1);
   } // end of switch

   return output;
}



//////////////////////////////
//
// SoundHeader::print --
//

void SoundHeader::print(void) {
   int flag = setOutputType(TYPE_TEXT);
   cout << *this;
   setOutputType(flag);
}



///////////////////////////////////////////////////////////////////////
//
// Private functions:
//



//////////////////////////////
//
// SoundHeader::getFormat
//

int SoundHeader::getFormat(const char* string) {
   char extension[1000];
   int index = strlen(string) - 1;
   int sum = 0;
   while (index >= 0 && string[index] != '.') {
      sum += (int)tolower(string[index]);
      index--;
   }
   index++;

   strncpy(extension, &string[index], 1000);
   int length = strlen(extension);
   if (length > 200) length = 200;
   for (int i=0; i<length; i++) {
      extension[i] = tolower(extension[i]);
   }

   if (strcmp(extension, "snd") == 0) {
      return FORMAT_SND_LINEAR_16;
   } else if (strcmp(extension, "au") == 0) {
      return FORMAT_SND_LINEAR_16;
   } else if (strcmp(extension, "sndm") == 0) {
      return FORMAT_SND_MULAW_8;
   } else if (strcmp(extension, "aum") == 0) {
      return FORMAT_SND_MULAW_8;
   } else if (strcmp(extension, "snd1") == 0) {
      return FORMAT_SND_LINEAR_8;
   } else if (strcmp(extension, "au1") == 0) {
      return FORMAT_SND_LINEAR_8;
   } else if (strcmp(extension, "snd2") == 0) {
      return FORMAT_SND_LINEAR_16;
   } else if (strcmp(extension, "au2") == 0) {
      return FORMAT_SND_LINEAR_16;
   } else if (strcmp(extension, "snd3") == 0) {
      return FORMAT_SND_LINEAR_24;
   } else if (strcmp(extension, "au3") == 0) {
      return FORMAT_SND_LINEAR_24;
   } else if (strcmp(extension, "snd4") == 0) {
      return FORMAT_SND_LINEAR_32;
   } else if (strcmp(extension, "au4") == 0) {
      return FORMAT_SND_LINEAR_32;
   } else if (strcmp(extension, "sndf") == 0) {
      return FORMAT_SND_FLOAT;
   } else if (strcmp(extension, "auf") == 0) {
      return FORMAT_SND_FLOAT;
   } else if (strcmp(extension, "sndd") == 0) {
      return FORMAT_SND_DOUBLE;
   } else if (strcmp(extension, "aud") == 0) {
      return FORMAT_SND_DOUBLE;
   } else if (strcmp(extension, "wav2") == 0) {
      return FORMAT_WAV_LINEAR_16;
   } else if (strcmp(extension, "wav") == 0) {
      return FORMAT_WAV_LINEAR_16;
   } else if (strcmp(extension, "wav1") == 0) {
      return FORMAT_WAV_LINEAR_8;
   } else if (strcmp(extension, "wav3") == 0) {
      return FORMAT_WAV_LINEAR_24;
   } else if (strcmp(extension, "wav4") == 0) {
      return FORMAT_WAV_LINEAR_32;
   } else {
      cerr << "Unknown sound file type" << endl;
      exit(1);
   }

   // include the following line for stuipd Visual C++ 5.0 compiler.
   // this line should never be reached.
   return 0;
}



//////////////////////////////
//
// SoundHeader::guessNextFormat -- tries to match
//	the header data to a NeXT/Sun format number to
//	set the NeXT_DataFormat variable.  
//

void SoundHeader::guessNextFormat(void) {
   switch (getBitsPerSample()) {
      case 8:
         NeXT_DataFormat = SND_FORMAT_MULAW_8;
         break;
      case 16:
         NeXT_DataFormat = SND_FORMAT_LINEAR_16;
         break;
      case 24:
         NeXT_DataFormat = SND_FORMAT_LINEAR_24;
         break;
      case 32:
         NeXT_DataFormat = SND_FORMAT_FLOAT;
         break;
      case 64:
         NeXT_DataFormat = SND_FORMAT_DOUBLE;
         break;
      default:
         NeXT_DataFormat = SND_FORMAT_UNSPECIFIED;
   }
}



//////////////////////////////
//
// SoundHeader::processInSndFormat -- read the header from a
//	NeXT/Sun soundfile.
//

void SoundHeader::processInSndFormat(const char* filename) {
   FileIO sndfile;
   
   #ifndef OLDCPP
      sndfile.open(filename, ios::in);
   #else
      sndfile.open(filename, ios::in | ios::nocreate);
   #endif

   if(!sndfile.is_open()) {
      cerr << "Error: sound file " << filename
           << " could not be opened!" << endl;
      exit(1);
   }

   ulong tempLong;

   // read the magic number ".snd"
   sndfile.readBigEndian(tempLong);
   if (tempLong != 0x2e736e64) {
      cerr << "File: " << filename << " is not a recognized sound file type."
           << " Magic number should be 2e736e64, but is: " << hex 
           << tempLong << "." << endl;
      exit(1);
   }


   // read the data Location:
   sndfile.readBigEndian(tempLong);
   int dataLocation = tempLong;


   // read the number of bytes in the raw sound data.  calculate
   // samples later:
   ulong dataSize;
   sndfile.readBigEndian(dataSize);


   // read the sound format, which includes various pieces of 
   // sound information:
   sndfile.readBigEndian(tempLong);
   NeXT_DataFormat = tempLong;


   // read the sampling rate:
   sndfile.readBigEndian(tempLong);
   samplingRate = tempLong;


   // read the number of channels
   sndfile.readBigEndian(tempLong);
   channels = tempLong;


   // calulate the comment length and read the comment
   commentLength = dataLocation - 24;
   if (commentString != NULL) {
      delete [] commentString;
   }
   commentString = new char[commentLength+1];
   sndfile.read(commentString, commentLength);
   commentString[commentLength] = '\0';

   sndfile.close();

   // process data elements of SoundHeader

   // inputType already set before entering function but set again just in case:
   inputType  = TYPE_SND;
   outputType = TYPE_SND;

   // channels already set
   // samplingRate already set
   // comment string already set
   // commentLength already set
   // dataLocation already set
   // Next_DataFormat already set

   // calculate sample count and bits per sample:

   switch (NeXT_DataFormat) {
      case SND_FORMAT_MULAW_8:
         samples = dataSize / channels / 1;
         bitsPerSample = 8;
         break;
      case SND_FORMAT_LINEAR_8:
         samples = dataSize / channels / 1;
         bitsPerSample = 8;
         break;
      case SND_FORMAT_LINEAR_16:
         samples = dataSize / channels / 2;
         bitsPerSample = 16;
         break;
      case SND_FORMAT_LINEAR_24:
         samples = dataSize / channels / 3;
         bitsPerSample = 24;
         break;
      case SND_FORMAT_LINEAR_32:
         samples = dataSize / channels / 4;
         bitsPerSample = 32;
         break;
      case SND_FORMAT_FLOAT:
         samples = dataSize / channels / 4;
         bitsPerSample = 32;
         break;
      case SND_FORMAT_DOUBLE:
         samples = dataSize / channels / 8;
         bitsPerSample = 64;
         break;
      default:
         cerr << "Unknown NeXT/Sun data format: " << NeXT_DataFormat << endl;
         exit(1);
   }


   updateDerivedVariables();
}


/*
 * NeXT/Sun soundfile format:
 * 
 * OFFSET :
 * SIZE       field name   description
 * ====================================================================
 *  0 : 4     magic        must be equal to SND_MAGIC (0x2e736e64)
 *                         The magic number must appear at the
 *                         beginning of every SNDSoundStruct.  It is
 *                         used for type checking and byte ordering
 *                         information.
 *  4 : 4    dataLocation  Offset from start of file to the raw sound data
 *  8 : 4    dataSize      Number of bytes of data in the raw sound data
 * 12 : 4    dataFormat    NeXT data format codes. User-defined formats
 *                         should be greater than 255.  Negative format numbers
 *                         are reserved.  Defined codes are listed in
 *                         <sound/soundstruct.h>.  Common formats are:
 *                                 SND_FORMAT_MULAW_8         (1)
 *                                 SND_FORMAT_LINEAR_16       (3)
 *                                 SND_FORMAT_FLOAT           (6)
 * 16 : 4    samplingRate  The sampling rate.  Hardware supported sampling rates
 *                         are:
 *                                 SND_RATE_CODEC          (8012.8210513)
 *                                 SND_RATE_LOW            (22050.0)
 *                                 SND_RATE_HIGH           (44100.0)
 *                                 SND_RATE_LOW_PC         (11025.0)
 * 20 : 4    channelCount  The number of channels
 * 24 : 4+X  info[4]       Comments relating to the sound, or user
 *                         defined data fields. Must be at least 4 bytes long,
 *                         but can be as long as desired, since the data starts
 *                         at the offset in bytes from the beginning of the
 *                         soundfile according to the number in the dataLocation
 *                         field.
 * 28+X: *   data          the actual sound data bytes.
 * 
 */


//////////////////////////////
//
// SoundHeader::processInWavFormat
//

void SoundHeader::processInWavFormat(const char *filename) {
   FileIO sndfile;

   sndfile.open(filename, ios::in);
   if (!sndfile.is_open()) {
      cerr << "Error: sound file " << filename
           << " could not be opened!" << endl;
      exit(1);
   }

   ulong tempLong;
   ushort tempShort;

   // read the ChunkID, which should be the characters "RIFF":
   sndfile.readBigEndian(tempLong);
   if (tempLong != 0x52494646) {
      cerr << "Error: ChunkID expected to be 0x52494646 (RIFF), but is: 0x"
           << hex << tempLong << endl;
      exit(1);
   }


   // read the ChunkSize (ignore for Canonical WAVE format):
   sndfile.readLittleEndian(tempLong);

   
   // read the Format:
   sndfile.readBigEndian(tempLong);
   if (tempLong != 0x57415645) {
      cerr << "Error: expecting the WAVE format (0x57415645 = WAVE) \n"
           << "   but got: 0x" << tempLong << endl;
      exit(1);
   }


   // read the Subchunk1ID (should be "fmt "):
   sndfile.readBigEndian(tempLong);
   if (tempLong != 0x666d7420) {
      cerr << "Error: expecting the fmt subchunk (0x666d7420) \n"
           << "   but got: 0x" << tempLong << endl;
      exit(1);
   }


   // read the Subchunk1Size:
   sndfile.readLittleEndian(tempLong);
   if (tempLong != 16) {
      cerr << "Error: expecting the fmt subchunk size to be 16, \n"
           << "   but it is: " << tempLong << endl;
      exit(1);
   }


   // read the AudioFormat:
   sndfile.readLittleEndian(tempShort);
   if (tempShort != 1) {
      cerr << "Error: expecting audio format to be PCM (1). " << endl
           << "   Cannot handle this format (compression): " 
           << tempShort << endl;
      exit(1);
   }


   // read the NumChannels:
   sndfile.readLittleEndian(tempShort);
   if (tempShort > 1000) {
      cerr << "Error: I don't believe this file has " << tempShort 
           << " channels" << endl;
      exit(1);
   }
   channels = tempShort;


   // read the SampleRate:
   sndfile.readLittleEndian(tempLong);
   samplingRate = tempLong;


   // read the ByteRate == SampleRate * NumChannels * BitsPerSample/8:
   sndfile.readLittleEndian(tempLong);


   // read the BlockAlign == NumChannels * BitsPerSample/8:
   sndfile.readLittleEndian(tempShort);


   // read the BitsPerSample:
   sndfile.readLittleEndian(tempShort);
   bitsPerSample = tempShort;


   // read the Subchunk2ID (should be "data" (0x64617461)):
   sndfile.readBigEndian(tempLong);


   // read the Subchunk2Size, calculate the number of samples:
   sndfile.readLittleEndian(tempLong);
   samples = tempLong / (bitsPerSample/8) / channels;

   sndfile.close();

   // process data elements of SoundHeader

   // dataLocation = 44;

   // channels already set
   // samplingRate already set
   // sample count already set
   // bits per sample already set
   
   // inputType already set before entering function but set again just in case:
   inputType = TYPE_WAV_PCM;
   outputType = inputType;

   // set the comment string to nothing:
   if (commentString != NULL) {
      delete [] commentString;
   }
   commentString = new char[1];
   commentString[0] = '\0';
   commentLength = 0;

   // set the Next_DataFormat
   guessNextFormat();

   updateDerivedVariables();
}



/*
 * 
 * Wave (*.wav) PCM File Format
 * 
 * OFFSET  SIZE  NAME             DESCRIPTION
 * 0         4   ChunkID          Contains the letters "RIFF"
 * 4         4   ChunkSize        38 + ExtraParamSize + SubChunk2Size
 *                                This is the offset of the actual sound data
 * 8         4   Format           Contains the letters "WAVE"
 * 12        4   Subchunk1ID      Contains the letters "fmt "
 * 16        4   Subchunk1Size    18 + ExtraParamSize
 * 20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
 * 22        2   NumChannels      mono = 1, stereo = 2, etc.
 * 24        4   SampleRate       8000, 44100, etc.
 * 28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
 * 32        2   BlockAlign       == NumChannels * BitsPerSample/8
 * 34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.
 * 36        4   Subchunk2ID      Contains the letters "data"
 * 40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
 *                                the number of data bytes
 * 44        *   Data             the actual sound data.
 * 
 */




//////////////////////////////
//
// SoundHeader::updateDerivedVariables
//

void SoundHeader::updateDerivedVariables(void) {
   blockAlign = getBitsPerSample()/8 * getChannels();
   sampleBytes = getBitsPerSample() / 8;

};



// md5sum: 70ce68c99ced33a13c2ea3bb8a33aa64 SoundHeader.cpp [20050403]
