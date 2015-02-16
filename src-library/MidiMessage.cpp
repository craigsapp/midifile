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


MidiMessage::MidiMessage(MidiMessage& message) {
   (*this) = message;
}


MidiMessage::MidiMessage(vector<uchar>& message) {
   (*this) = message;
}


MidiMessage::MidiMessage(vector<char>& message) {
   resize(message.size());
   for (int i=0; i<size(); i++)  {
      (*this)[i] = (uchar)message[i];
   }
}


MidiMessage::MidiMessage(vector<int>& message) {
   resize(message.size());
   for (int i=0; i<size(); i++)  {
      (*this)[i] = (uchar)message[i];
   }
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

MidiMessage& MidiMessage::operator=(MidiMessage& message) {
   if (this == &message) {
      return *this;
   }
   (*this) = message;
   return *this;
}


MidiMessage& MidiMessage::operator=(vector<uchar>& bytes) {
   if (this == &bytes) {
      return *this;
   }
   (*this) = bytes;
   return *this;
}


MidiMessage& MidiMessage::operator=(vector<char>& bytes) {
   setMessage(bytes);
   return *this;
}


MidiMessage& MidiMessage::operator=(vector<int>& bytes) {
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

int MidiMessage::getSize(void) {
   return this->size();
}



//////////////////////////////
//
// MidiMessage::setSizeToCommand -- Set the number of parameters if the
//   command byte is set in the range from 0x80 to 0xef.  Any newly 
//   added parameter bytes will be set to 0.
//

int MidiMessage::setSizeToCommand(void) {
   int osize = this->size();
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

   return size();
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

int MidiMessage::getTempoMicro(void) {
   if (!isTempo()) {
      return -1;
   } else {
      return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
   }
}


int MidiMessage::getTempoMicroseconds(void) {
   return getTempoMicro();
}



//////////////////////////////
//
// MidiMessage::getTempoSeconds -- Returns the number of seconds per
//      quarter note.  Returns -1.0 if the MIDI message is not a
//      tempo meta message.
//

double MidiMessage::getTempoSeconds(void) {
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

double MidiMessage::getTempoBPM(void) {
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

double MidiMessage::getTempoTPS(int tpq) {
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

double MidiMessage::getTempoSPT(int tpq) {
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

int MidiMessage::isMeta(void) {
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


int MidiMessage::isMetaMessage(void) {
   return isMeta();
}



//////////////////////////////
//
// MidiMessage::isNoteOff -- Returns true if the command nibble is 0x80
//     or if the command nibble is 0x90 with p2=0 velocity.
//

int MidiMessage::isNoteOff(void) {
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

int MidiMessage::isNoteOn(void) {
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
// MidiMessage::isAftertouch -- Returns true if the command byte is in the 0xA0
//    range.
//

int MidiMessage::isAftertouch(void) {
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

int MidiMessage::isController(void) {
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

int MidiMessage::isTimbre(void) {
   if (((*this)[0] & 0xf0) != 0xc0) {
      return 0;
   } else if (size() != 2) {
      return 0;
   } else {
      return 1;
   }
}


int MidiMessage::isPatchChange(void) {
   return isTimbre();
}



//////////////////////////////
//
// MidiMessage::isPressure -- Returns true of a channel pressure message
//    (command nibble 0xd0).
//

int MidiMessage::isPressure(void) {
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

int MidiMessage::isPitchbend(void) {
   if (((*this)[0] & 0xf0) != 0xe0) {
      return 0;
   } else if (size() != 3) {
      return 0;
   } else {
      return 1;
   }
}



//////////////////////////////
//
// MidiMessage::isTempo -- Returns true if message is a meta message
//      describing tempo (meta message type 0x51).
//

int MidiMessage::isTempo(void) {
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
// MidiMessage::getCommandNibble -- Returns the top 4 bits of the (*this)[0]
//    entry, or -1 if there is not (*this)[0].
//

int MidiMessage::getCommandNibble(void) {
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

int MidiMessage::getCommandByte(void) {
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

int MidiMessage::getChannelNibble(void) {
   if (size() < 1) {
      return -1;
   } else {
      return (*this)[0] & 0x0f;
   }
}


int MidiMessage::getChannel(void) {
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
   int oldsize = size();
   resize(2);
   (*this)[1] = (uchar)p1;
   if (oldsize < 1) {
      (*this)[0] = 0;
   }
}


void MidiMessage::setParameters(int p1, int p2) {
   int oldsize = size();
   resize(3);
   (*this)[1] = (uchar)p1;
   (*this)[2] = (uchar)p2;
   if (oldsize < 1) {
      (*this)[0] = 0;
   }
}


//////////////////////////////
//
// MidiMessage::setMessage --  Set the contents of
//

void MidiMessage::setMessage(vector<uchar>& message) { 
   (*this) = message;
}


void MidiMessage::setMessage(vector<char>& message) {
   resize(message.size());
   for (int i=0; i<size(); i++) {
      (*this)[i] = (uchar)message[i];
   }
}


void MidiMessage::setMessage(vector<int>& message) {
   resize(message.size());
   for (int i=0; i<size(); i++) {
      (*this)[i] = (uchar)message[i];
   }
}



//////////////////////////////
//
// MidiMessage::setMetaTempo -- Input tempo is in quarter notes per minute.
//

void MidiMessage::setMetaTempo(double tempo) {
   int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
   resize(6);
   (*this)[0] = 0xff;
   (*this)[1] = 0x51;
   (*this)[2] = 3;
   (*this)[3] = (microseconds >> 16) & 0xff;
   (*this)[4] = (microseconds >>  8) & 0xff;
   (*this)[5] = (microseconds >>  0) & 0xff;
}


void MidiMessage::setTempo(double tempo) {
   setMetaTempo(tempo);
}



