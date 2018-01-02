//
// Programmer:    Tom M. <tom.mbrt@gmail.com>
// Creation Date: Sat Nov 05 14:51:00 PST 2016
// Last Modified: Sat Nov 05 14:51:00 PST 2016
// Filename:      midifile/src-programs/deleteOverlapps.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/src-programs/deleteOverlapps.cpp
// Syntax:        C++
//
// Description:   gets rid of overlapping note in a midi file, i.e. sets the note off event of an overlapping note right before
//                note on event of a second note
//
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char** argv) {
    Options options;
    options.process(argc, argv);
    MidiFile midifile;
    if (options.getArgCount() > 1) {
        midifile.read(options.getArg(1));
    }
    else
    {
        cout << "usage: infile.mid outfile.mid" << endl;
        return -1;
    }


    cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
    cout << "TRACKS: " << midifile.getTrackCount() << endl;


    cout << "notepairs: " << midifile.linkNotePairs() << endl;

    midifile.absoluteTicks();

    for (int track=0; track < midifile.getTrackCount(); track++)
    {
        cout << "\nTrack " << track << endl;
        for (int event=0; event < midifile[track].size(); event++)

        {
            MidiEvent* on1 = &midifile[track][event];
            if (on1->isNoteOn())
            {
                MidiEvent* off1 = on1->getLinkedEvent();
                if(off1 != nullptr)
                {
                    on1->unlinkEvent();
                    off1->unlinkEvent();

                    for(int e=event; e<midifile[track].size(); e++)
                    {
                        MidiEvent* on2 = &midifile[track][e];

                        if(on2 != NULL &&
                                on2->isNoteOn() &&
                                on1->getKeyNumber() == on2->getKeyNumber() &&
                                on1->getChannel()   == on2->getChannel() &&

                                on1->tick < on2->tick &&
                                off1->tick >= on2->tick // they overlapp
                          )
                        {
                            off1->tick = on2->tick-1;
                        }

                    }
                }
            }
        }
    }

    midifile.sortTracks();
    midifile.write(options.getArg(2));


    return 0;
}
