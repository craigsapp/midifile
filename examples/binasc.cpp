//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Apr 28 15:09:31 GMT-0800 1997
// Last Modified: Mon Jan 19 02:41:56 GMT-0800 1998
// Last Modified: Thu Oct 22 16:47:41 PDT 1998
// Last Modified: Fri Sep 15 01:03:43 PDT 2006 (upgraded to ANSI C99 C++)
// Filename:      .../lang/c/binasc/binasc.cpp
// Syntax:        C++
// $Smake:        smake -v linux %f
// $Smake-linux:  g++ -DLINUX -O3 -o %b %f Options.cpp FileIO.cpp && strip %b
//

#include <ctype.h>     
#include <string.h>

#include "Options.h"
#include "FileIO.h"

#ifndef OLDCPP
   #include <iostream>
   #include <fstream>
   #include <iomanip>
   using namespace std;
#else
   #include <iostream.h>
   #include <fstream.h>
   #include <iomanip.h>
#endif
   

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;


// global variables:
Options options;            // command-line options
FileIO  outputCompiled;     // output for compilation


// function declarations:
void checkOptions(Options& opts);
void compileFile(const char* filename);
void example(void);
void manual(void);
void outputStyleAscii(const char* filename);
void outputStyleBinary(const char* filename);
void outputStyleBoth(const char* filename);
void processAsciiWord(const char* word, int lineNumber, FileIO& out);
void processBinaryWord(const char* word, int lineNumber, FileIO& out);
void processDecimalWord(const char* word, int lineNumber, FileIO& out);
void processHexadecimalWord(const char* word, int lineNumber, FileIO& out);
void processLine(char* word, int lineNumber, FileIO& out);
void usage(const char* command);


///////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   options.setOptions(argc, argv);
   checkOptions(options);
   
   for (int i=0; i<options.getArgCount(); i++) {
      if (options.getBoolean("compile")) {
         compileFile(options.getArg(i+1));
      } else if (options.getBoolean("binary")) {
         outputStyleBinary(options.getArg(i+1));
      } else if (options.getBoolean("ascii")) {
         outputStyleAscii(options.getArg(i+1));
      } else {
         outputStyleBoth(options.getArg(i+1));
      }
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////



//////////////////////////////
//
// checkOptions -- check and process the command line options for
//     the program.
//

void checkOptions(Options& opts) {
   opts.define("a|ascii=b");
   opts.define("b|binary=b");
   opts.define("c|compile=s:");
   opts.define("m|man|manual=b");
   opts.define("mod=i:25");
   opts.define("wrap=i:75");              // for -a option
   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.process();

   if (opts.getBoolean("a") + opts.getBoolean("b") +
         opts.getBoolean("c") > 1) {
      cerr << "Error: only one of the opts -a, -b, or -c can be used"
              "at one time." << endl;
      usage(opts.getCommand());
      exit(1);
   }

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, April 1997" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "last edited: Thu Oct 22 14:17:23 PDT 1998 " << endl;
      cout << "compiled:    " << __DATE__ << endl;
      exit(0);
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }
   if (opts.getBoolean("manual")) {
      manual();
      exit(0);
   }

   
   if (opts.getBoolean("compile") && strlen(opts.getString("compile")) == 0) {
      cerr << "Error: you must specify an output file when using the -c option"
           << endl;
      exit(1);
   }

   if (opts.getArgCount() < 1) {
      cerr << "Error: you must specify at least one file on the command-line"
           << endl;
      usage(opts.getCommand());
      exit(1);
   }

   if (strlen(opts.getString("compile")) > 0) {
      outputCompiled.open(opts.getString("compile"), ios::out);
      if (!outputCompiled.is_open()) {
         cerr << "Error opening output file: " << opts.getString("compile") 
              << endl;
         exit(1);
      }
   }

}



//////////////////////////////
//
// compileFile -- convert an ascii file with bytes
//     specified as numbers into output stream.
//

void compileFile(const char* filename) {
   char     inputLine[1024] = {0};    // current line being processed
   int      lineCount = 0;            // count current line being processed
   ifstream infile;                   // file to be read from

   infile.open(filename);
   if (!infile.is_open()) {
      cerr << "Error opening file: " << filename << endl;
      exit(1);
   }

   if (!outputCompiled.is_open()) {
      cerr << "Error: output file was not opened" << endl;
      exit(1);
   }

   infile.getline(inputLine, 1024, '\n');
   lineCount++;
   while (!infile.eof()) {
      processLine(inputLine, lineCount, outputCompiled);
      infile.getline(inputLine, 1024, '\n');
      lineCount++;
   }
   infile.close();   
}



//////////////////////////////
//
// example -- gives example calls to the binasc program.
//

void example(void) {
   cout <<
   "# display bytes a hexadecimal values and any associated ascii characters \n"
   "       binasc filename                                                   \n"
   "# display bytes only as associated ascii characters (suppressing spaces) \n"
   "       binasc -a filename                                                \n"
   "# display bytes only as hexadecimal values                               \n"
   "       binasc -b filename                                                \n"
   "# compile the numeric values of the input into bytes in output           \n"
   "       binasc -c filename                                                \n"
   << endl;
}



//////////////////////////////
//
// outputStyleAscii -- read an input file and output bytes in ascii
//    form, not displaying any blank lines.  Output words are not
//    broken unless they are longer than 75 characters.
//

void outputStyleAscii (const char* filename) {
   uchar outputWord[256] = {0};   // storage for current word
   int index = 0;                 // current length of word
   int lineCount = 0;             // current length of line
   int maxLineLength = options.getInteger("wrap"); // max line length for output
   if (maxLineLength < 1) {
      cerr << "Error invalid colmn wrap specified" << endl;
      exit(1);
   }
   uchar ch;                      // current input byte
   int type = 0;                  // 0=space, 1=printable
   int lastType = 0;              // 0=space, 1=printable
   ifstream infile;               // file to be read from


   infile.open(filename);
   if (!infile.is_open()) {
      cerr << "Error opening file: " << filename << endl;
      exit(1);
   }

   ch = infile.get();

   while (!infile.eof()) {

      lastType = type;
      if (isprint(ch) && !isspace(ch)) {
         type = 1;
      } else {        
         type = 0;
      }

      if (type == 1 && lastType == 0) {
         // start of a new word.  check where to put old word
         if (index + lineCount >= maxLineLength) {  // put on next line
            outputWord[index] = '\0';
            cout << '\n' << outputWord;
            lineCount = index;
            index = 0;
         } else {                                   // put on current line
            outputWord[index] = '\0';
            if (lineCount != 0) {
               cout << ' ';
               lineCount++;
            }
            cout << outputWord;
            lineCount += index;
            index = 0;
         }
      } 
     
      if (type == 1) {
         outputWord[index++] = ch;
      }

      ch = infile.get();
   }

   if (index != 0) {
      cout << endl;
   }

   infile.close();
}



//////////////////////////////
//
// outputStyleBinary -- read an input file and output bytes in ascii form,
//     hexadecimal numbers only.
//

void outputStyleBinary(const char* filename) {
   int maxByteInLine = options.getInt("mod"); // max line length for output
   if (maxByteInLine < 1) {
      cerr << "Error invalid byte count specified" << endl;
      exit(1);
   }
   int currentByte = 0;           // current byte output in line
   uchar ch;                      // current input byte
   ifstream infile;               // file to be read from

   infile.open(filename);
   if (!infile.is_open()) {
      cerr << "Error opening file: " << filename << endl;
      exit(1);
   }

   ch = infile.get();

   if (infile.eof()) {
      cout << "End of the file right away!" << endl; 
   }

   while (!infile.eof()) {
      if (ch < 0x10) {
         cout << '0';
      }
      cout << hex << (int)ch << ' ';
      currentByte++;
      if (currentByte >= maxByteInLine) {
         cout << '\n';
         currentByte = 0;
      }
      ch = infile.get();
   }

   if (currentByte != 0) {
      cout << endl;
   }
   infile.close();
}
   


//////////////////////////////
//
// outputStyleBoth -- read an input file and output bytes in ascii form
//     with both hexadecimal numbers and ascii representation
//

void outputStyleBoth(const char* filename) {
   uchar asciiLine[256] = {0};    // storage for output line
   int maxByteInLine = options.getInt("mod"); // max line length for output
   if (maxByteInLine < 1) {
      cerr << "Error invalid byte count specified" << endl;
      exit(1);
   }
   int currentByte = 0;           // current byte output in line
   int index = 0;                 // current character in asciiLine
   uchar ch;                      // current input byte
   ifstream infile;               // file to be read from
 
   infile.open(filename);
   if (!infile.is_open()) {
      cerr << "Error opening file: " << filename << endl;
      exit(1);
   }

   ch = infile.get();
   while (!infile.eof()) {
      if (index == 0) {
         asciiLine[index++] = ';';
         cout << ' '; 
      }
      if (ch < 0x10) {
         cout << '0';
      }
      cout << hex << (int)ch << ' ';
      currentByte++;
  
      asciiLine[index++] = ' ';
      if (isprint(ch)) {
         asciiLine[index++] = ch;
      } else {
         asciiLine[index++] = ' ';
      }
      asciiLine[index++] = ' ';

      if (currentByte >= maxByteInLine) {
         cout << '\n';
         asciiLine[index] = '\0';
         cout << asciiLine << "\n\n"; 
         currentByte = 0;
         index = 0;
      }
      ch = infile.get();
   }

   if (currentByte != 0) {
      cout << '\n';
      asciiLine[index] = '\0';
      cout << asciiLine << '\n' << endl;
   }

   infile.close();
}
   


///////////////////////////////
//
// processLine -- read a line of input and output any specified bytes
//

#define WORD_SEPARATORS " \n\t"

void processLine(char* inputLine, int lineCount, FileIO& out) {
   char* word = strtok(inputLine, WORD_SEPARATORS);
   while (word != NULL) {
      if (word[0] == ';') {
         return;
      }
      if (word[0] == '+') {
         processAsciiWord(word, lineCount, out);
      } else if (strchr(word, '\'')) {
         processDecimalWord(word, lineCount, out);
      } else if (strchr(word, ',') || strlen(word) > 2) {
         processBinaryWord(word, lineCount, out);
      } else {
         processHexadecimalWord(word, lineCount, out);
      }
      word = strtok(NULL, WORD_SEPARATORS);
   }
}


//////////////////////////////
//
// processDecimalWord -- interprets a decimal word into
//     constituent bytes
//

void processDecimalWord(const char* word, int lineNumber, FileIO& out) {
   int length = strlen(word);       // length of ascii binary number
   int byteCount = -1;              // number of bytes to output
   int quoteIndex = -1;             // index of decimal specifier
   int signIndex = -1;              // index of any sign for number
   int periodIndex = -1;            // index of period for floating point
   int endianIndex = -1;            // index of little endian specifier
   int i = 0;

   // make sure that all characters are valid
   for (i=0; i<length; i++) {
      switch (word[i]) {
         case '\'':
            if (quoteIndex != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "extra quote in decimal number" << endl;
               exit(1);
            } else {
               quoteIndex = i;
            }
            break;
         case '-':
            if (signIndex != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "cannot have more than two minus signs in number" 
                    << endl;
               exit(1);
            } else {
               signIndex = i;
            }
            if (i == 0 || word[i-1] != '\'') {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "minus sign must immediately follow quote mark" << endl;
               exit(1);
            }
            break;
         case '.':
            if (quoteIndex == -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "cannot have decimal marker before quote" << endl;
               exit(1);
            }
            if (periodIndex != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "extra period in decimal number" << endl;
               exit(1);
            } else {
               periodIndex = i;
            }
            break;
         case 'u':
         case 'U':
            if (quoteIndex != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "cannot have endian specified after quote" << endl;
               exit(1);
            }
            if (endianIndex != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "extra \"u\" in decimal number" << endl;
               exit(1);
            } else {
               endianIndex = i;
            }
            break;
         case '8': 
         case '1': case '2': case '3': case '4':
            if (quoteIndex == -1 && byteCount != -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "invalid byte specificaton before quote in "
                    << "decimal number" << endl;
               exit(1);
            } else if (quoteIndex == -1) {
               byteCount = word[i] - '0';
            }
            break;
         case '0': case '5': case '6': case '7': case '9':
            if (quoteIndex == -1) {
               cerr << "Error on line " << lineNumber << " at token: " << word 
                    << endl;
               cerr << "cannot have numbers before quote in decimal number" 
                    << endl;
               exit(1);
            }
            break;
         default:
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "Invalid character in decimal number"
                    " (character number " << i <<")" << endl;
            exit(1);
      }
   }

   // there must be a quote character to indicate a decimal number
   // and there must be a decimal number after the quote
   if (quoteIndex == -1) {
      cerr << "Error on line " << lineNumber << " at token: " << word 
           << endl;
      cerr << "there must be a quote to signify a decimal number" << endl;
      exit(1);
   } else if (quoteIndex == length - 1) {
      cerr << "Error on line " << lineNumber << " at token: " << word 
           << endl;
      cerr << "there must be a decimal number after the quote" << endl;
      exit(1);
   }

   // 8 byte decimal output can only occur if reading a double number
   if (periodIndex == -1 && byteCount == 8) {
      cerr << "Error on line " << lineNumber << " at token: " << word 
           << endl;
      cerr << "only floating-point numbers can use 8 bytes" << endl;
      exit(1);
   }

   // default size for floating point numbers is 4 bytes
   if (periodIndex != -1) {
      if (byteCount == -1) {
         byteCount = 4;
      }
   } 

   // process any floating point numbers possibilities
   if (periodIndex != -1) {
      double doubleOutput = atof(&word[quoteIndex+1]);
      float  floatOutput  = (float)doubleOutput;
      switch (byteCount) {
         case 4:
           if (endianIndex == -1) {
              out.writeBigEndian(floatOutput);
           } else {
              out.writeLittleEndian(floatOutput);
           }
           return;
           break;
         case 8:
           if (endianIndex == -1) {
              out.writeBigEndian(doubleOutput);
           } else {
              out.writeLittleEndian(doubleOutput); 
           }
           return;
           break;
         default:
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "floating-point numbers can be only 4 or 8 bytes" << endl;
            exit(1);
      }
   }
   
   // process any integer decimal number possibilities

   // default integer size is one byte, if size is not specified, then
   // the number must be in the one byte range and cannot overflow
   // the byte if the size of the decimal number is not specified
   if (byteCount == -1) {
      if (signIndex != -1) {
         long tempLong = atoi(&word[quoteIndex + 1]);
         if (tempLong > 127 || tempLong < -128) {
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "Decimal number out of range from -128 to 127" << endl;
            exit(1);
         }
         char charOutput = (char)tempLong;
         out << charOutput;
         return;
      } else {
         ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
         uchar ucharOutput = (uchar)tempLong;
         if (tempLong > 255 || tempLong < 0) {
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "Decimal number out of range from 0 to 255" << endl;
            exit(1);
         }
         out << ucharOutput;
         return;
      }
   }

   // left with an integer number with a specified number of bytes
   switch (byteCount) {
      case 1:
         if (signIndex != -1) {
            long tempLong = atoi(&word[quoteIndex + 1]);
            char charOutput = (char)tempLong;
            out << charOutput;
            return;
         } else {
            ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
            uchar ucharOutput = (uchar)tempLong;
            out << ucharOutput;
            return;
         }
         break;
      case 2:
         if (signIndex != -1) {
            long tempLong = atoi(&word[quoteIndex + 1]);
            short shortOutput = (short)tempLong;
            if (endianIndex == -1) {
               out.writeBigEndian(shortOutput);
            } else {
               out.writeLittleEndian(shortOutput);
            }
            return;
         } else {
            ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
            ushort ushortOutput = (ushort)tempLong;
            if (endianIndex == -1) {
               out.writeBigEndian(ushortOutput);
            } else {
               out.writeLittleEndian(ushortOutput);
            }
            return;
         }
         break;
      case 3:
         {
         if (signIndex != -1) {
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "negative decimal numbers cannot be stored in 3 bytes"
                 << endl;
            exit(1);
         }
         ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
         uchar byte1 = (tempLong & 0x00ff0000) >> 16;
         uchar byte2 = (tempLong & 0x0000ff00) >>  8;
         uchar byte3 = (tempLong & 0x000000ff);
         if (endianIndex == -1) {
            out << byte1;
            out << byte2;
            out << byte3;
         } else {
            out << byte3;
            out << byte2;
            out << byte1;
         }
         return;
         }
         break;
      case 4:
         if (signIndex != -1) {
            long tempLong = atoi(&word[quoteIndex + 1]);
            if (endianIndex == -1) {
               out.writeBigEndian(tempLong);
            } else {
               out.writeLittleEndian(tempLong);
            }
            return;
         } else {
            ulong tempuLong = (ulong)atoi(&word[quoteIndex + 1]);
            if (endianIndex == -1) {
               out.writeBigEndian(tempuLong);
            } else {
               out.writeLittleEndian(tempuLong);
            }
            return;
         }
         break;
      default:
         cerr << "Error on line " << lineNumber << " at token: " << word 
              << endl;
         cerr << "invalid byte count specification for decimal number" << endl;
         exit(1);
   }

}



//////////////////////////////
//
// processHexadecimalWord -- interprets a hexadecimal word into
//     its constituent byte
//

void processHexadecimalWord(const char* word, int lineNumber, FileIO& out) {
   int length = strlen(word);
   uchar outputByte;

   if (length > 2) {
      cerr << "Error on line " << lineNumber << " at token: " << word << endl;
      cerr << "Size of hexadecimal number is too large.  Max is ff." << endl;
      exit(1);
   }

   if (!isxdigit(word[0]) || (length == 2 && !isxdigit(word[1]))) {
      cerr << "Error on line " << lineNumber << " at token: " << word << endl;
      cerr << "Invalid character in hexadecimal number." << endl;
      exit(1);
   }
   
   outputByte = (uchar)strtol(word, (char**)NULL, 16);
   out << outputByte;
}



//////////////////////////////
//
// processAsciiWord -- interprets a binary word into
//     its constituent byte
//

void processAsciiWord(const char* word, int lineNumber, FileIO& out) {
   int length = strlen(word);
   uchar outputByte;
  
   if (word[0] != '+') {
      cerr << "Error on line " << lineNumber << " at token: " << word << endl;
      cerr << "character byte must start with \'+\' sign: " << endl;
      exit(1);
   }

   if (length > 2) {
      cerr << "Error on line " << lineNumber << " at token: " << word << endl;
      cerr << "character byte word is too long -- specify only one character" 
           << endl;
      exit(1);
   }

   if (length == 2) {
      outputByte = (uchar)word[1];
   } else {
      outputByte = ' ';
   }
   out << outputByte;
}
  



//////////////////////////////
//
// processBinaryWord -- interprets a binary word into
//     its constituent byte
//

void processBinaryWord(const char* word, int lineNumber, FileIO& out) {
   int length = strlen(word);       // length of ascii binary number
   int commaIndex = -1;             // index location of comma in number
   int leftDigits = -1;             // number of digits to left of comma
   int rightDigits = -1;            // number of digits to right of comma
   int i = 0;

   // make sure that all characters are valid
   for (i=0; i<length; i++) {
      if (word [i] == ',') {
         if (commaIndex != -1) {
            cerr << "Error on line " << lineNumber << " at token: " << word 
                 << endl;
            cerr << "extra comma in binary number" << endl;
            exit(1);
         } else {
            commaIndex = i;
         }
      } else if (!(word[i] == '1' || word[i] == '0')) {
         cerr << "Error on line " << lineNumber << " at token: " << word 
              << endl;
         cerr << "Invalid character in binary number"
                 " (character is " << word[i] <<")" << endl;
         exit(1);
      }
   }

   // comma cannot start or end number
   if (commaIndex == 0) {
      cerr << "Error on line " << lineNumber << " at token: " << word
           << endl;
      cerr << "cannot start binary number with a comma" << endl;
      exit(1);
   } else if (commaIndex == length - 1 ) {
      cerr << "Error on line " << lineNumber << " at token: " << word
           << endl;
      cerr << "cannot end binary number with a comma" << endl;
      exit(1);
   }

   // figure out how many digits there are in binary number 
   // number must be able to fit into one byte.
   if (commaIndex != -1) {
      leftDigits = commaIndex;
      rightDigits = length - commaIndex - 1;
   } else if (length > 8) {
      cerr << "Error on line " << lineNumber << " at token: " << word
           << endl;
      cerr << "too many digits in binary number" << endl;
      exit(1);
   }
   // if there is a comma, then there cannot be more than 4 digits on a side
   if (leftDigits > 4) {
      cerr << "Error on line " << lineNumber << " at token: " << word
           << endl;
      cerr << "too many digits to left of comma" << endl;
      exit(1);
   }
   if (rightDigits > 4) {
      cerr << "Error on line " << lineNumber << " at token: " << word
           << endl;
      cerr << "too many digits to right of comma" << endl;
      exit(1);
   }

   // OK, we have a valid binary number, so calculate the byte
   
   uchar output = 0;
   
   // if no comma in binary number
   if (commaIndex == -1) {
      for (i=0; i<length; i++) {
         output = output << 1;
         output |= word[i] - '0';
      }
   } 
   // if comma in binary number
   else {
      for (i=0; i<leftDigits; i++) {
         output = output << 1;
         output |= word[i] - '0';
      }
      output = output << (4-rightDigits);
      for (i=0+commaIndex+1; i<rightDigits+commaIndex+1; i++) {
         output = output << 1;
         output |= word[i] - '0';
      }
   }

   // send the byte to the output
   out << output;
}
         


//////////////////////////////
//
// usage -- instructions on how to run the binasc program on the
//     command line.
//     its constituent byte
//

void usage(const char* command) {
   cout << 
   "                                                                     \n"
   "For converting/compiling a binary file to/from an ASCII listing of   \n"
   "individual bytes of the file.                                        \n"
   "                                                                     \n"
   "Usage: " << command << " [-a | -b | -c output] input(s)              \n"
   "                                                                     \n"
   "Options:                                                             \n"
   "   -a = output only non-space printable asci words                   \n"
   "   -b = output only hexadecimal ascii numbers for each byte          \n"
   "   -c output = compiled binary file using ascii number of input      \n"
   "   -m = display the man page for the program                         \n"
   "   no options = combination of -a and -b options.                    \n"
   "   --options  = list of all options, aliases and defaults            \n"
   << endl;
}



//////////////////////////////
//
// maunal -- verbose instructions to be printed out
//

void manual(void) {
cout << 
"binasc: binary/ascii file viewing/creation\n"
"\n"
"1. Displaying ASCII codes for bytes in a file.\n"
"\n"
"The binasc program can convert a file into an ASCII list of hexadecimal\n"
"numbers which represent each byte in the input file as well as\n"
"display any printable ascii characters associated with the hexadecimal\n"
"numbers. Example output given below shows beginning of the output from\n"
"the binasc program when it is run on the binasc program file. Note that\n"
"the lines come in pairs, first the line describing the bytes, then a\n"
"comment line displaying any ASCII printable bytes.\n"
"\n"
"\n"
" 7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 02 00 03 00 01 00 00 00 ac \n"
";    E  L  F                                                                \n"
"\n"
" 8c 04 08 34 00 00 00 68 5e 00 00 00 00 00 00 34 00 20 00 05 00 28 00 16 00 \n"
";          4           h  ^                    4                 (          \n"
"\n"
" 15 00 06 00 00 00 34 00 00 00 34 80 04 08 34 80 04 08 a0 00 00 00 a0 00 00 \n"
";                   4           4           4                               \n"
"\n"
" 00 05 00 00 00 04 00 00 00 03 00 00 00 d4 00 00 00 d4 80 04 08 d4 80 04 08 \n"
";                                                                           \n"
"\n"
" 13 00 00 00 13 00 00 00 04 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 \n"
";                                                                           \n"
"\n"
" 80 04 08 00 80 04 08 78 5a 00 00 78 5a 00 00 05 00 00 00 00 10 00 00 01 00 \n"
";                      x  Z        x  Z                                     \n"
"\n"
" 00 00 78 5a 00 00 78 ea 04 08 78 ea 04 08 2c 02 00 00 38 03 00 00 06 00 00 \n"
";       x  Z        x           x           ,           8                   \n"
"\n"
" 00 00 10 00 00 02 00 00 00 04 5c 00 00 04 ec 04 08 04 ec 04 08 a0 00 00 00 \n"
";                               \\                                           \n"
"\n"
" a0 00 00 00 06 00 00 00 04 00 00 00 2f 6c 69 62 2f 6c 64 2d 6c 69 6e 75 78 \n"
";                                     /  l  i  b  /  l  d  -  l  i  n  u  x \n"
"\n"
" 2e 73 6f 2e 32 00 00 25 00 00 00 38 00 00 00 00 00 00 00 0d 00 00 00 20 00 \n"
"; .  s  o  .  2        %           8                                        \n"
"\n"
"There are two other main viewing options for the binasc command: -a for\n"
"displaying only ASCII printable bytes, and -b for displaying only the\n"
"hexadecimal numbers for the bytes.\n"
"\n"
"the -a option will display only the ascii-printable characters in a\n"
"file. Multiple spaces (unprintable characters) are suppressed in the\n"
"output. The -a option is a good way to search for text in a binary\n"
"file. Here is an example output using the same file as in the example\n"
"show above:\n"
"\n"
"ELF 4 h^ 4 ( 4 4 4 xZ xZ xZ x x , 8 \\ /lib/ld-linux.so.2 % 8 # / 5 ! % , \"\n"
"( < > ( 8 @ ( = D > K > e , v 0 , ) E . l I l 3 y E | Q i a C \\ | \' | ! !\n"
"__gmon_start__ libg++.so.2.7.2 _DYNAMIC _GLOBAL_OFFSET_TABLE_ _init _fini\n"
"__builtin_vec_new __builtin_delete __builtin_new __builtin_vec_delete\n"
"__ls__7ostreamPCc __ctype_b __ctype_tolower write__7ostreamPCci\n"
"get__7istreamRc _vt.3ios _vt.7ostream.3ios __ls__7ostreami cerr exit\n"
"__strtod_internal __ls__7ostreamc cout strchr strcmp atexit\n"
"libstdc++.so.2.7.2 __11fstreambasei _vt.7istream.3ios _vt.8ifstream.3ios\n"
"__11fstreambaseiPCcii open__11fstreambasePCcii _vt.8iostream.3ios\n"
"_vt.7fstream.3ios close__11fstreambase _._7fstream _._8ifstream\n"
"getline__7istreamPcic read__7istreamPci hex__FR3ios __ls__7ostreaml\n"
"endl__FR7ostream libm.so.6 libc.so.6 __libc_init_first bsearch qsort\n"
"__strtol_internal strcpy strncpy strtok _environ __environ environ _start\n"
"\n"
"Alternatively, with the -b option, you can print out only the ASCII codes\n"
"for the binary numbers associated with each byte in the file. Unlike\n"
"the unix od command, the bytes are not grouped into two-byte words when\n"
"displayed as hexadecimal numbers (which will switch order of the bytes\n"
"in the output display on little-endian computers). Here is example output\n"
"when using the -b option using the same file as in previous examples:\n"
"\n"
"\n"
"7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 02 00 03 00 01 00 00 00 ac \n"
"8c 04 08 34 00 00 00 68 5e 00 00 00 00 00 00 34 00 20 00 05 00 28 00 16 00 \n"
"15 00 06 00 00 00 34 00 00 00 34 80 04 08 34 80 04 08 a0 00 00 00 a0 00 00 \n"
"00 05 00 00 00 04 00 00 00 03 00 00 00 d4 00 00 00 d4 80 04 08 d4 80 04 08 \n"
"13 00 00 00 13 00 00 00 04 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 \n"
"80 04 08 00 80 04 08 78 5a 00 00 78 5a 00 00 05 00 00 00 00 10 00 00 01 00 \n"
"00 00 78 5a 00 00 78 ea 04 08 78 ea 04 08 2c 02 00 00 38 03 00 00 06 00 00 \n"
"00 00 10 00 00 02 00 00 00 04 5c 00 00 04 ec 04 08 04 ec 04 08 a0 00 00 00 \n"
"a0 00 00 00 06 00 00 00 04 00 00 00 2f 6c 69 62 2f 6c 64 2d 6c 69 6e 75 78 \n"
"2e 73 6f 2e 32 00 00 25 00 00 00 38 00 00 00 00 00 00 00 0d 00 00 00 20 00 \n"
"00 00 15 00 00 00 00 00 00 00 07 00 00 00 0b 00 00 00 23 00 00 00 01 00 00 \n"
"00 1d 00 00 00 14 00 00 00 16 00 00 00 0c 00 00 00 00 00 00 00 2f 00 00 00 \n"
"0e 00 00 00 00 00 00 00 00 00 00 00 35 00 00 00 19 00 00 00 21 00 00 00 1f \n"
"\n"
"2. Creating files by byte description\n"
"\n"
"With the binasc program, you can convert an ascii file with the binary\n"
"numbers back into actual bytes by using the -c option. When using the -c\n"
"option, you must specify an output file with the -o option. Byte numbers\n"
"can be of various formats as described below.\n"
"\n"
"binasc comments\n"
"\n"
"   A semi-colon (;) marks the beginning of a comment which extends to the\n"
"   end of a line. A space (or tab) character must precede the semi-colon\n"
"   when the comment follows a number on a line.\n"
"\n"
"binasc hexadimal numbers\n"
"\n"
"   hexadecimal numbers specify one byte and must contain no more than 2\n"
"   digits and range from 00 to ff (0 to 255 in decimal notation, or -128\n"
"   to 127 in signed decimal notation.): example of valid hexadecimal\n"
"   numbers:\n"
"\n"
"\n"
"   7f 45 4c 46 1 1 1 0 0 \n"
"   8c 04 08 34 0 0 0 8 e \n"
"   15 00 06 10 0 0 4 0 0 \n"
"\n"
"binasc binary numbers\n"
"\n"
"   Binary numbers can be specified by numbers longer than three\n"
"   characters, or numbers containing a comma. The binary number is allowed\n"
"   to have up to 8 digits (bits) since the binary number represents one\n"
"   byte in the output file. An optional comma is expected to split the\n"
"   number into two equal parts with 4 bits on each side of the comma\n"
"\n"
"   For example 0010 is the binary number which is equal to the decimal\n"
"   number 4. 0010 is equivalent to 0,0010 or 0000,0010. Note that 10\n"
"   is the hexadecimal number equal to the decimal number 16 and is\n"
"   not the binary number equal to the decimal number 2.\n"
"\n"
"   Here are some binary numbers examples:\n"
"\n"
"\n"
"   binary    decimal        invalid     reason\n"
"   0,0       = 0              ,0          cannot start or end with comma\n"
"   0000,0000 = 0              0000, 0000  cannot have spaces around comma\n"
"   00000000  = 0              1001010110  maximum of 8 binary digits\n"
"   1,1       = 17             10011,1000  max of 4 digits each side of comma\n"
"   0001,0001 = 17             \n"
"   010       = 2              10          interpreted as a hexadecimal number\n"
"   0,101     = 5              \n"
"   00000101  = 5\n"
"   101       = 5\n"
"\n"
"binasc decimal numbers\n"
"\n"
"   binasc decimal numbers, unlike hexadecimal or binary numbers, can fill\n"
"   slots of 1-4 bytes for integers, or 4 and 8 bytes for floating-point\n"
"   decimal numbers. Decimal numbers may also be either positive or\n"
"   negative unlike the hexadecimal or binary number input.\n"
"\n"
"   A decimal number starts with a quote character. There are two\n"
"   specifications which can be given just before the quote:\n"
"\n"
"   1. a number in the range from 1 to 4 which specifies how many bytes an\n"
"   integer decimal number is to be stored in. Floating-point numbers can\n"
"   be either 4 or 8 bytes in size. The default size for floating-point\n"
"   numbers is 4 bytes if no size is specified.\n"
"\n"
"   2. the symbol u can be given before the quote character in a\n"
"   decimal number to indicate the direction into which the bytes for the\n"
"   number will be placed in the file. No letter u means that the most\n"
"   significan byte is written first (big-endian)., while the letter u\n"
"   indicates to write the bytes in reverse order (little-endian). For\n"
"   example, the decimal number 1234 can be represented by the two-byte\n"
"   hexadecimal number 0x04d2. In big-endian storage the 04 byte is\n"
"   written first, then the d2 byte. in little-endian storage the d2 byte\n"
"   is written first then the 04 byte.\n"
"\n"
"   1234 = 0x04d2       big endian:   04 d2      little endian:   d2 04\n"
"\n"
"   When a byte size is not specified before the quote character (* here), the\n"
"   default is 1 for integers and 4 for floating-point. When not speifying\n"
"   a byte size, valid decimal numbers are in the range from 0 to 255, or\n"
"   -128 to 127 if signed, i.e., the range for one-byte decimal numbers\n"
"   is from -128 to 255, and you have to know the representation later\n"
"   (signed or unsigned). If you specifically specify a byte size of 1,\n"
"   then you can give any integer number value which may be truncated to\n"
"   fit into one byte. The maxmum integer decimal number which can fill 4\n"
"   bytes is 4294967294 or so. (hexadecimal 0xffffffff).\n"
"\n"
"   If a decimal number includes a period character (.) it is assumed to\n"
"   be a floating-point number. Floating-point numbers can be either 4\n"
"   or 8 bytes. Integer numbers can be between 1 and 4 bytes, but 3-byte\n"
"   integers can only be positive.\n"
"\n"
"   Examples of decimal numbers:\n"
"                      \n"
"     valid                     invalid \n"
"     examples                  examples      reason\n"
"     *0      =    0               123         does not start with a quote\n"
"     *255    =  255             \n"
"     1*256   =   0 (truncated)   *256         exceeds one byte in size\n"
"     2*256   = 256\n"
"     4*44100 = 44100\n"
"     4u*453  = 453 (but bytes are written small to large order)\n"
"     u4*453  = 453 (same as above)\n"
"     2*-5    = -5  (short int)   2* -5        cannot have a space around quote\n"
"     *3.1415 = 3.1415 (4-byte storage, float in c)\n"
"     8*3.1415 = 3.1415 (8-byte storage, double in c)\n"
"\n"
"\n"
"binasc ascii bytes\n"
"\n"
"   ASCII characters can be input by preceding each with a plus (+). Each\n"
"   character is a separate word. For example, to place the characters\n"
"   cat into a file, the input would be +c +a +t.\n"
"\n"
"example 1\n"
"\n"
"The following file will compile into a NeXT/Sun soundfile with five\n"
"zero-valued sound samples. This example has lots of comments.\n"
"\n"
"\n"
<<
" 2e 73 6e 64      ; the magic number which identifies the type of the file\n"
"; .  s  n  d      ; character equivalents of the magic number digits\n"
"\n"
"\n"
"00 00 00 32       ; the byte offset of the data (50 bytes precede the data)\n"
"                  ; i.e., the header contains 50 bytes\n"
"\n"
"00 00 00 0a       ; the number of bytes in the data (10 bytes).\n"
"\n"
"00 00 00 03       ; the NeXT/sun data format (3 = 16-bit Linear sound)\n"
"\n"
"00 00 ac 44       ; the sampling rage, which is 44100 samples/sec here\n"
"\n"
"00 00 00 01       ; the number of channels (1 = monophonic soundfile)\n"
"\n"
"                  ; next comes a sound file comment:\n"
" 54 68 69 73 20 69 73 20 61 20 62 6c 61 6e 6b 20 73 6f 75 6e 64 66 69 6c 65 2e\n"
"; T  h  i  s     i  s     a     b  l  a  n  k     s  o  u  n  d  f  i  l  e  .\n"
"\n"
"; finally the individual sample data:\n"
"\n"
"00 00       ; first 16-bit sample (big-endian)\n"
"00 00       ; second 16-bit sample (big-endian)\n"
"00 00       ; third 16-bit sample (big-endian)\n"
"00 00       ; fourth 16-bit sample (big-endian)\n"
"00 00       ; fifth 16-bit sample (big-endian)\n"
"\n"
"; end of example soundfile.\n"
"\n"
"Here is a more succinct version of the previous example:\n"
"\n"
"\n"
"+. +s +n +d      ; magic number (characters .snd)\n"
"4*50             ; header bytes (the decimal number 50 filling 4 bytes)\n"
"4*10             ; sample count\n"
"4*3              ; format\n"
"4*44100          ; srate\n"
"4*1              ; channels\n"
"                 ; comment:\n"
"+T +h +i +s +  +i +s +  +a +  +b +l +a +n +k +  +s +o +u +n +d +f +i +l +e +.\n"
"\n"
"                 ; sample data shown in various input possibilities\n"
"00 00            ; sample 1: hexadecimal digits\n"
"*0 *0            ; sample 2: decimal digits\n"
"2*0              ; sample 3: decimal number 0 filling up two bytes\n"
"0000,0000 0,0    ; sample 4: binary digits\n"
"2u*0             ; sample 5: decimal digits filling two bytes, but\n"
"                 ; using little endian byte ordering (backward).\n"
"; end of example soundfile.\n"
"\n"
"The simplest view of the previous example:\n"
"\n"
"\n"
"2e 73 6e 64 00 00 00 32 00 00 00 0a 00 00 00 03 00 00 ac 44 00 00 00 01 \n"
"54 68 69 73 20 69 73 20 61 20 62 6c 61 6e 6b 20 73 6f 75 6e 64 66 69 6c \n"
"65 2e 00 00 00 00 00 00 00 00 00 00 \n"
"\n"
"example 2\n"
"\n"
"Just for fun, here is a WAVE format soundfile with the same contents as\n"
"the previous examples (5 zero-valued samples). Notice that most data\n"
"fields in the file are little-endian forms of numbers (since Intel\n"
"computers are little-endian).\n"
"\n"
"\n"
"52 49 46 46 2e 00 00 00 57 41 56 45 66 6d 74 20 10 00 00 00 01 00 01 00 \n"
"44 ac 00 00 88 58 01 00 02 00 10 00 64 61 74 61 0a 00 00 00 00 00 00 00 \n"
"00 00 00 00 00 00 \n"
"\n"
"Which is equivalent to:\n"
"\n"
"\n"
"; This is a WAVE formated soundfile with 5 zero samples.\n"
"+R +I +F +F           ; RIFF chunk descriptor\n"
"4u.46                 ; size of the chunk in bytes\n"
"+W +A +V +E           ; format is the type of RIFF that follows\n"
"+f +m +t +            ; the fmt sub chunk\n"
"4u*16                 ; number of bytes total in sub-chuck which follow\n"
"2u*1                  ; audio format (PCM Linear)\n"
"2u*1                  ; number of channels\n"
"2u*44100              ; sampling rate 44100 = ac 44, 2u.44100 = 44 ac\n"
"4u*88200              ; byte rate = srate * channels * bitspersample / 8.\n"
"2u*2                  ; block align (bytes per sample / 8)\n"
"2u*16                 ; bits per sample\n"
"+d +a +t +a           ; data subchunk\n"
"4u*10                 ; size of data subchunk in bytes which follows\n"
"2u*0                  ; sample 1\n"
"2u*0                  ; sample 2\n"
"2u*0                  ; sample 3\n"
"2u*0                  ; sample 4\n"
"2u*0                  ; sample 5\n"
"; end of example wave file.\n"
"\n"
"\n"
"Note that you can reverse the process of the binasc program unless you\n"
"specify the -a option:\n"
"\n"
"\n"
"   binasc file1 > file2\n"
"   binasc -c file2 > file3\n"
"   ; file1 and file3 should be the same\n"
"\n"
"   binasc -b file1 > file2\n"
"   binasc -c file2 > file3\n"
"   ; file1 and file3 should be the same\n"
"\n"
"   binasc -a file1 > file2\n"
"   binasc -c file2 > file3        ; this results in an error\n"
"\n"
<< endl;
}

// md5sum: 4324db14f9ca74c3b516e42721745b1e binasc.cpp [20061014]
