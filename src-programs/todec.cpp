//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Dec  9 21:10:17 PST 1997
// Last Modified: Tue Jan 27 21:23:19 GMT-0800 1998
// Last Modified: Wed Jun  7 20:55:18 PDT 2000 (added signed/unsigned option)
// Filename:      ...sig/doc/examples/all/todec/todec.cpp
// Syntax:        C++
// $Smake:        g++ -o %b %f && strip %b
//

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <iostream>

using namespace std;

unsigned long atohd      (const char* aNumber);
void          exitUsage  (const char* command);


int main(int argc, char* argv[]) {
   char* numberstring = NULL;
   int sign = 0;

   if (argc < 2) {
      exitUsage(argv[0]);
   } else if (argc == 3) {
      if (strcmp(argv[1], "-s") == 0) {
         sign = 1;
      } else {
         exitUsage(argv[0]);
      }
      numberstring = argv[2];
   } else if (argc == 2) {
      numberstring = argv[1];
   } else {
      exitUsage(argv[0]);
   }

   if (numberstring[0] == 'o' || numberstring[0] == 'O') {
      numberstring[0] = '0';
   }

   unsigned long aValue;
   aValue = atohd(numberstring);
   if (sign) {
      cout << (long)aValue << endl;
   } else {
      cout << aValue << endl;
   }

   return 0;
}


unsigned long atohd(const char* aNumber) {
   return (int)strtoul(aNumber, (char**)NULL, 16);
}



void exitUsage(const char* command) {
   cout << "\n"
   "Converts an integer into decimal form.\n"
   "\n"
   "Usage: " << command << " [-s] integer\n"
   "\n"
   "Options:\n"
   "   -s = interpret hex values as signed values\n"
   "\n"
   "   signed integer =   [-0x80000000..0x7fffffff] \n"
   "                      [-2147483648..2147483647] \n"
   "   unsigned integer = [0x00000000..0xffffffff] \n"
   "                      [0..4294967295] \n"
   << endl;
   exit(1);
}



