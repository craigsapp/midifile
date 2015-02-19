//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Fri Nov 26 14:12:01 PST 1999
// Last Modified: Fri Dec  2 13:26:44 PST 1999
// Last Modified: Fri Nov 10 12:13:15 PST 2000 Added some more editing cap.
// Last Modified: Thu Jan 10 10:03:39 PST 2002 Added allocateEvents()
// Last Modified: Mon Jun 10 22:43:10 PDT 2002 Added clear()
// Last Modified: Sat Dec 17 23:11:57 PST 2005 Added millisecond ticks
// Last Modified: Tue Feb  5 11:51:43 PST 2008 Read() set to const char*
// Last Modified: Tue Apr  7 09:23:48 PDT 2009 Added addMetaEvent
// Last Modified: Fri Jun 12 22:58:34 PDT 2009 Renamed SigCollection class
// Last Modified: Thu Jul 22 23:28:54 PDT 2010 Added tick to time mapping
// Last Modified: Thu Jul 22 23:28:54 PDT 2010 Changed _MidiEvent to MidiEvent
// Last Modified: Tue Feb 22 13:26:40 PST 2011 Added write(ostream)
// Last Modified: Mon Nov 18 13:10:37 PST 2013 Added .printHex function.
// Last Modified: Mon Feb  9 14:01:31 PST 2015 Removed FileIO dependency.
// Last Modified: Sat Feb 14 22:35:25 PST 2015 Split out subclasses.
// Filename:      midifile/include/MidiFile.h
// Website:       http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class which can read/write Standard MIDI files.
//                MIDI data is stored by track in an array.
//

#ifndef _MIDIFILE_H_INCLUDED
#define _MIDIFILE_H_INCLUDED

#include "MidiEventList.h"

#include <vector>
#include <istream>
#include <fstream>

using namespace std;

#define TIME_STATE_DELTA       0
#define TIME_STATE_ABSOLUTE    1

#define TRACK_STATE_SPLIT      0
#define TRACK_STATE_JOINED     1

class _TickTime {
   public:
      int    tick;
      double seconds;
};


class MidiFile {
   public:
                MidiFile                  (void);
                MidiFile                  (const char* aFile);
                MidiFile                  (const string& aFile);
               ~MidiFile                  ();

      void      absoluteTime              (void);
      int       addEvent                  (int aTrack, int aTime, 
                                             vector<uchar>& midiData);
      int       addEvent                  (MidiEvent& mfevent);
      int       addMetaEvent              (int aTrack, int aTime, int aType,
                                             vector<uchar>& metaData);
      int       addMetaEvent              (int aTrack, int aTime, int aType,
                                             const char* metaData);
      int       addPitchBend              (int aTrack, int aTime,
                                           int aChannel, double amount);
      int       addTrack                  (void);
      int       addTrack                  (int count);
      void      allocateEvents            (int track, int aSize);
      void      deltaTime                 (void);
      void      deleteTrack               (int aTrack);
      void      erase                     (void);
      void      clear                     (void);
      MidiEvent&  getEvent                (int aTrack, int anIndex);
      MidiEventList& operator[]           (int aTrack);
      int       getTimeState              (void);
      int       getTrackState             (void);
      int       getTicksPerQuarterNote    (void);
      int       getTrackCountAsType1      (void);
      int       getTrackCount             (void);
      int       getTrack                  (int track, int index);
      int       getNumTracks              (void);
      int       getEventCount             (int aTrack);
      int       getNumEvents              (int aTrack);
      void      joinTracks                (void);
      void      mergeTracks               (int aTrack1, int aTrack2);
      int       read                      (const char* aFile);
      int       read                      (const string& aFile);
      int       read                      (istream& istream);

      void      setTicksPerQuarterNote    (int ticks);
      void      setMillisecondDelta       (void);

      void      doTimeInSecondsAnalysis   (void);
      double    getTimeInSeconds          (int aTrack, int anIndex);
      double    getTimeInSeconds          (int tickvalue);
      int       getAbsoluteTickTime       (double starttime);

      void      sortTrack                 (MidiEventList& trackData);
      void      sortTracks                (void);
      void      splitTracks               (void);
      int       write                     (const char* aFile);
      int       write                     (const string& aFile);
      int       write                     (ostream& out);
      int       writeBinasc               (const char* aFile);
      int       writeBinasc               (const string& aFile);
      int       writeBinasc               (ostream& out);
      ostream&  printHex                  (ostream& out);
      void      setFilename               (const char* aname);
      void      setFilename               (const string& aname);
      const char* getFilename             (void);

      // static functions:
      static uchar    readByte                (istream& input);
      static ushort   readLittleEndian2Bytes  (istream& input);
      static ulong    readLittleEndian4Bytes  (istream& input);
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
      vector<MidiEventList*> events;             // MIDI file events
      int              ticksPerQuarterNote;      // time base of file
      int              trackCount;               // # of tracks in file
      int              theTrackState;            // joined or split
      int              theTimeState;             // absolute or delta
      vector<char>     readFileName;             // read file name

      int               timemapvalid;    
      vector<_TickTime> timemap;

   private:
      int        extractMidiData  (istream& inputfile, vector<uchar>& array, 
                                       uchar& runningCommand);
      ulong      extractVlvTime   (istream& inputfile);
      ulong      unpackVLV        (uchar a, uchar b, uchar c, uchar d, uchar e);
      void       writeVLValue     (long aValue, vector<uchar>& data);
      int        makeVLV          (uchar *buffer, int number);
      static int ticksearch       (const void* A, const void* B);
      static int secondsearch     (const void* A, const void* B);
      void       buildTimeMap     (void);
      int        linearTickInterpolationAtSecond  (double seconds);
};


int eventcompare(const void* a, const void* b);
ostream& operator<<(ostream& out, MidiFile& aMidiFile);

#endif /* _MIDIFILE_H_INCLUDED */



