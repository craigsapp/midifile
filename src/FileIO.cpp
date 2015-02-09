//
// Copyright 1997 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri May  9 22:30:32 PDT 1997
// Last Modified: Sun Dec 14 03:29:39 GMT-0800 1997
// Last Modified: Thu Feb 17 04:04:18 PST 2011 (updated for 64-bit archs)
// Filename:      ...sig/maint/code/sigBase/FileIO.cpp
// Web Address:   http://sig.sapp.org/src/sigBase/FileIO.cpp
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

#include "sigConfiguration.h"
#include "assert.h"
#include "string.h"
#include "FileIO.h"

#ifndef OLDCPP
   using namespace std;
   #include <iostream>
#endif



//////////////////////////////
//
// FileIO::FileIO --
//

FileIO::FileIO(void) {
   // do nothing
};

#ifndef OLDCPP
   FileIO::FileIO(const char* filename, ios::openmode state) :
#else
   FileIO::FileIO(const char* filename, int state) :
#endif 

#ifdef VISUAL                 /* for stupid LF-CR prevention in DOS */
   fstream(filename, state | ios::binary) { 
#else
   fstream(filename, state) { 
#endif

   // do nothing
};



//////////////////////////////
//
// FileIO::~FileIO --
//

FileIO::~FileIO() {
   // do nothing
}



//////////////////////////////
//
// FileIO::readBigEndian --
//	Read numbers from a file as big endian
//

void FileIO::readBigEndian(char& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(uchar& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(short& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(ushort& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(long& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(ulong& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(int& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(uint& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(float& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}

void FileIO::readBigEndian(double& aNumber) {
   #ifdef OTHEREND
      readNotMachineEndian(aNumber);
   #else
      readMachineEndian(aNumber);
   #endif
}



//////////////////////////////
//
// FileIO::readLittleEndian --
//	Read numbers from a file as little endian
//

void FileIO::readLittleEndian(char& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(uchar& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(short& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(ushort& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(long& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(ulong& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(int& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(uint& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(float& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}

void FileIO::readLittleEndian(double& aNumber) {
   #ifdef OTHEREND
      readMachineEndian(aNumber);
   #else
      readNotMachineEndian(aNumber);
   #endif
}



//////////////////////////////
//
// FileIO::readMachineEndian --
//	Read numbers from a file in the same endian as the computer.
//

void FileIO::readMachineEndian(char& aNumber) {
   this->read(&aNumber, 1);
}

void FileIO::readMachineEndian(uchar& aNumber) {
   this->read((char*)(&aNumber), 1);
}

void FileIO::readMachineEndian(short& aNumber) {
   this->read((char*)&aNumber, 2);
}

void FileIO::readMachineEndian(ushort& aNumber) {
   this->read((char*)&aNumber, 2);
}

void FileIO::readMachineEndian(long& aNumber) {
   this->read((char*)&aNumber, 4);
}

void FileIO::readMachineEndian(ulong& aNumber) {
   this->read((char*)&aNumber, 4);
}

void FileIO::readMachineEndian(int& aNumber) {
   this->read((char*)&aNumber, 4);
}

void FileIO::readMachineEndian(uint& aNumber) {
   this->read((char*)&aNumber, 4);
}

void FileIO::readMachineEndian(float& aNumber) {
   this->read((char*)&aNumber, 4);
}

void FileIO::readMachineEndian(double& aNumber) {
   this->read((char*)&aNumber, 8);
}



//////////////////////////////
//
// FileIO::readNotMachineEndian --
//	Read numbers from a file with different endian from the computer.
//

void FileIO::readNotMachineEndian(char& aNumber) {
   this->read(&aNumber, 1);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(uchar& aNumber) {
   this->read((char*)&aNumber, 1);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(short& aNumber) {
   this->read((char*)&aNumber, 2);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(ushort& aNumber) {
   this->read((char*)&aNumber, 2);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(long& aNumber) {
   this->read((char*)&aNumber, 4);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(ulong& aNumber) {
   this->read((char*)&aNumber, 4);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(int& aNumber) {
   this->read((char*)&aNumber, 4);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(uint& aNumber) {
   this->read((char*)&aNumber, 4);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(float& aNumber) {
   this->read((char*)&aNumber, 4);
   aNumber = flipBytes(aNumber);
}

void FileIO::readNotMachineEndian(double& aNumber) {
   this->read((char*)&aNumber, 8);
   aNumber = flipBytes(aNumber);
}



//////////////////////////////
//
// FileIO::writeBigEndian --
//

void FileIO::writeBigEndian(char aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(uchar aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(short aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(ushort aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(long aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(ulong aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(int aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(uint aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(float aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}

void FileIO::writeBigEndian(double aNumber) {
   #ifdef OTHEREND
      writeNotMachineEndian(aNumber);
   #else
      writeMachineEndian(aNumber);
   #endif
}



//////////////////////////////
//
// FileIO::writeLittleEndian --
//

void FileIO::writeLittleEndian(char aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(uchar aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(short aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(ushort aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(long aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(ulong aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(int aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(uint aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(float aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}

void FileIO::writeLittleEndian(double aNumber) {
   #ifdef OTHEREND
      writeMachineEndian(aNumber);
   #else
      writeNotMachineEndian(aNumber);
   #endif
}



//////////////////////////////
//
// FileIO::writeMachineEndian --
//

void FileIO::writeMachineEndian(char aNumber) {
   this->write(&aNumber, 1);
}

void FileIO::writeMachineEndian(uchar aNumber) {
   this->write((char*)&aNumber, 1);
}

void FileIO::writeMachineEndian(short aNumber) {
   this->write((char*)&aNumber, 2);
}

void FileIO::writeMachineEndian(ushort aNumber) {
   this->write((char*)&aNumber, 2);
}

void FileIO::writeMachineEndian(long aNumber) {
   this->write((char*)&aNumber, 4);
}

void FileIO::writeMachineEndian(ulong aNumber) {
   this->write((char*)&aNumber, 4);
}

void FileIO::writeMachineEndian(int aNumber) {
   this->write((char*)&aNumber, 4);
}

void FileIO::writeMachineEndian(uint aNumber) {
   this->write((char*)&aNumber, 4);
}

void FileIO::writeMachineEndian(float aNumber) {
   this->write((char*)&aNumber, 4);
}

void FileIO::writeMachineEndian(double aNumber) {
   this->write((char*)&aNumber, 8);
}



//////////////////////////////
//
// FileIO::writeNotMachineEndian --
//

void FileIO::writeNotMachineEndian(char aNumber) {
   // aNumber = flipBytes(aNumber);
   this->write(&aNumber, 1);
}

void FileIO::writeNotMachineEndian(uchar aNumber) {
   // aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 1);
}

void FileIO::writeNotMachineEndian(short aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 2);
}

void FileIO::writeNotMachineEndian(ushort aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 2);
}

void FileIO::writeNotMachineEndian(long aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 4);
}

void FileIO::writeNotMachineEndian(ulong aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 4);
}

void FileIO::writeNotMachineEndian(int aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 4);
}

void FileIO::writeNotMachineEndian(uint aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 4);
}

void FileIO::writeNotMachineEndian(float aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 4);
}

void FileIO::writeNotMachineEndian(double aNumber) {
   aNumber = flipBytes(aNumber);
   this->write((char*)&aNumber, 8);
}


///////////////////////////////////////////////////////////////////////////
// 
// private functions
//


//////////////////////////////
//
// flipBytes -- flip the bytes in a number
//

char FileIO::flipBytes(char aNumber) {
   return aNumber;
}


uchar FileIO::flipBytes(uchar aNumber) {
   return aNumber;
}


short FileIO::flipBytes(short aNumber) {
   uchar input[2];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   return (input[0] << 8) | input[1];
}


ushort FileIO::flipBytes(ushort aNumber) {
   uchar input[2];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   return (input[0] << 8) | input[1];
}


long FileIO::flipBytes(long aNumber) {
   uchar input[4];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   input[2] = (uchar)(0xff & (aNumber >> 16));
   input[3] = (uchar)(0xff & (aNumber >> 24));
   return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
}


ulong FileIO::flipBytes(ulong aNumber) {
   uchar input[4];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   input[2] = (uchar)(0xff & (aNumber >> 16));
   input[3] = (uchar)(0xff & (aNumber >> 24));
   return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
}


int FileIO::flipBytes(int aNumber) {
   uchar input[4];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   input[2] = (uchar)(0xff & (aNumber >> 16));
   input[3] = (uchar)(0xff & (aNumber >> 24));
   return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
}


uint FileIO::flipBytes(uint aNumber) {
   uchar input[4];
   input[0] = (uchar)(0xff & aNumber);
   input[1] = (uchar)(0xff & (aNumber >> 8));
   input[2] = (uchar)(0xff & (aNumber >> 16));
   input[3] = (uchar)(0xff & (aNumber >> 24));
   return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
}
 
   
float FileIO::flipBytes(float aNumber) {
   assert(sizeof(float) >= 4);
   uchar input[4];
   uchar output[4];
   memcpy(input, &aNumber, 4);
   output[0] = input[3];
   output[1] = input[2];
   output[2] = input[1];
   output[3] = input[0];
   double realoutput;
   memcpy(&realoutput, output, 4);
   return realoutput;
}



double FileIO::flipBytes(double aNumber) {
   assert(sizeof(double) >= 8);
   uchar input[8];
   uchar output[8];
   memcpy(input, &aNumber, 8);
   output[0] = input[7];
   output[1] = input[6];
   output[2] = input[5];
   output[3] = input[4];
   output[4] = input[3];
   output[5] = input[2];
   output[6] = input[1];
   output[7] = input[0];
   double realoutput;
   memcpy(&realoutput, output, 8);
   return realoutput;
}



// md5sum: 98791588b78006c4a17c290acb82d15e FileIO.cpp [20050403]
