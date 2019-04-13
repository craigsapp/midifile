//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr 15 12:11:36 PDT 2018
// Last Modified: Sun Apr 15 12:11:40 PDT 2018
// Filename:      midifile/tools/readstatus.cpp
// Syntax:        C++11
//
// Description:   Demonstration of checking the read status.
//

#include "MidiFile.h"
#include "Options.h"
#include <iostream>

using namespace std;
using namespace smf;

void printResult(const string& filename, int status, int status2);

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   Options options;
   options.define("p|print=b", "Display parsed MIDI data in ASCII format");
   options.process(argc, argv);
   MidiFile midifile;
   int status;
   if (options.getArgCount() == 0) {
      status = midifile.read(cin);
      printResult("[standard input]", status, midifile.status());
      if (options.getBoolean("print")) {
         cout << midifile;
      }
   } else {
      for (int i=0; i<options.getArgCount(); i++) {
         string filename = options.getArg(i+1);
         status = midifile.read(filename);
         printResult(filename, status, midifile.status());
         if (options.getBoolean("print")) {
            cout << midifile;
         }
      }
   }
   
   return 0;
}


//////////////////////////////
//
// printResult --
//

void printResult(const string& filename, int status, int status2) {
      cout << "Reading from " << filename << ":\t";
      if (status) {
          cout << "SUCCESS";
      } else {
          cout << "FAILURE";
      }
      if (status2) {
          cout << "\tSUCCESS";
      } else {
          cout << "\tFAILURE";
      }
     
      cout << endl;
}



