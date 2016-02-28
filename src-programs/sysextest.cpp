//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Mar 19 13:42:37 PDT 2015
// Last Modified: Thu Mar 19 15:34:12 PDT 2015
// Filename:      src-programs/sysextest.cpp
// Syntax:        C++
//
// Description:   Demonstration of how to create a MIDI file with
//                system exclusive messages.  A complete sysex message is
//                given in track 1 and the same sysex messages is given
//                in segmented form in track 2.  MIDI files require the
//                size of the sysex message to be give after the first
//                byte of the message (and the count excludes the first
//                byte.  F7 continuation messages also require a size
//                after the initial message byte.  The MidiFile class
//                transparently handles inserting this size parameter
//                into the MIDI file, so you should not add it yourself.
//
//                For example this sysex message:
//                   f0 43 12 00 43 12 00 42 12 00 43 12 00 f7
//                is stored in the MidiFile object, but when written
//                to a file, the size of the message excluding the first
//                byte is inserted after the first byte:
//                   f0 0d 43 12 00 43 12 00 42 12 00 43 12 00 f7
//                where 0d is decimal 13, which is the number of bytes
//                after the first byte in the sysex message.
//

#include "MidiFile.h"
#include <iostream>

using namespace std;

typedef unsigned char uchar;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   MidiFile outputfile;        // create an empty MIDI file with one track
   outputfile.addTrack(2);     // Add another two tracks to the MIDI file

   vector<uchar> fullsysex(14);
   fullsysex[0]  = 0xf0;
   // message size will automatically be inserted here when
   // writing to a MIDI file.
   fullsysex[1]  = 0x43;
   fullsysex[2]  = 0x12;
   fullsysex[3]  = 0x00;
   fullsysex[4]  = 0x43;
   fullsysex[5]  = 0x12;
   fullsysex[6]  = 0x00;
   fullsysex[7]  = 0x42;
   fullsysex[8]  = 0x12;
   fullsysex[9]  = 0x00;
   fullsysex[10] = 0x43;
   fullsysex[11] = 0x12;
   fullsysex[12] = 0x00;
   fullsysex[13] = 0xf7;

   vector<uchar> startsysex(4);
   startsysex[0] = 0xf0;
   // Partial message size will automatically be inserted here when
   // writing to a MIDI file (size = 3 bytes).
   startsysex[1] = 0x43;
   startsysex[2] = 0x12;
   startsysex[3] = 0x00;

   vector<uchar> midsysex(7);
   midsysex[0]   = 0xf7;      // sysex continuation marker (not part of msg.)
   // Partial message size will automatically be inserted here when
   // writing to a MIDI file (size = 6 bytes).
   midsysex[1]   = 0x43;
   midsysex[2]   = 0x12;
   midsysex[3]   = 0x00;
   midsysex[4]   = 0x42;
   midsysex[5]   = 0x12;
   midsysex[6]   = 0x00;

   vector<uchar> endsysex(5);
   endsysex[0]   = 0xf7;      // sysex continuation marker (not part of msg.)
   // Partial message size will automatically be inserted here when
   // writing to a MIDI file (size = 4 bytes).
   endsysex[1]   = 0x43;
   endsysex[2]   = 0x12;
   endsysex[3]   = 0x00;
   endsysex[4]   = 0xf7;      // sysex termination marker (part of msg.)

   outputfile.setTicksPerQuarterNote(100);
   outputfile.addEvent(1,   0, fullsysex );
   outputfile.addEvent(2,   0, startsysex);
   outputfile.addEvent(2, 100, midsysex  );
   outputfile.addEvent(2, 200, endsysex  );

   outputfile.writeBinasc(cout);
   //outputfile.writeHex(cout);
   return 0;
}

/*
 * The expected output from this program when using the writeBinasc()
 * method demonstrates how the length of the system exclusive message
 * is embedded into the MIDI file after the first byte in the message.
 *
 * for example the first message which is 14 bytes long:
 *      f0 43 12 00 43 12 00 42 12 00 43 12 00 f7
 * has an extra byte added for the length of the message after the first
 * byte (0d = 13 bytes):
 *      f0 0d 43 12 00 43 12 00 42 12 00 43 12 00 f7
 * This is a Variable Length Value, so the binasc parsed MIDI file uses
 * this format:
 *      f0 v13 43 12 00 43 12 00 42 12 00 43 12 00 f7
 * where v13 is a VLV value representing the integer 13.
 *

+M +T +h +d
4'6
2'1
2'3
2'100

; TRACK 0 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'4
v0	ff 2f v0

; TRACK 1 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'20
v0	f0 v13 43 12 00 43 12 00 42 12 00 43 12 00 f7
v0	ff 2f v0

; TRACK 2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
+M +T +r +k
4'26
v0	f0 v3 43 12 00
v100	f7 v6 43 12 00 42 12 00
v100	f7 v4 43 12 00 f7
v0	ff 2f v0


In hex byte code format (outputfile.writeHex()), here is the expected
output content:

4d 54 68 64 00 00 00 06 00 01 00 03 00 64 4d 54 72 6b 00 00 00 04 00 ff 2f
00 4d 54 72 6b 00 00 00 14 00 f0 0d 43 12 00 43 12 00 42 12 00 43 12 00 f7
00 ff 2f 00 4d 54 72 6b 00 00 00 1a 00 f0 03 43 12 00 64 f7 06 43 12 00 42
12 00 64 f7 04 43 12 00 f7 00 ff 2f 00


Manual parsing of the hex byte codes to make output similar to the binasc
format above:

4d 54 68 64
00 00 00 06
00 01
00 03
00 64

4d 54 72 6b
00 00 00 04
00 ff 2f 00

4d 54 72 6b
00 00 00 14
00 f0 0d 43 12 00 43 12 00 42 12 00 43 12 00 f7
00 ff 2f 00

4d 54 72 6b
00 00 00 1a
00 f0 03 43 12 00
64 f7 06 43 12 00 42 12 00
64 f7 04 43 12 00 f7
00 ff 2f 00


 *
 *
 */



