//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Feb 16 12:26:32 PST 2015 Adapted from binasc program.
// Last Modified: Wed Feb 18 14:48:21 PST 2015
// Filename:      midifile/include/Binasc.cpp
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// description:   Interface to convert bytes between binary and ASCII forms.
//

#ifndef _BINASC_H_INCLUDED
#define _BINASC_H_INCLUDED

#include <iostream>
#include <fstream>

using namespace std;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

class Binasc {
   public:
               Binasc         (void);
              ~Binasc         ();

      // functions for setting options:
      int      setLineLength  (int length);
      int      getLineLength  (void);
      int      setLineBytes   (int length);
      int      getLineBytes   (void);
      void     setComments    (int state);
      void     setCommentsOn  (void);
      void     setCommentsOff (void);
      void     setBytes       (int state);
      void     setBytesOn     (void);
      void     setBytesOff    (void);

      // functions for converting into a binary file:
      void     writeToBinary  (const string& outfile, const string& infile);
      void     writeToBinary  (const string& outfile, istream& input);
      ostream& writeToBinary  (ostream& out, const string& infile);
      ostream& writeToBinary  (ostream& out, istream& input);

      // functions for converting into an ASCII file with hex bytes:
      void     readFromBinary (const string& outfile, const string& infile);
      void     readFromBinary (const string& outfile, istream& input);
      ostream& readFromBinary (ostream& out, const string& infile);
      ostream& readFromBinary (ostream& out, istream& input);

      ostream& outputStyleMidiFile(ostream& out, istream& input);
      int      readMidiEvent  (ostream& out, istream& infile, int& trackbytes, 
                               int& command);
      int      getVLV         (istream& infile, int& trackbytes);

      // static functions for writing ordered bytes:
      static ostream& writeLittleEndianUShort (ostream& out, ushort value);
      static ostream& writeBigEndianUShort    (ostream& out, ushort value);
      static ostream& writeLittleEndianShort  (ostream& out, short  value);
      static ostream& writeBigEndianShort     (ostream& out, short  value);
      static ostream& writeLittleEndianULong  (ostream& out, ulong  value);
      static ostream& writeBigEndianULong     (ostream& out, ulong  value);
      static ostream& writeLittleEndianLong   (ostream& out, long   value);
      static ostream& writeBigEndianLong      (ostream& out, long   value);
      static ostream& writeLittleEndianFloat  (ostream& out, float  value);
      static ostream& writeBigEndianFloat     (ostream& out, float  value);
      static ostream& writeLittleEndianDouble (ostream& out, double value);
      static ostream& writeBigEndianDouble    (ostream& out, double value);

   protected:
      // helper functions for reading ASCII content to conver to binary:
      ostream& processLine        (ostream& out, char* word, int lineNum);
      ostream& processAsciiWord   (ostream& out, const char* word, int lineNum);
      ostream& processBinaryWord  (ostream& out, const char* word, int lineNum);
      ostream& processDecimalWord (ostream& out, const char* word, int lineNum);
      ostream& processHexWord     (ostream& out, const char* word, int lineNum);
      ostream& processVlvWord     (ostream& out, const char* word, int lineNum);
      ostream& processMidiPitchBendWord(ostream& out, const char* word, 
                                   int lineNum);

      // helper functions for reading binary content to convert to ASCII:
      ostream& outputStyleAscii   (ostream& out, istream& input);
      ostream& outputStyleBinary  (ostream& out, istream& input);
      ostream& outputStyleBoth    (ostream& out, istream& input);

   private:
      int bytesQ;        // option for printing hex bytes in ASCII output.
      int commentsQ;     // option for printing comments in ASCII output.
      int maxLineLength; // number of character in ASCII output on a line.
      int maxLineBytes;  // number of hex bytes in ASCII output on a line.
};


#endif /* _BINASC_H_INCLUDED */



