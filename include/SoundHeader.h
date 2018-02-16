//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri May 23 21:40:12 GMT-0800 1997
// Last Modified: Fri May 23 21:40:12 GMT-0800 1997
// Filename:      ...sig/maint/code/base/SoundHeader/SoundHeader.h
// Web Address:   http://sig.sapp.org/include/sigBase/SoundHeader.h
// Documentation: http://sig.sapp.org/doc/classes/SoundHeader
// Syntax:        C++ 
//

#ifndef _SOUNDHEADER_H_INCLUDED
#define _SOUNDHEADER_H_INCLUDED

#include "FileIO.h"

#ifndef OLDCPP
   #include <iostream>
   using namespace std;
#else
   #include <iostream.h>
#endif

// define recognized NeXT/Sun soundfile formats
#ifndef SND_FORMAT_UNSPECIFIED
   #define SND_FORMAT_UNSPECIFIED (0)
   #define SND_FORMAT_MULAW_8     (1)
   #define SND_FORMAT_LINEAR_8    (2)
   #define SND_FORMAT_LINEAR_16   (3)
   #define SND_FORMAT_LINEAR_24   (4)
   #define SND_FORMAT_LINEAR_32   (5)
   #define SND_FORMAT_FLOAT       (6)
   #define SND_FORMAT_DOUBLE      (7)
#endif

// internal Soundheader format types.
#define FORMAT_NONE          (0)
#define FORMAT_SND_LINEAR_16 (1)
#define FORMAT_AU_LINEAR_16  (1)
#define FORMAT_SND_MULAW_8   (2)
#define FORMAT_AU_MULAW_8    (2)
#define FORMAT_SND_LINEAR_8  (3)
#define FORMAT_AU_LINEAR_8   (3)
#define FORMAT_SND_LINEAR_24 (4)
#define FORMAT_AU_LINEAR_24  (4)
#define FORMAT_SND_LINEAR_32 (5)
#define FORMAT_AU_LINEAR_32  (5)
#define FORMAT_SND_FLOAT     (6)
#define FORMAT_AU_FLOAT      (6)
#define FORMAT_SND_DOUBLE    (7)
#define FORMAT_AU_DOUBLE     (7)
#define FORMAT_WAV_LINEAR_8  (8)
#define FORMAT_WAV_LINEAR_16 (9)
#define FORMAT_WAV_LINEAR_24 (10)
#define FORMAT_WAV_LINEAR_32 (11)
#define FORMAT_TXT           (12)
#define FORMAT_DEFAULT       (1)

// sound file types that are recognized by the SoundHeader Class
#define TYPE_NONE            (0)
#define TYPE_SND             (1)
#define TYPE_WAV             (2)
#define TYPE_WAV_PCM         (2)
#define TYPE_TEXT            (3)
#define TYPE_DEFAULT         (4)


class SoundHeader {
   public:
                      SoundHeader         (void);
                      SoundHeader         (char* aFilename);
                      SoundHeader         (SoundHeader& aHeader);
                     ~SoundHeader         ();
      int             getBitsPerSample    () const;
      int             getBlockAlign       () const;
      int             getBytesPerSample   () const;
      int             getChannels         () const;
      const char*     getComment          () const;
      int             getCommentSize      () const;
      int             getDataByteOffset   (int aType = TYPE_NONE) const;
      int             getDataByteSize     (void) const;
      int             getInputType        (void) const;
      int             getNextFormat       (void) const;
      int             getOutputType       (void) const;
      int             getSamples          (void) const;
      int             getSrate            (void) const;
      void            modifyFormat        (const char* filename);
      SoundHeader&    operator=           (const SoundHeader& h);
      void            setChannels         (int numChannels);
      void            setComment          (const char* aString);
      void            setHeader           (const char *filename);
      void            setHighMono         (const char* comment = "");
      void            setHighStereo       (const char* comment = "");
      void            setNextFormat       (int format = SND_FORMAT_LINEAR_16);
      int             setOutputType       (int format = TYPE_DEFAULT);
      void            setSamples          (int numSamples);
      void            setSrate            (int aSrate);
      void            print               (void);
      

   protected:
      int   bitsPerSample;     // needed for WAVE files mostly
      int   channels;          // number of channels
      int   commentLength;     // length of comment string
      char* commentString;     // comment string (not for WAVE format)
      int   NeXT_DataFormat;   // needed for NeXT/Sun files
      int   samples;           // number of samples in the data (per channel)
      int   samplingRate;      // (rounded) samping rate

      int   inputType;         // header type that was read in.
      int   outputType;        // format for operator<<


      // derived data variables:
      int   blockAlign;        // bitspersample/8*channels
      int   sampleBytes;       // bytes per sample




      // private functions:
      int  getFormat(const char *filename);
      void guessNextFormat(void);
      void processInSndFormat(const char* filename);
      void processInWavFormat(const char* filename);
      void updateDerivedVariables(void);
};


ostream& operator<<(ostream& output, SoundHeader& aSoundHeader);
FileIO& operator<<(FileIO& output, SoundHeader& aSoundHeader);


#define SIG_LITTLE_ENDIAN (0)
#define SIG_BIG_ENDIAN    (1)



#endif  /* _SOUNDHEADER_H_INCLUDED */



// md5sum: b6f79e8be91b24265324e6c7d8dd44c5 SoundHeader.h [20010708]
