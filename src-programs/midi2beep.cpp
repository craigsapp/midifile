//
// Programmer:    Tom M. <tom.mbrt@gmail.com>
// Creation Date: Sat Nov 05 14:51:00 PST 2016
// Last Modified: Sat Nov 05 14:51:00 PST 2016
// Filename:      midifile/src-programs/mid2beep.cpp
// Web Address:   https://github.com/craigsapp/midifile/blob/master/src-programs/mid2beep.cpp
// Syntax:        C++
//
// Description:   Linux-only: Play a monophonic midi file on the PC Speaker (i.e. the midi file contains only a single track
//                on a single channel playing a single note at a time)
//


#include <iostream>
#include <cmath>
#include <unistd.h>     // usleep()
#include <sys/io.h>

#include "MidiFile.h"
#include "Options.h"

#include <signal.h>

using namespace std;

void beepOff(int)
{

    int r = inb(0x61);
    outb(r|3, 0x61);
    outb(r & 0xFC, 0x61);

    exit(0);
}

void beep(int fre, int usDuration)
{
    int r, ff;

    if(fre > 0) {
        ff = 1193180/fre;
        outb( 0xB6,            0x43);
        outb( ff & 0xff,       0x42);
        outb((ff >> 8) & 0xff, 0x42);
    }

    r = inb(0x61);
    if(fre > 0) outb(r|3, 0x61);
    usleep(usDuration);
    outb(r & 0xFC, 0x61);
}

int main(int argc, char** argv)
{
    signal(SIGINT, beepOff);

    Options options;
    options.process(argc, argv);
    MidiFile midifile;
    if (options.getArgCount() == 1) {
        midifile.read(options.getArg(1));
    }
    else
    {
        cout << "usage: " << argv[0] << " file.mid" << endl;
        return -1;
    }

    int k= iopl(3);

    printf("\n Result iopl: %d \n", k);
    if (k<0)
    {
        cerr << " iopl() " << endl;
        return k;
    }

    midifile.linkNotePairs();
    midifile.joinTracks();
    midifile.doTimeAnalysis();

    midifile.absoluteTicks();

    double lastNoteFinished = 0.0;
    for (int track=0; track < midifile.getTrackCount(); track++) {
        for (int i=0; i<midifile[track].size(); i++) {
            MidiEvent* mev = &midifile[track][i];
            if (!mev->isNoteOn() || mev->getLinkedEvent() == NULL) {
                continue;
            }

            // pause, silence
            int silence = static_cast<int>((midifile.getTimeInSeconds(mev->tick) - lastNoteFinished) * 1000 * 1000);
            if(silence >0)
            {
                usleep(silence);
            }

            double duration = mev->getDurationInSeconds();

            int halfTonesFromA4 = mev->getKeyNumber() - 69; // 69 == A4 == 440Hz
            int frq = 440 * pow(2, halfTonesFromA4/12.0);

            // play note
            beep(frq, static_cast<int>(duration*1000*1000));

            MidiEvent* off = mev->getLinkedEvent();
            lastNoteFinished = midifile.getTimeInSeconds(off->tick);

        }
    }


    return 0;
}


