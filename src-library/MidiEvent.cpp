//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Feb 14 21:40:14 PST 2015
// Last Modified: Sat Feb 14 23:33:51 PST 2015
// Filename:      midifile/src/MidiEvent.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class which stores a MidiMessage and a timestamp
//                for the MidiFile class.
//

#include "MidiEvent.h"

using namespace std;


//////////////////////////////
//
// MidiEvent::MidiEvent -- Constructor classes
//

MidiEvent::MidiEvent(void) : MidiMessage() {
   tick  = 0;
   track = 0;
}


MidiEvent::MidiEvent(int command) : MidiMessage(command)  {
   tick  = 0;
   track = 0;
}


MidiEvent::MidiEvent(int command, int p1) : MidiMessage(command, p1) {
   tick  = 0;
   track = 0;
}


MidiEvent::MidiEvent(int command, int p1, int p2) 
      : MidiMessage(command, p1, p2) {
   tick  = 0;
   track = 0;
}


MidiEvent::MidiEvent(int aTime, int aTrack, vector<uchar>& message)
      : MidiMessage(message) {
   tick  = aTime;
   track = aTrack;
}


MidiEvent::MidiEvent(const MidiEvent& mfevent) {
   tick  = mfevent.tick;
   track = mfevent.track;
   this->resize(mfevent.size());
   for (int i=0; i<this->size(); i++) {
      (*this)[i] = mfevent[i];
   }
}



//////////////////////////////
//
// MidiEvent::~MidiEvent -- MidiFile Event destructor
//

MidiEvent::~MidiEvent() {
   tick  = -1;
   track = -1;
   this->resize(0);
}



//////////////////////////////
//
// MidiEvent::operator= -- Copy the contents of another MidiEvent.
//

MidiEvent& MidiEvent::operator=(MidiEvent& mfevent) {
   if (this == &mfevent) {
      return *this;
   }
   tick  = mfevent.tick;
   track = mfevent.track;
   this->resize(mfevent.size());
   for (int i=0; i<this->size(); i++) {
      (*this)[i] = mfevent[i];
   }
   return *this;
}


MidiEvent& MidiEvent::operator=(MidiMessage& message) {
   if (this == &message) {
      return *this;
   }
   tick  = 0;
   track = 0;
   this->resize(message.size());
   for (int i=0; i<this->size(); i++) {
      (*this)[i] = message[i];
   }
   return *this;
}


MidiEvent& MidiEvent::operator=(vector<uchar>& bytes) {
   tick  = 0;
   track = 0;
   this->resize(bytes.size());
   for (int i=0; i<this->size(); i++) {
      (*this)[i] = bytes[i];
   }
   return *this;
}


MidiEvent& MidiEvent::operator=(vector<char>& bytes) {
   tick  = 0;
   track = 0;
   setMessage(bytes);
   return *this;
}


MidiEvent& MidiEvent::operator=(vector<int>& bytes) {
   tick  = 0;
   track = 0;
   setMessage(bytes);
   return *this;
}




