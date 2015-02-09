//
// Copyright 1997 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri May  9 22:30:32 PDT 1997
// Last Modified: Sun Dec 14 05:26:16 GMT-0800 1997
// Filename:      ...sig/maint/code/base/FileIO/FileIO.h
// Web Address:   http://sig.sapp.org/include/sigBase/FileIO.h
// Documentation: http://sig.sapp.org/doc/classes/FileIO
// Syntax:        C++ 
//
// Description:   Derived from the fstream class, this class has
//                functions which allow writing binary files in
//                both little and big endian formats.  Useful for
//                writing files such as soundfiles and MIDI files
//                which require numbers to be stored in a particular
//                endian format.
//

#ifndef _FILEIO_H_INCLUDED
#define _FILEIO_H_INCLUDED

#ifndef OLDCPP
   #include <fstream>
   using namespace std;
#else
   #include <fstream.h>
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;


// templates would be nice to use here, but they don't seem 
// to work intuitively...

class FileIO : public fstream {
   public:
                     FileIO                (void);
#ifndef OLDCPP
                     FileIO      (const char* filename, ios::openmode state);
#else
                     FileIO      (const char* filename, int state);
#endif

                    ~FileIO                ();

      void           readBigEndian         (char& aNumber);
      void           readBigEndian         (uchar& aNumber);
      void           readBigEndian         (short& aNumber);
      void           readBigEndian         (ushort& aNumber);
      void           readBigEndian         (long& aNumber);
      void           readBigEndian         (ulong& aNumber);
      void           readBigEndian         (int& aNumber);
      void           readBigEndian         (uint& aNumber);
      void           readBigEndian         (float& aNumber);
      void           readBigEndian         (double& aNumber);

      void           readLittleEndian      (char& aNumber);
      void           readLittleEndian      (uchar& aNumber);
      void           readLittleEndian      (short& aNumber);
      void           readLittleEndian      (ushort& aNumber);
      void           readLittleEndian      (long& aNumber);
      void           readLittleEndian      (ulong& aNumber);
      void           readLittleEndian      (int& aNumber);
      void           readLittleEndian      (uint& aNumber);
      void           readLittleEndian      (float& aNumber);
      void           readLittleEndian      (double& aNumber);

      void           readMachineEndian     (char& aNumber);
      void           readMachineEndian     (uchar& aNumber);
      void           readMachineEndian     (short& aNumber);
      void           readMachineEndian     (ushort& aNumber);
      void           readMachineEndian     (long& aNumber);
      void           readMachineEndian     (ulong& aNumber);
      void           readMachineEndian     (int& aNumber);
      void           readMachineEndian     (uint& aNumber);
      void           readMachineEndian     (float& aNumber);
      void           readMachineEndian     (double& aNumber);

      void           readNotMachineEndian  (char& aNumber);
      void           readNotMachineEndian  (uchar& aNumber);
      void           readNotMachineEndian  (short& aNumber);
      void           readNotMachineEndian  (ushort& aNumber);
      void           readNotMachineEndian  (long& aNumber);
      void           readNotMachineEndian  (ulong& aNumber);
      void           readNotMachineEndian  (int& aNumber);
      void           readNotMachineEndian  (uint& aNumber);
      void           readNotMachineEndian  (float& aNumber);
      void           readNotMachineEndian  (double& aNumber);

      void           writeBigEndian        (char aNumber);
      void           writeBigEndian        (uchar aNumber);
      void           writeBigEndian        (short aNumber);
      void           writeBigEndian        (ushort aNumber);
      void           writeBigEndian        (long aNumber);
      void           writeBigEndian        (ulong aNumber);
      void           writeBigEndian        (int aNumber);
      void           writeBigEndian        (uint aNumber);
      void           writeBigEndian        (float aNumber);
      void           writeBigEndian        (double aNumber);

      void           writeLittleEndian     (char aNumber);
      void           writeLittleEndian     (uchar aNumber);
      void           writeLittleEndian     (short aNumber);
      void           writeLittleEndian     (ushort aNumber);
      void           writeLittleEndian     (long aNumber);
      void           writeLittleEndian     (ulong aNumber);
      void           writeLittleEndian     (int aNumber);
      void           writeLittleEndian     (uint aNumber);
      void           writeLittleEndian     (float aNumber);
      void           writeLittleEndian     (double aNumber);

      void           writeMachineEndian    (char aNumber);
      void           writeMachineEndian    (uchar aNumber);
      void           writeMachineEndian    (short aNumber);
      void           writeMachineEndian    (ushort aNumber);
      void           writeMachineEndian    (long aNumber);
      void           writeMachineEndian    (ulong aNumber);
      void           writeMachineEndian    (int aNumber);
      void           writeMachineEndian    (uint aNumber);
      void           writeMachineEndian    (float aNumber);
      void           writeMachineEndian    (double aNumber);

      void           writeNotMachineEndian (char aNumber);
      void           writeNotMachineEndian (uchar aNumber);
      void           writeNotMachineEndian (short aNumber);
      void           writeNotMachineEndian (ushort aNumber);
      void           writeNotMachineEndian (long aNumber);
      void           writeNotMachineEndian (ulong aNumber);
      void           writeNotMachineEndian (int aNumber);
      void           writeNotMachineEndian (uint aNumber);
      void           writeNotMachineEndian (float aNumber);
      void           writeNotMachineEndian (double aNumber);

   protected:

      char           flipBytes             (char aNumber);
      uchar          flipBytes             (uchar aNumber);
      short          flipBytes             (short aNumber);
      ushort         flipBytes             (ushort aNumber);
      long           flipBytes             (long aNumber);
      ulong          flipBytes             (ulong aNumber);
      int            flipBytes             (int aNumber);
      uint           flipBytes             (uint aNumber);
      float          flipBytes             (float aNumber);
      double         flipBytes             (double aNumber);

};



#endif  /* _FILEIO_H_INCLUDED */



// md5sum: 20f4083bc14ee144905e6902659f7494 FileIO.h [20050403]
