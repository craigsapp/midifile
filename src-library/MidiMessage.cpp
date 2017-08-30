//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Feb 14 20:49:21 PST 2015
// Last Modified: Sat Feb 14 21:40:31 PST 2015
// Filename:      midifile/src-library/MidiMessage.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   Storage for bytes of a MIDI message for Standard
//                MIDI Files.
//

#include "MidiMessage.h"

#include <vector>
#include <iostream>
#include <iterator>

using namespace std;


//////////////////////////////
//
// MidiMessage::MidiMessage -- Constructor.
//

MidiMessage::MidiMessage(void) {
   // do nothing
}


MidiMessage::MidiMessage(int command) {
   this->resize(1);
   (*this)[0] = (uchar)command;
}


MidiMessage::MidiMessage(int command, int p1) {
   this->resize(2);
   (*this)[0] = (uchar)command;
   (*this)[1] = (uchar)p1;
}


MidiMessage::MidiMessage(int command, int p1, int p2) {
   this->resize(3);
   (*this)[0] = (uchar)command;
   (*this)[1] = (uchar)p1;
   (*this)[2] = (uchar)p2;
}


MidiMessage::MidiMessage(const MidiMessage& message) {
   (*this) = message;
}


MidiMessage::MidiMessage(const vector<uchar>& message) {
   setMessage(message);
}


MidiMessage::MidiMessage(const vector<char>& message) {
   setMessage(message);
}


MidiMessage::MidiMessage(const vector<int>& message) {
   setMessage(message);
}



//////////////////////////////
//
// MidiMessage::~MidiMessage -- Deconstructor.
//

MidiMessage::~MidiMessage() {
   resize(0);
}



//////////////////////////////
//
// MidiMessage::operator= --
//

MidiMessage& MidiMessage::operator=(const MidiMessage& message) {
   if (this == &message) {
      return *this;
   }
   (*this) = message;
   return *this;
}


MidiMessage& MidiMessage::operator=(const vector<uchar>& bytes) {
   if (this == &bytes) {
      return *this;
   }
   setMessage(bytes);
   return *this;
}


MidiMessage& MidiMessage::operator=(const vector<char>& bytes) {
   setMessage(bytes);
   return *this;
}


MidiMessage& MidiMessage::operator=(const vector<int>& bytes) {
   setMessage(bytes);
   return *this;
}



//////////////////////////////
//
// MidiMessage::setSize -- Change the size of the message byte list.
//

void MidiMessage::setSize(int asize) {
   this->resize(asize);
}



//////////////////////////////
//
// MidiMessage::getSize -- Return the size of the MIDI message bytes.
//

int MidiMessage::getSize(void) const {
   return (int)this->size();
}



//////////////////////////////
//
// MidiMessage::setSizeToCommand -- Set the number of parameters if the
//   command byte is set in the range from 0x80 to 0xef.  Any newly
//   added parameter bytes will be set to 0.
//

int MidiMessage::setSizeToCommand(void) {
   int osize = (int)this->size();
   if (osize < 1) {
      return 0;
   }
   int command = getCommandNibble();
   if (command < 0) {
      return 0;
   }
   int bytecount = 1;
   switch (command) {
      case 0x80: bytecount = 2; break;  // Note Off
      case 0x90: bytecount = 2; break;  // Note On
      case 0xA0: bytecount = 2; break;  // Aftertouch
      case 0xB0: bytecount = 2; break;  // Continuous Controller
      case 0xC0: bytecount = 1; break;  // Patch Change
      case 0xD0: bytecount = 1; break;  // Channel Pressure
      case 0xE0: bytecount = 2; break;  // Pitch Bend
      case 0xF0:
      default:
         return 0;
   }
   if (bytecount + 1 < osize) {
      resize(bytecount+1);
      for (int i=osize; i<bytecount+1; i++) {
         (*this)[i] = 0;
      }
   }

   return (int)size();
}


int MidiMessage::resizeToCommand(void) {
   return setSizeToCommand();
}



//////////////////////////////
//
// MidiMessage::getTempoMicro -- Returns the number of microseconds per
//      quarter note if the MidiMessage is a tempo meta message.
//      Returns -1 if the MIDI message is not a tempo meta message.
//

int MidiMessage::getTempoMicro(void) const {
   if (!isTempo()) {
      return -1;
   } else {
      return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
   }
}


int MidiMessage::getTempoMicroseconds(void) const {
   return getTempoMicro();
}



//////////////////////////////
//
// MidiMessage::getTempoSeconds -- Returns the number of seconds per
//      quarter note.  Returns -1.0 if the MIDI message is not a
//      tempo meta message.
//

double MidiMessage::getTempoSeconds(void) const {
   int microseconds = getTempoMicroseconds();
   if (microseconds < 0) {
      return -1.0;
   } else {
      return (double)microseconds / 1000000.0;
   }
}



//////////////////////////////
//
// MidiMessage::getTempoBPM -- Returns the tempo in terms of beats per minute.
//   Returns -1 if the MidiMessage is note a tempo meta message.
//

double MidiMessage::getTempoBPM(void) const {
   int microseconds = getTempoMicroseconds();
   if (microseconds < 0) {
      return -1.0;
   }
   return 60000000.0 / (double)microseconds;
}



//////////////////////////////
//
// MidiMessage::getTempoTPS -- Returns the tempo in terms of ticks per seconds.
//

double MidiMessage::getTempoTPS(int tpq) const {
   int microseconds = getTempoMicroseconds();
   if (microseconds < 0) {
      return -1.0;
   } else {
      return tpq * 1000000.0 / (double)microseconds;
   }
}



//////////////////////////////
//
// MidiMessage::getTempoSPT -- Returns the tempo in terms of seconds per tick.
//

double MidiMessage::getTempoSPT(int tpq) const {
   int microseconds = getTempoMicroseconds();
   if (microseconds < 0) {
      return -1.0;
   } else {
      return (double)microseconds / 1000000.0 / tpq;
   }
}



//////////////////////////////
//
// MidiMessage::isMeta -- Returns true if message is a Meta message
//      (when the command byte is 0xff).
//

int MidiMessage::isMeta(void) const {
   if (size() == 0) {
      return 0;
   } else if ((*this)[0] != 0xff) {
      return 0;
   } else if (size() < 3) {
      // meta message is ill-formed.
      // meta messages must have at least three bytes:
      //    0: 0xff == meta message marker
      //    1: meta message type
      //    2: meta message data bytes to follow
      return 0;
   } else {
      return 1;
   }
}


int MidiMessage::isMetaMessage(void) const {
   return isMeta();
}



//////////////////////////////
//
// MidiMessage::isNoteOff -- Returns true if the command nibble is 0x80
//     or if the command nibble is 0x90 with p2=0 velocity.
//

int MidiMessage::isNoteOff(void) const {
   if (size() != 3) {
      return 0;
   } else if (((*this)[0] & 0xf0) == 0x80) {
      return 1;
   } else if ((((*this)[0] & 0xf0) == 0x90) && ((*this)[2] == 0)) {
      return 1;
   } else {
      return 0;
   }
}



//////////////////////////////
//
// MidiMessage::isNoteOn -- Returns true if the command byte is in the 0x90
//    range and the velocity is non-zero
//

int MidiMessage::isNoteOn(void) const {
   if (size() != 3) {
      return 0;
   } else if (((*this)[0] & 0xf0) != 0x90) {
      return 0;
   } else if ((*this)[2] == 0) {
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isNote -- Returns true if either a note-on or a note-off
//     message.
//

int MidiMessage::isNote(void) const {
   return isNoteOn() || isNoteOff();
}



//////////////////////////////
//
// MidiMessage::isAftertouch -- Returns true if the command byte is in the 0xA0
//    range.
//

int MidiMessage::isAftertouch(void) const {
   if (size() != 3) {
      return 0;
   } else if (((*this)[0] & 0xf0) != 0xA0) {
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isController -- Returns true if the command byte is in the 0xB0
//    range.
//

int MidiMessage::isController(void) const {
   if (size() != 3) {
      return 0;
   } else if (((*this)[0] & 0xf0) != 0xB0) {
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isTimbre -- Returns true of a patch change message
//    (command nibble 0xc0).
//

int MidiMessage::isTimbre(void) const {
   if (((*this)[0] & 0xf0) != 0xc0) {
      return 0;
   } else if (size() != 2) {
      return 0;
   } else {
      return 1;
   }
}


int MidiMessage::isPatchChange(void) const {
   return isTimbre();
}



//////////////////////////////
//
// MidiMessage::isPressure -- Returns true of a channel pressure message
//    (command nibble 0xd0).
//

int MidiMessage::isPressure(void) const {
   if (((*this)[0] & 0xf0) != 0xd0) {
      return 0;
   } else if (size() != 2) {
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isPitchbend -- Returns true of a pitch-bend message
//    (command nibble 0xe0).
//

int MidiMessage::isPitchbend(void) const {
   if (((*this)[0] & 0xf0) != 0xe0) {
      return 0;
   } else if (size() != 3) {
      return 0;
   } else {
      return 1;
   }
}



///////////////////////////////
//
// MidiMessage::getMetaType -- returns the meta-message type for the
//     MidiMessage.  If the message is not a meta message, then returns
//     -1.
//

int MidiMessage::getMetaType(void) const {
   if (!isMetaMessage()) {
      return -1;
   } else {
      return (int)(*this)[1];
   }
}

///////////////////////////////
//
// MidiMessage::getMetaData -- returns the meta-message data for the
//     MidiMessage.  If the message is not a meta message, then returns
//     an empty string
//

string MidiMessage::getMetaData() const
{
   if (!isMetaMessage() || size() < 4) {
	 return string();
   } else {
	  return std::string(begin() + 3, end());
   }
}



//////////////////////////////
//
// MidiMessage::isTempo -- Returns true if message is a meta message
//      describing tempo (meta message type 0x51).
//

int MidiMessage::isTempo(void) const {
   if (!isMetaMessage()) {
      return 0;
   } else if ((*this)[1] != 0x51) {
      return 0;
   } else if (size() != 6) {
      // Meta tempo message can only be 6 bytes long.
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isEndOfTrack -- Returns true if message is a meta message
//      for end-of-track (meta message type 0x2f).
//

int MidiMessage::isEndOfTrack(void) const {
   return getMetaType() == 0x2f ? 1 : 0;
}



//////////////////////////////
//
// MidiMessage::getP0 -- Return index 1 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP0(void) const {
   return size() < 1 ? -1 : (*this)[0];
}



//////////////////////////////
//
// MidiMessage::getP1 -- Return index 1 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP1(void) const {
   return size() < 2 ? -1 : (*this)[1];
}



//////////////////////////////
//
// MidiMessage::getP2 -- Return index 2 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP2(void) const {
   return size() < 3 ? -1 : (*this)[2];
}



//////////////////////////////
//
// MidiMessage::getP3 -- Return index 3 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP3(void) const {
   return size() < 4 ? -1 : (*this)[3];
}



//////////////////////////////
//
// MidiMessage::getKeyNumber -- Return the key number (such as 60 for
//    middle C).  If the message does not have a note parameter, then
//    return -1;  if the key is invalid (above 127 in value), then
//    limit to the range 0 to 127.
//

int MidiMessage::getKeyNumber(void) const {
   if (isNote() || isAftertouch()) {
      int output = getP1();
      if (output < 0) {
         return output;
      } else {
         return 0xff & output;
      }
   } else {
      return -1;
   }
}



//////////////////////////////
//
// MidiMessage::getVelocity -- Return the key veolocity.  If the message
//   is not a note-on or a note-off, then return -1.  If the value is
//   out of the range 0-127, then chop off the high-bits.
//

int MidiMessage::getVelocity(void) const {
   if (isNote()) {
      int output = getP2();
      if (output < 0) {
         return output;
      } else {
         return 0xff & output;
      }
   } else {
      return -1;
   }
}



//////////////////////////////
//
// MidiMessage::setP0 -- Set the command byte.
//   If the MidiMessage is too short, add extra spaces to
//   allow for P0.  The value should be in the range from
//   128 to 255, but this function will not babysit you.
//

void MidiMessage::setP0(int value) {
   if (getSize() < 1) {
      resize(1);
   }
   (*this)[0] = value;
}



//////////////////////////////
//
// MidiMessage::setP1 -- Set the first parameter value.
//   If the MidiMessage is too short, add extra spaces to
//   allow for P1.  The command byte will be undefined if
//   it was added.  The value should be in the range from
//   0 to 127, but this function will not babysit you.
//

void MidiMessage::setP1(int value) {
   if (getSize() < 2) {
      resize(2);
   }
   (*this)[1] = value;
}



//////////////////////////////
//
// MidiMessage::setP2 -- Set the second paramter value.
//     If the MidiMessage is too short, add extra spaces
//     to allow for P2.  The command byte and/or the P1 value
//     will be undefined if extra space needs to be added and
//     those slots are created.  The value should be in the range
//     from 0 to 127, but this function will not babysit you.
//

void MidiMessage::setP2(int value) {
   if (getSize() < 3) {
      resize(3);
   }
   (*this)[2] = value;
}



//////////////////////////////
//
// MidiMessage::setP3 -- Set the third paramter value.
//     If the MidiMessage is too short, add extra spaces
//     to allow for P3.  The command byte and/or the P1/P2 values
//     will be undefined if extra space needs to be added and
//     those slots are created.  The value should be in the range
//     from 0 to 127, but this function will not babysit you.
//

void MidiMessage::setP3(int value) {
   if (getSize() < 4) {
      resize(4);
   }
   (*this)[3] = value;
}



//////////////////////////////
//
// MidiMessage::setKeyNumber -- Set the note on/off key number (or
//    aftertouch key).  Ignore if not note or aftertouch message.
//    Limits the input value to the range from 0 to 127.
//

void MidiMessage::setKeyNumber(int value) {
   if (isNote() || isAftertouch()) {
      setP1(value & 0xff);
   } else {
      // don't do anything since this is not a note-related message.
   }
}



//////////////////////////////
//
// MidiMessage::setVelocity -- Set the note on/off velocity; ignore
//   if note a note message.  Limits the input value to the range
//   from 0 to 127.
//

void MidiMessage::setVelocity(int value) {
   if (isNote()) {
      setP2(value & 0xff);
   } else {
      // don't do anything since this is not a note-related message.
   }
}



//////////////////////////////
//
// MidiMessage::getCommandNibble -- Returns the top 4 bits of the (*this)[0]
//    entry, or -1 if there is not (*this)[0].
//

int MidiMessage::getCommandNibble(void) const {
   if (size() < 1) {
      return -1;
   } else {
      return (*this)[0] & 0xf0;
   }
}



//////////////////////////////
//
// MidiMessage::getCommandByte -- Return the command byte or -1 if not
//    allocated.
//

int MidiMessage::getCommandByte(void) const {
   if (size() < 1) {
      return -1;
   } else {
      return (*this)[0];
   }
}



//////////////////////////////
//
// MidiMessage::getChannelNibble -- Returns the bottom 4 bites of the
//      (*this)[0] entry, or -1 if there is not (*this)[0].  Should be refined
//      to return -1 if the top nibble is 0xf0, since those commands are
//      not channel specific.
//

int MidiMessage::getChannelNibble(void) const {
   if (size() < 1) {
      return -1;
   } else {
      return (*this)[0] & 0x0f;
   }
}


int MidiMessage::getChannel(void) const {
   return getChannelNibble();
}



//////////////////////////////
//
// MidiMessage::setCommandByte --
//

void MidiMessage::setCommandByte(int value) {
   if (size() < 1) {
      resize(1);
   } else {
      (*this)[0] = (uchar)(value & 0xff);
   }
}

void MidiMessage::setCommand(int value) {
   setCommandByte(value);
}



//////////////////////////////
//
// MidiMessage::setCommand -- Set the command byte and parameter bytes
//   for a MidiMessage.  The size of the message will be adjusted to
//   the number of input parameters.
//

void MidiMessage::setCommand(int value, int p1) {
   this->resize(2);
   (*this)[0] = (uchar)value;
   (*this)[1] = (uchar)p1;
}


void MidiMessage::setCommand(int value, int p1, int p2) {
   this->resize(3);
   (*this)[0] = (uchar)value;
   (*this)[1] = (uchar)p1;
   (*this)[2] = (uchar)p2;
}



//////////////////////////////
//
// MidiMessage::setCommandNibble --
//

void MidiMessage::setCommandNibble(int value) {
   if (this->size() < 1) {
      this->resize(1);
   }
   if (value <= 0x0f) {
      (*this)[0] = ((*this)[0] & 0x0f) | ((uchar)((value << 4) & 0xf0));
   } else {
      (*this)[0] = ((*this)[0] & 0x0f) | ((uchar)(value & 0xf0));
   }
}




//////////////////////////////
//
// MidiMessage::setChannelNibble --
//

void MidiMessage::setChannelNibble(int value) {
   if (this->size() < 1) {
      this->resize(1);
   }
   (*this)[0] = ((*this)[0] & 0xf0) | ((uchar)(value & 0x0f));
}


void MidiMessage::setChannel(int value) {
   setChannelNibble(value);
}



//////////////////////////////
//
// MidiMessage::setParameters -- Set the second and optionally the
//     third MIDI byte of a MIDI message.  The command byte will not
//     be altered, and will be set to 0 if it currently does not exist.
//

void MidiMessage::setParameters(int p1) {
   int oldsize = (int)size();
   resize(2);
   (*this)[1] = (uchar)p1;
   if (oldsize < 1) {
      (*this)[0] = 0;
   }
}


void MidiMessage::setParameters(int p1, int p2) {
   int oldsize = (int)size();
   resize(3);
   (*this)[1] = (uchar)p1;
   (*this)[2] = (uchar)p2;
   if (oldsize < 1) {
      (*this)[0] = 0;
   }
}


//////////////////////////////
//
// MidiMessage::setMessage --  Set the contents of MIDI bytes to the
//   input list of bytes.
//

void MidiMessage::setMessage(const vector<uchar>& message) {
   this->resize(message.size());
   for (int i=0; i<(int)this->size(); i++) {
      (*this)[i] = message[i];
   }
}


void MidiMessage::setMessage(const vector<char>& message) {
   resize(message.size());
   for (int i=0; i<(int)size(); i++) {
      (*this)[i] = (uchar)message[i];
   }
}


void MidiMessage::setMessage(const vector<int>& message) {
   resize(message.size());
   for (int i=0; i<(int)size(); i++) {
      (*this)[i] = (uchar)message[i];
   }
}



//////////////////////////////
//
// MidiMessage::setSpelling -- Encode a MidiPlus accidental state for a note.
//    For example, if a note's key number is 60, the enharmonic pitch name
//    could be any of these possibilities:
//        C, B-sharp, D-double-flat
//    MIDI note 60 is ambiguous as to which of these names are intended,
//    so MIDIPlus allows these mappings to be preserved for later recovery.
//    See Chapter 5 (pp. 99-104) of Beyond MIDI (1997).
//
//    The first parameter is the diatonic pitch number (or pitch class
//    if the octave is set to 0):
//       octave * 7 + 0 = C pitches
//       octave * 7 + 1 = D pitches
//       octave * 7 + 2 = E pitches
//       octave * 7 + 3 = F pitches
//       octave * 7 + 4 = G pitches
//       octave * 7 + 5 = A pitches
//       octave * 7 + 6 = B pitches
//
//    The second parameter is the semitone alteration (accidental).
//    0 = natural state, 1 = sharp, 2 = double sharp, -1 = flat,
//    -2 = double flat.
//
//    Only note-on messages can be processed (other messages will be
//    silently ignored).
//

void MidiMessage::setSpelling(int base7, int accidental) {
   if (!isNoteOn()) {
      return;
   }
   // The bottom two bits of the attack velocity are used for the
   // spelling, so need to make sure the velocity will not accidentally
   // be set to zero (and make the note-on a note-off).
   if (getVelocity() < 4) {
      setVelocity(4);
   }
   int dpc = base7 % 7;
   uchar spelling = 0;

   // Table 5.1, page 101 in Beyond MIDI (1997)
   // http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
   switch (dpc) {

      case 0:
         switch (accidental) {
            case -2: spelling = 1; break; // Cbb
            case -1: spelling = 1; break; // Cb
            case  0: spelling = 2; break; // C
            case +1: spelling = 2; break; // C#
            case +2: spelling = 3; break; // C##
        }
        break;

      case 1:
         switch (accidental) {
            case -2: spelling = 1; break; // Dbb
            case -1: spelling = 1; break; // Db
            case  0: spelling = 2; break; // D
            case +1: spelling = 3; break; // D#
            case +2: spelling = 3; break; // D##
        }
        break;

      case 2:
         switch (accidental) {
            case -2: spelling = 1; break; // Ebb
            case -1: spelling = 2; break; // Eb
            case  0: spelling = 2; break; // E
            case +1: spelling = 3; break; // E#
            case +2: spelling = 3; break; // E##
        }
        break;

      case 3:
         switch (accidental) {
            case -2: spelling = 1; break; // Fbb
            case -1: spelling = 1; break; // Fb
            case  0: spelling = 2; break; // F
            case +1: spelling = 2; break; // F#
            case +2: spelling = 3; break; // F##
            case +3: spelling = 3; break; // F###
        }
        break;

      case 4:
         switch (accidental) {
            case -2: spelling = 1; break; // Gbb
            case -1: spelling = 1; break; // Gb
            case  0: spelling = 2; break; // G
            case +1: spelling = 2; break; // G#
            case +2: spelling = 3; break; // G##
        }
        break;

      case 5:
         switch (accidental) {
            case -2: spelling = 1; break; // Abb
            case -1: spelling = 1; break; // Ab
            case  0: spelling = 2; break; // A
            case +1: spelling = 3; break; // A#
            case +2: spelling = 3; break; // A##
        }
        break;

      case 6:
         switch (accidental) {
            case -2: spelling = 1; break; // Bbb
            case -1: spelling = 2; break; // Bb
            case  0: spelling = 2; break; // B
            case +1: spelling = 3; break; // B#
            case +2: spelling = 3; break; // B##
        }
        break;

   }

   uchar vel = getVelocity();
   // suppress any previous content in the first two bits:
   vel = vel & 0xFC;
   // insert the spelling code:
   vel = vel | spelling;
   setVelocity(vel);
}



//////////////////////////////
//
// MidiMessage::getSpelling -- Return the diatonic pitch class and accidental
//    for a note-on's key number.  The MIDI file must be encoded with MIDIPlus
//    pitch spelling codes for this function to return valid data; otherwise,
//    it will return a neutral fixed spelling for each MIDI key.
//
//    The first parameter will be filled in with the base-7 diatonic pitch:
//        pc + octave * 7
//     where pc is the numbers 0 through 6 representing the pitch classes
//     C through B, the octave is MIDI octave (not the scientific pitch
//     octave which is one less than the MIDI ocatave, such as C4 = middle C).
//     The second number is the accidental for the base-7 pitch.
//

void MidiMessage::getSpelling(int& base7, int& accidental) {
   if (!isNoteOn()) {
      return;
   }
   base7 = -123456;
   accidental = 123456;
   int base12   = getKeyNumber();
   int octave   = base12 / 12;
   int base12pc = base12 - octave * 12;
   int base7pc  = 0;
   int spelling = 0x03 & getVelocity();

   // Table 5.1, page 101 in Beyond MIDI (1997)
   // http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
   switch (base12pc) {

      case 0:
         switch (spelling) {
                    case 1: base7pc = 1; accidental = -2; break;  // Dbb
            case 0: case 2: base7pc = 0; accidental =  0; break;  // C
                    case 3: base7pc = 6; accidental = +1; octave--; break;  // B#
         }
         break;

      case 1:
         switch (spelling) {
                    case 1: base7pc = 1; accidental = -1; break;  // Db
            case 0: case 2: base7pc = 0; accidental = +1; break;  // C#
                    case 3: base7pc = 6; accidental = +2; octave--; break;  // B##
         }
         break;

      case 2:
         switch (spelling) {
                    case 1: base7pc = 2; accidental = -2; break;  // Ebb
            case 0: case 2: base7pc = 1; accidental =  0; break;  // D
                    case 3: base7pc = 0; accidental = +2; break;  // C##
         }
         break;

      case 3:
         switch (spelling) {
                    case 1: base7pc = 3; accidental = -2; break;  // Fbb
            case 0: case 2: base7pc = 2; accidental = -1; break;  // Eb
                    case 3: base7pc = 1; accidental = +1; break;  // D#
         }
         break;

      case 4:
         switch (spelling) {
                    case 1: base7pc = 3; accidental = -1; break;  // Fb
                    case 2: base7pc = 2; accidental =  0; break;  // E
                    case 3: base7pc = 1; accidental = +2; break;  // D##
         }
         break;

      case 5:
         switch (spelling) {
                    case 1: base7pc = 4; accidental = -2; break;  // Gbb
            case 0: case 2: base7pc = 3; accidental =  0; break;  // F
                    case 3: base7pc = 2; accidental = +1; break;  // E#
         }
         break;

      case 6:
         switch (spelling) {
                    case 1: base7pc = 4; accidental = -1; break;  // Gb
            case 0: case 2: base7pc = 3; accidental = +1; break;  // F#
                    case 3: base7pc = 2; accidental = +2; break;  // E##
         }
         break;

      case 7:
         switch (spelling) {
                    case 1: base7pc = 5; accidental = -2; break;  // Abb
            case 0: case 2: base7pc = 4; accidental =  0; break;  // G
                    case 3: base7pc = 3; accidental = +2; break;  // F##
         }
         break;

      case 8:
         switch (spelling) {
                    case 1: base7pc = 5; accidental = -1; break;  // Ab
            case 0: case 2: base7pc = 4; accidental = +1; break;  // G#
                    case 3: base7pc = 3; accidental = +3; break;  // F###
         }
         break;

      case 9:
         switch (spelling) {
                    case 1: base7pc = 6; accidental = -2; break;  // Bbb
            case 0: case 2: base7pc = 5; accidental =  0; break;  // A
                    case 3: base7pc = 4; accidental = +2; break;  // G##
         }
         break;

      case 10:
         switch (spelling) {
                    case 1: base7pc = 0; accidental = -2; octave++; break;  // Cbb
            case 0: case 2: base7pc = 6; accidental = -1; break;  // Bb
                    case 3: base7pc = 5; accidental = +1; break;  // A#
         }
         break;

      case 11:
         switch (spelling) {
                    case 1: base7pc = 0; accidental = -1; octave++; break;  // Cb
            case 0: case 2: base7pc = 6; accidental =  0; break;  // B
                    case 3: base7pc = 5; accidental = +2; break;  // A##
         }
         break;

   }

   base7 = base7pc + 7 * octave;
}



//////////////////////////////
//
// MidiMessage::setMetaTempo -- Input tempo is in quarter notes per minute
//   (meta message #0x51).
//

void MidiMessage::setMetaTempo(double tempo) {
   int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
   setTempoMicroseconds(microseconds);
}



//////////////////////////////
//
// MidiMessage::setTempo -- Alias for MidiMessage::setMetaTempo().
//

void MidiMessage::setTempo(double tempo) {
   setMetaTempo(tempo);
}



//////////////////////////////
//
// MidiMessage::setTempoMicroseconds -- Set the tempo in terms
//   of microseconds per quarter note.
//

void MidiMessage::setTempoMicroseconds(int microseconds) {
   resize(6);
   (*this)[0] = 0xff;
   (*this)[1] = 0x51;
   (*this)[2] = 3;
   (*this)[3] = (microseconds >> 16) & 0xff;
   (*this)[4] = (microseconds >>  8) & 0xff;
   (*this)[5] = (microseconds >>  0) & 0xff;
}



//////////////////////////////
//
// MidiMessage::makeTimeSignature -- create a time signature meta message
//      (meta #0x58).  The "bottom" parameter should be a power of two;
//      otherwise, it will be forced to be the next highest power of two,
//      as MIDI time signatures must have a power of two in the denominator.
//
// Default values:
//     clocksPerClick     == 24 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//
// Time signature of 4/4 would be:
//    top    = 4
//    bottom = 4 (converted to 2 in the MIDI file for 2nd power of 2).
//    clocksPerClick = 24 (2 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//
// Time signature of 6/8 would be:
//    top    = 6
//    bottom = 8 (converted to 3 in the MIDI file for 3rd power of 2).
//    clocksPerClick = 36 (3 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//

void MidiMessage::makeTimeSignature(int top, int bottom, int clocksPerClick,
      int num32ndsPerQuarter) {
   int base2 = 0;
   while (bottom >>= 1) base2++;
   resize(7);
   (*this)[0] = 0xff;
   (*this)[1] = 0x58;
   (*this)[2] = 4;
   (*this)[3] = 0xff & top;
   (*this)[4] = 0xff & base2;
   (*this)[5] = 0xff & clocksPerClick;
   (*this)[6] = 0xff & num32ndsPerQuarter;
}



///////////////////////////////////////////////////////////////////////////
//
// make functions to create various MIDI message --
//


//////////////////////////////
//
// MidiMessage::makeNoteOn -- create a note-on message.
//
// default value: channel = 0
//
// Note: The channel parameter used to be last, but makes more sense to
//   have it first...
//

void MidiMessage::makeNoteOn(int channel, int key, int velocity) {
   resize(3);
   (*this)[0] = 0x90 | (0x0f & channel);
   (*this)[1] = key & 0x7f;
   (*this)[2] = velocity & 0x7f;
}



//////////////////////////////
//
// MidiMessage::makeNoteOff -- create a note-off message.   If no
//   parameters are given, the current contents is presumed to be a
//   note-on message, which will be converted into a note-off message.
//
// default value: channel = 0
//
// Note: The channel parameter used to be last, but makes more sense to
//   have it first...
//


void MidiMessage::makeNoteOff(int channel, int key, int velocity) {
   resize(3);
   (*this)[0] = 0x80 | (0x0f & channel);
   (*this)[1] = key & 0x7f;
   (*this)[2] = velocity & 0x7f;
}


void MidiMessage::makeNoteOff(int channel, int key) {
   resize(3);
   (*this)[0] = 0x90 | (0x0f & channel);
   (*this)[1] = key & 0x7f;
   (*this)[2] = 0x00;
}


void MidiMessage::makeNoteOff(void) {
   if (!isNoteOn()) {
      resize(3);
      (*this)[0] = 0x90;
      (*this)[1] = 0;
      (*this)[2] = 0;
   } else {
      (*this)[2] = 0;
   }
}



/////////////////////////////
//
// MidiMessage::makeController -- Create a controller message.
//

void MidiMessage::makeController(int channel, int num, int value) {
   resize(0);
   push_back(0xb0 | (0x0f & channel));
   push_back(0x7f & num);
   push_back(0x7f & value);
}



/////////////////////////////
//
// MidiMessage::makePatchChange -- Create a patch-change message.
//

void MidiMessage::makePatchChange(int channel, int patchnum) {
   resize(0);
   push_back(0xc0 | (0x0f & channel));
   push_back(0x7f & patchnum);
}


void MidiMessage::makeTimbre(int channel, int patchnum) {
   makePatchChange(channel, patchnum);
}



//////////////////////////////
//
// MidiMessage::makeMetaMessage -- Create a Meta event with the
//   given text string as the parameter.  The length of the string should
//   not be longer than 127 characters at the moment (will have to check
//   if VLV sizes are allowed).
//

void MidiMessage::makeMetaMessage(int mnum, const string& data) {
   resize(0);
   push_back(0xff);
   push_back(mnum & 0x7f); // I think max is 0x7f.
   int dsize = (int)data.size();
   if (dsize > 127) {
      push_back(127);
      for (int i=0; i<128; i++) {
         push_back(data[i]);
      }
   } else {
      push_back(data.size());
      std::copy(data.begin(), data.end(), std::back_inserter(*this));
   }
}



//////////////////////////////
//
// MidiMessage::makeCopyright -- Create a metaevent copyright message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeCopyright(const string& text) {
   makeMetaMessage(0x02, text);
}



//////////////////////////////
//
// MidiMessage::makeTrackName -- Create a metaevent track name message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeTrackName(const string& name) {
   makeMetaMessage(0x03, name);
}



//////////////////////////////
//
// MidiMessage::makeTrackName -- Create a metaevent instrument name message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeInstrumentName(const string& name) {
   makeMetaMessage(0x04, name);
}



//////////////////////////////
//
// MidiMessage::makeLyric -- Create a metaevent lyrics/text message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeLyric(const string& text) {
   makeMetaMessage(0x05, text);
}



//////////////////////////////
//
// MidiMessage::makeMarker -- Create a metaevent marker message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeMarker(const string& text) {
   makeMetaMessage(0x06, text);
}



//////////////////////////////
//
// MidiMessage::makeCue -- Create a metaevent cue-point message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeCue(const string& text) {
   makeMetaMessage(0x07, text);
}



