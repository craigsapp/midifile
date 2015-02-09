//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Jan 13 08:31:19 PST 1999
// Last Modified: Wed Jan 13 08:31:19 PST 1999
// Filename:      ...improv/doc/examples/improv/vlv/vlv.cpp
// Syntax:        C++; improv
// $Smake:        g++ -O3 -o ../bin/%b -I../include %f -L../lib %
//		     -limprov && strip %b
//
// Description:   Takes Variable Length Values and returns them as
//                the correct value or vice-versa
//

#include "Options.h"
#include <stdlib.h>

#ifndef OLDCPP
   #include <iostream>
   #include <iomanip>
   using namespace std;
#else
   #include <iostream.h>
   #include <iomanip.h>
#endif

#define DECODE 0
#define ENCODE 1

// Global variables for command-line options.
Options  options;            // for command-line processing
int      direction = DECODE; // decoding=0, encoding=1
int      inputStyle = 16;    // number base of the input (2, 10, or 16)
int      outputStyle = 16;   // number base of the output (2, 10, or 16) 
int      inCount = 0;        // number of command line input numbers
int*     input = NULL;       // storage of input numbers;


// function declarations:
void checkOptions(Options& opts);
void displayVLV(int number, ostream& out);
void example(void);
void printbinary(int number, ostream& out);
void usage(const char* command);


///////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv) {
   options.setOptions(argc, argv);
   checkOptions(options);

   if (direction == DECODE) {    // decoding
      unsigned long output = 0;
      for (int i=0; i<inCount; i++) {
         output = output << 7;
         output |= input[i] & 0x7f;
      }
      
      switch (outputStyle) {
         case 2:
            printbinary(output, cout);
            cout << endl;
            break;
         case 16:
            cout << hex << output << endl;
            break;
         default: cout << dec << output << endl;
      }

   } else {                 // encoding
      displayVLV(input[0], cout);
      cout << endl;
   }

   if (input != NULL)   delete [] input;
   return 0;
}

///////////////////////////////////////////////////////////////////////////
   

//////////////////////////////
//
// checkOptions --
//

void checkOptions(Options& opts) {    // options are:
   opts.define("2|bin=b");            //    boolean for binary input
   opts.define("d|dec=b");            //    boolean for decimal input
   opts.define("b=b");                //    boolean for binary output
   opts.define("c=b");                //    boolean for decimal output
   opts.define("x|hex=b");            //    boolean for hex output
   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("h|help=b");
   opts.process();

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
              "craig@ccrma.stanford.edu, January 1999" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << "vlv, version 1.0 (14 Jan 1999)\n"
              "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   // determine input style (the default is 16)
   if (opts.getBoolean("2")) {
      inputStyle = 2;
   } else if (opts.getBoolean("d")) {
      inputStyle = 10;
   } else {
      inputStyle = 16;
   }
 
   // determine output style (the default is the input style)
   if (opts.getBoolean("b")) {
      outputStyle = 2;
   } else if (opts.getBoolean("x")) {
      outputStyle = 16;
   } else if (opts.getBoolean("c")) {
      outputStyle = 10;
   } else {
      outputStyle = inputStyle;
   }

   if (opts.getArgCount() == 0) {
      usage(opts.getCommand());
      exit(1);
   } else if (opts.getArgCount() == 1) {     // encoding into VLV values
      direction = ENCODE; 
   } else {                                  // decoding from VLV values
      direction = DECODE;
   }

   // process the input numbers:

   inCount = opts.getArgCount();

   if (inCount > 4) {
      cout << "Error: VLV values cannot have more than four bytes" << endl;
      exit(1);
   }

   if (input != NULL)   delete [] input;
   input = new int[inCount];

   for (int i=1; i<=opts.getArgCount(); i++) {
      input[i-1] = strtol(opts.getArg(i), NULL, inputStyle);
 
      if (direction == DECODE && i != inCount) {
         if (input[i-1] < 0x80 || input[i-1] > 255) {
            cout << "Invalid VLV byte: " << opts.getArg(i) << endl;
            exit(1);
         }
      } else if (direction == DECODE && i == inCount) {
         if (input[i-1] < 0 || input[i-1] >0x7f) {
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
         switch (outputStyle) {
            case 2:   printbinary(byte[i], out);   break;
            case 16:  out << hex << byte[i];       break;
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
   "vlv -c 3fff\n"
   "\n"
   "# convert a decimal number to VLV bytes in decimal notation:\n" 
   "vlv -d -c 54632\n"
   "\n"
   "# convert hex VLV bytes into hex number:\n" 
   "vlv 81 80 F4 00\n"
   "\n"
   "# convert hex VLV bytes into dec number:\n" 
   "vlv -c 81 80 F4 00\n"
   "\n"
   << endl;
}



//////////////////////////////
//
// printbinary -- prints the number in binary form with no
//    leading zeros.
//

void printbinary(int number, ostream& out) {
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

void usage(const char* command) {
   cout << 
   "\n"
   "Convert/Create Variable Length Values.\n"
   "\n"
   "Usage: " << command << " [-d|-2] [-x|-c|-b] number(s)\n"
   "\n"
   "Options:\n"
   "   -d = input numbers are given in decimal notation (hex is default)\n"
   "   -2 = input numbers are given in binary notation (hex is default)\n"
   "   -b = output numbers are given in binary notation\n" 
   "   -c = output numbers are given in decimal notation\n" 
   "   -x = output numbers are given in hexadecimal notation\n" 
   "   --options = list all options, default values, and aliases\n"
   "\n"
   << endl;
}



// md5sum: 9ab1bf95badc70d153294b2125c4ee7f vlv.cpp [20050403]
