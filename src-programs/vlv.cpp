//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jan 13 08:31:19 PST 1999
// Last Modified: Mon Feb  9 21:26:32 PST 2015 Updated for C++11.
// Filename:      midifile/src-programs/vlv.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Converts Variable Length Values into integers as
//                well as converts integers into VLVs.
//

#include "Options.h"
#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

#define DECODE 0
#define ENCODE 1

// Global variables for command-line options.
Options  options;              // for command-line processing
int      Direction   = DECODE; // decoding=0, encoding=1
int      InputStyle  = 16;     // number base of the input (2, 10, or 16)
int      OutputStyle = 16;     // number base of the output (2, 10, or 16)
vector<int> Input;             // storage of input numbers


// function declarations:
void     checkOptions        (Options& opts);
void     displayVLV          (int number, ostream& out);
void     example             (void);
void     printBinary         (int number, ostream& out);
void     usage               (const string& command);


///////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
   options.setOptions(argc, argv);
   checkOptions(options);

   if (Direction == DECODE) {    // decoding
      unsigned long output = 0;
      for (int i=0; i<(int)Input.size(); i++) {
         output = output << 7;
         output |= Input[i] & 0x7f;
      }

      switch (OutputStyle) {
         case 2:
            printBinary(output, cout);
            cout << endl;
            break;
         case 16:
            if (output < 16) {
               cout << '0';
            }
            cout << hex << output << endl;
            break;
         default:
            cout << dec << output << endl;
      }

   } else {                 // encoding
      displayVLV(Input[0], cout);
      cout << endl;
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts) {
   opts.define("b|2|bin|binary=b",   "input is binary");
   opts.define("d|10|dec|decimal=b", "input is decimal");
   opts.define("B=b",                "print binary output");
   opts.define("D=b",                "print decimal output");
   opts.define("author=b",           "author of the program");
   opts.define("version=b",          "version of the program");
   opts.define("example=b",          "example usage of the program");
   opts.define("h|help=b",           "list of options for the program");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
              "craig@ccrma.stanford.edu, January 1999" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << "vlv, version 2.0 (9 Feb 2015)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   // determine input style (the default is hexadecimal)
   if (opts.getBoolean("binary")) {
      InputStyle = 2;
   } else if (opts.getBoolean("decimal")) {
      InputStyle = 10;
   } else {
      InputStyle = 16;
   }

   // determine output style (the default is hexadecimal)
   if (opts.getBoolean("B")) {
      OutputStyle = 2;
   } else if (opts.getBoolean("D")) {
      OutputStyle = 10;
   } else {
      OutputStyle = 16;
   }

   if (opts.getArgCount() == 0) {
      usage(opts.getCommand());
      exit(1);
   } else if (opts.getArgCount() == 1) {     // encoding into VLV values
      Direction = ENCODE;
   } else {                                  // decoding from VLV values
      Direction = DECODE;
   }

   // process the input numbers:
   int inCount = opts.getArgCount();
   if (inCount > 4) {
      // Technically 5 as long as they unpack into a four-byte value:
      cout << "Error: VLV values cannot have more than four bytes" << endl;
      exit(1);
   }

   Input.reserve(opts.getArgCount());
   Input.resize(0);

   for (int i=1; i<=opts.getArgCount(); i++) {
      Input.push_back(strtol(opts.getArg(i).c_str(), NULL, InputStyle));

      if (Direction == DECODE && i != inCount) {
         if (Input.back() < 0x80 || Input.back() > 255) {
            cout << "Invalid VLV byte: " << opts.getArg(i) << endl;
            exit(1);
         }
      } else if (Direction == DECODE && i == inCount) {
         if (Input.back() < 0 || Input.back() >0x7f) {
            cout << "Invalid VLV byte: " << opts.getArg(i) << endl;
            exit(1);
         }
      }

   }
}


//////////////////////////////
//
// displayVLV -- prints the number in VLV form
//

void displayVLV(int number, ostream& out) {
   unsigned long value = (unsigned long)number;

   if (value >= (1 << 28)) {
      cout << "Error: number too large to handle" << endl;
      exit(1);
   }

   unsigned long byte[4];
   byte[0] = (value >> 21) & 0x7f;
   byte[1] = (value >> 14) & 0x7f;
   byte[2] = (value >>  7) & 0x7f;
   byte[3] = (value >>  0) & 0x7f;

   int i;
   int flag = 0;
   for (i=0; i<3; i++) {
      if (byte[i] != 0) {
         flag = 1;
      }
      if (flag) {
         byte[i] |= 0x80;
      }
   }

   for (i=0; i<4; i++) {
      if (byte[i] >= 0x80 || i == 3) {
         switch (OutputStyle) {
            case 2:   printBinary(byte[i], out);   break;
            case 16:
               if (byte[i] < 0x0f) {
                  out << '0';
               }
               out << hex << byte[i];
               break;
            default:  out << dec << byte[i];
         }
         if (i != 3) {
            cout << ' ';
         }
      }
   }
}



//////////////////////////////
//
// example -- shows various command-line option calls to program.
//

void example(void) {
   cout <<
   "\n"
   "# convert a hex number to VLV bytes:\n"
   "vlv 3fff\n"
   "\n"
   "# convert a hex number to VLV bytes in decimal notation:\n"
   "vlv -D 3fff\n"
   "\n"
   "# convert a decimal number to VLV bytes in decimal notation:\n"
   "vlv -d -D 54632\n"
   "\n"
   "# convert hex VLV bytes into hex number:\n"
   "vlv 81 80 F4 00\n"
   "\n"
   "# convert hex VLV bytes into dec number:\n"
   "vlv -D 81 80 F4 00\n"
   "\n"
   << endl;
}



//////////////////////////////
//
// printBinary -- prints the number in binary form with no
//    leading zeros.
//

void printBinary(int number, ostream& out) {
   unsigned long value = (unsigned long) number;
   int flag = 0;

   for (int i=0; i<32; i++) {
      if (value & (1 << (31-i))) {
         flag = 1;
      }

      if (flag) {
         if (value & (1 << (31-i))) {
            out << '1';
         } else {
            out << '0';
         }
      }
   }

   if (flag == 0) {
      out << '0';
   }
}



//////////////////////////////
//
// usage -- how to run this program from the command-line.
//

void usage(const string& command) {
   cout <<
   "\n"
   "Convert/Create Variable Length Values.\n"
   "\n"
   "Usage: " << command << " [-d|-b] [-D|-B] number(s)\n"
   "\n"
   "Options:\n"
   "   -d = input numbers are given in decimal notation (hex is default)\n"
   "   -b = input numbers are given in binary notation (hex is default)\n"
   "   -D = output numbers are given in decimal notation (hex is default)\n"
   "   -B = output numbers are given in binary notation (hex is default)\n"
   "   --options = list all options, default values, and aliases\n"
   "\n"
   << endl;
}



