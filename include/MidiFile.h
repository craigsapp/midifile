//
// Copyright 1999-2000 by Craig Stuart Sapp, All Rights Reserved.
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
// Last Modified: Thu Jul 22 23:28:54 PDT 2010 Changed _MFEvent to MFEvent
// Last Modified: Tue Feb 22 13:26:40 PST 2011 Added write(ostream)
// Last Modified: Mon Nov 18 13:10:37 PST 2013 Added .printHex function.
// Filename:      ...sig/include/sigInfo/MidiFile.h
// Web Address:   http://sig.sapp.org/include/sigInfo/MidiFile.h
// Syntax:        C++ 
//
// Description:   A class which can read/write Standard MIDI files.
//                MIDI data is stored by track in an array.  This
//                class is used for example in the MidiPerform class.
//

#ifndef _MIDIfILE_H_INCLUDED
#define _MIDIfILE_H_INCLUDED

#include "FileIO.h"
#include "Array.h"
#include "SigCollection.h"

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;

#define TIME_STATE_DELTA       0
#define TIME_STATE_ABSOLUTE    1

#define TRACK_STATE_SPLIT      0
#define TRACK_STATE_JOINED     1

class _TickTime {
   public:
      int    tick;
      double seconds;
};


class MFEvent {
   public:
                MFEvent    (void);

                // constructors which create store values in .data
                // with .time field left uninitialized:
                MFEvent    (int command);
                MFEvent    (int command, int param1);
                MFEvent    (int command, int param1, int param2);
                MFEvent    (int track, int command, int param1, int param2);

                MFEvent    (int aTime, int aTrack, int command, int param1, 
                            int param2);
                MFEvent    (int aTime, int aTrack, Array<uchar>& someData);
                MFEvent    (MFEvent& mfevent);
     MFEvent&   operator=  (MFEvent& mfevent);

               ~MFEvent    ();

      int       time;
      int       track;
      Array<uchar> data;

      // Convenience functions for parsing MFEvent contents.

      int       isMeta                    (void);
      int       isTempo                   (void);
      int       getTempoMicro             (void);
      int       getTempoMicroseconds      (void);
      double    getTempoSeconds           (void);
      double    getTempoBPM               (void);
      double    getTempoTPS               (int tpq);
      double    getTempoSPT               (int tpq);
      int       isNoteOff                 (void);
      int       isNoteOn                  (void);
      int       isTimbre                  (void);
      int       getCommandNibble          (void);
      int       getChannelNibble          (void);
};



class MidiFile {
   public:
                MidiFile                  (void);
                MidiFile                  (const char* aFile);
               ~MidiFile                  ();

      void      absoluteTime              (void);
      int       addEvent                  (int aTrack, int aTime, 
                                             Array<uchar>& midiData);
      int       addEvent                  (MFEvent& mfevent);
      int       addMetaEvent              (int aTrack, int aTime, int aType,
                                             Array<uchar>& metaData);
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
      MFEvent&  getEvent                  (int aTrack, int anIndex);
      int       getTimeState              (void);
      int       getTrackState             (void);
      int       getTicksPerQuarterNote    (void);
      int       getTrackCount             (void);
      int       getTrackCountAsType1      (void);
      int       getNumTracks              (void);
      int       getNumEvents              (int aTrack);
      int       getEventCount             (int aTrack) 
                                             { return getNumEvents(aTrack); }
      void      joinTracks                (void);
      void      mergeTracks               (int aTrack1, int aTrack2);
      int       read                      (const char* aFile);

      void      setTicksPerQuarterNote    (int ticks);
      void      setMillisecondDelta       (void);

      void      doTimeInSecondsAnalysis   (void);
      double    getTimeInSeconds          (int aTrack, int anIndex);
      double    getTimeInSeconds          (int tickvalue);
      int       getAbsoluteTickTime       (double starttime);

      void      sortTrack                 (SigCollection<MFEvent>& trackData);
      void      sortTracks                (void);
      void      splitTracks               (void);
      int       write                     (const char* aFile);
      int       write                     (ostream& out);
      ostream&  printHex                  (ostream& out);
      void      setFilename               (const char* aname);
      const char* getFilename             (void);

      // access to convenience functions in MFEvent:
      int       isMeta                    (int track, int index);
      int       isTempo                   (int track, int index);
      int       getTempoMicro             (int track, int index);
      int       getTempoMicroseconds      (int track, int index);
      double    getTempoSeconds           (int track, int index);
      double    getTempoBPM               (int track, int index);
      double    getTempoTPS               (int track, int index);
      double    getTempoSPT               (int track, int index);
      int       isNoteOff                 (int track, int index);
      int       isNoteOn                  (int track, int index);
      int       isTimbre                  (int track, int index);
      int       getCommandNibble          (int track, int index);
      int       getChannelNibble          (int track, int index);
      int       getTrack                  (int track, int index);


   protected:
      SigCollection<SigCollection<MFEvent>*> events;  // midi file events
      int              ticksPerQuarterNote;           // time base of file
      int              trackCount;                    // # of tracks in file
      int              theTrackState;                 // joined or split
      int              theTimeState;                  // absolute or delta
      Array<char>      readFileName;                  // read file name

      int              timemapvalid;    
      Array<_TickTime> timemap;

   private:
      void       extractMidiData  (FileIO& inputfile, Array<uchar>& array, 
                                       uchar& runningCommand);
      ulong      extractVlvTime   (FileIO& inputfile);
      ulong      unpackVLV        (uchar a, uchar b, uchar c, uchar d, uchar e);
      void       writeVLValue     (long aValue, Array<uchar>& data);
      int        makeVLV          (uchar *buffer, int number);
      static int ticksearch       (const void* A, const void* B);
      static int secondsearch     (const void* A, const void* B);
      void       buildTimeMap     (void);
      int        linearTickInterpolationAtSecond  (double seconds);
};


int eventcompare(const void* a, const void* b);
ostream& operator<<(ostream& out, MidiFile& aMidiFile);

#endif /* _MIDIfILE_H_INCLUDED */



// md5sum: ff46e64698e2d9e88ebeef3efa9927d0 MidiFile.h [20030102]
