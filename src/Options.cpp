//
// Copyright 1998-2000 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Sat Mar 27 18:17:06 PST 1999
// Last Modified: Thu Apr 13 14:02:52 PDT 2000 (added 2nd define function)   
// Last Modified: Fri May  5 17:52:01 PDT 2000 (added --options suppression)
// Last Modified: Tue May  1 01:25:58 PDT 2001 (fixed getArgumentCount())
// Last Modified: Mon Oct 20 07:56:41 PDT 2008 (allow for secondary user input)
// Filename:      ...sig/maint/code/sigBase/Options.cpp
// Web Address:   http://sig.sapp.org/src/sigBase/Options.cpp
// Documentation: http://sig.sapp.org/doc/classes/Options
// Syntax:        C++ 
//
// Description:   Handles command-line options in a graceful manner.
//

int optionListCompare(const void* a, const void* b);

#include "Options.h"
#include "Options_private.h"

#include <stdlib.h>
#include <string.h>
#include <cctype>

#ifndef OLDCPP
   #include <iostream>
   using namespace std;
#else
   #include <iostream.h>
#endif


//////////////////////////////
//
// Options::Options --
//

Options::Options(void) {
   optionFlag = '-';
   gargc = -1;
   gargv = NULL;
   argument.setSize(0);
   argument.allowGrowth();
   optionRegister.setSize(0);
   optionRegister.allowGrowth();
   optionList.setSize(0);
   optionList.allowGrowth();
   processedQ = 0;
   sortedQ = 0;
   commandString = NULL;
   options_error_check = 1;
   suppressQ = 0;
   optionsArgument = 0;

   extraArgv.setSize(100);
   extraArgv.setGrowth(100);
   extraArgv.setSize(0);
   extraArgv_strings.setSize(100);
   extraArgv_strings.setGrowth(100);
   extraArgv_strings.setSize(0);
}


Options::Options(int argc, char** argv) {
   optionFlag = '-';
   gargc = -1;
   gargv = NULL;
   argument.setSize(0);
   argument.allowGrowth();
   optionRegister.setSize(0);
   optionRegister.allowGrowth();
   optionList.setSize(0);
   optionList.allowGrowth();
   processedQ = 0;
   sortedQ = 0;
   commandString = NULL;
   options_error_check = 1;
   suppressQ = 0;
   optionsArgument = 0;

   extraArgv.setSize(100);
   extraArgv.setGrowth(100);
   extraArgv.setSize(0);
   extraArgv_strings.setSize(100);
   extraArgv_strings.setGrowth(100);
   extraArgv_strings.setSize(0);

   setOptions(argc, argv);
}



//////////////////////////////
//
// Options::~Options --
//

Options::~Options() {
   reset();
}



//////////////////////////////
//
// Options::argc -- returns the argument count as from main().
//

int Options::argc(void) const {
   return gargc;
}



//////////////////////////////
//
// Options::argv -- returns the arguments strings as from main().
//

char** Options::argv(void) const {
   return gargv;
}



//////////////////////////////
//
// Options::define -- define an option entry in the option register
//

void Options::define(const char* aDefinition) {
   sortedQ = 0;                        // have to sort option list later
   option_register* definitionEntry;
   option_list* optionListEntry;


   // error if the definition string doesn't contain an equals sign.
   if (strchr(aDefinition, '=') == NULL) {
      cout << "Error: no \"=\" in option definition: " << aDefinition << endl;
      exit(1);
   }

   // get the length of the definition string
   int len = strlen(aDefinition);

   // put space before and after the equals sign so that strtok works
   char* definitionString;
   definitionString = new char[len + 3];
   int i = 0;
   while (aDefinition[i] != '=' && i < len) {
      definitionString[i] = aDefinition[i];
      i++;
   }
   definitionString[i] = ' ';
   i++;
   definitionString[i] = '=';
   i++;
   definitionString[i] = ' ';
   for (int k=i; k<len+2; k++) {
      definitionString[k+1] = aDefinition[k-1];
   }
   len += 2;

   // determine the registry index for the definition
   int definitionIndex = optionRegister.getSize();


   // set up space for a definition entry
   definitionEntry = new option_register(aDefinition, OPTION_UNKNOWN_TYPE,
      "", NULL);


   char *tempstr;  // temporary storage for strtok use on defaultString
   tempstr = new char[len + 1];
   strncpy(tempstr, definitionString, len);

   char *optionName;   // option name to add to list

   // get the first option name
   optionName = strtok(tempstr, " \t\n|");
   if (optionName[0] == '=') {
      cout << "Error: must specify an option name in definition: " 
           << aDefinition << endl;
      exit(1);
   }
   // store an entry for each option name alias
   const char* tempsearch;
   while (optionName != NULL && optionName[0] != '=') {
      tempsearch = getDefinition(optionName);
      if (tempsearch != NULL) {
         cerr << "option name \"" << optionName 
              << "\" from definition: " << aDefinition << endl;
         cerr << "is already defined in definition: "
              << tempsearch << endl;
         exit(1);
      }
      optionListEntry = new option_list(optionName, definitionIndex);      
      optionList.append(optionListEntry);
      optionName = strtok(NULL, " \t\n|");
   }
   if (optionName == NULL) {
      cout << "Error: unknown error in definition: " << aDefinition << endl;
      exit(1);
   }

   // now process the option type and any default value.
   i = 0;

   // find the equals sign
   while (definitionString[i] != '=' && i < len) {
      i++;
   }
   i++;
   // skip over any white space
   while (std::isspace(definitionString[i]) && i < len) {
      i++;
   }

   // this character must be the option type
   char optionType = (char)std::tolower(definitionString[i]);
   definitionEntry->setType(optionType);
   i++;
      

   // check to make sure that the type is correct.
   if (optionType != OPTION_STRING_TYPE &&
         optionType != OPTION_INT_TYPE    &&
         optionType != OPTION_FLOAT_TYPE  &&
         optionType != OPTION_DOUBLE_TYPE &&
         optionType != OPTION_BOOLEAN_TYPE &&
         optionType != OPTION_CHAR_TYPE ) {
      cout << "Error: unknown option type \'" << optionType
           << "\' in defintion: " << aDefinition << endl;
      exit(1);
   }

   // skip any white space after option type.
   while (std::isspace(definitionString[i]) && i < len) {
      i++; 
   }


   // there can only be two characters now: '\0' or ':'
   if (i >= len || definitionString[i] == '\0') {
      goto option_finish;
   } else if (i<len && definitionString[i] == ':') {
      i++;
   } else {
      cout << "Unknown error in definition: " << aDefinition << endl;
      exit(1); 
   }


   // now proces the default string.  store it in a temp storage for copying

   // skip any white space after option type.
   while (i < len && std::isspace(definitionString[i])) {
      i++; 
   }
   if (i >= len || definitionString[i] == '\0') {
      goto option_finish;
   }
   

   // now at beginnng of default option string which continues
   // until the end of the definition string.
   definitionEntry->setDefault(&definitionString[i]);

option_finish:

   optionRegister.append(definitionEntry);


   delete [] definitionString;
   delete [] tempstr;

}


void Options::define(const char* aDefinition, const char* description) {
   define(aDefinition);

   // now find some place to store the description...
}



//////////////////////////////
//
// Options::getArg -- returns the specified argument.
//	argurment 0 is the command name.
//

char* Options::getArg(int index) {
   if (index < 0 || index >= argument.getSize()) {
      cout << "Error: argument " << index << " does not exist." << endl;
      exit(1);
   }
   return argument[index];
}



//////////////////////////////
//
// Options::getArgument -- same as getArg
//

char* Options::getArgument(int index) {
   return getArg(index);
}



//////////////////////////////
//
// Options::getArgCount --  number of arguments on command line.
//	does not count the options or the command name.
//

int Options::getArgCount(void) {
   return argument.getSize() - 1;
}



//////////////////////////////
//
// Options::getArgumentCount -- Same as getArgCount().
//

int Options::getArgumentCount(void) {
   return getArgCount();
}



//////////////////////////////
//
// Options::getArgList -- return a **char list of the arguments
//     after the options have been parsed out of it.  The last
//     item in the **char list is a NULL termination.
//

char** Options::getArgList(void) {
   argument.increase(1);
   argument.last() = NULL;
   argument.decrease(1);
   return argument.getBase()+1;
}



//////////////////////////////
//
// Options::getArgumentCount -- Same as getArgCount().
//

char** Options::getArgumentList(void) { 
   return getArgList();
}



//////////////////////////////
//
// Options::getBoolean --  returns true if the option was
//	used on the command line.
//

int Options::getBoolean(const char* optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return 0;
   }
   if (optionRegister[index]->getModifiedQ() == 1) {
      return 1;
   } else { 
      return 0;
   }
}



//////////////////////////////
//
// Options::getCommand -- returns argv[0]
//

const char* Options::getCommand(void) {
   if (argument.getSize() == 0) {
      return "";
   } else {
      return argument[0];
   }
}



//////////////////////////////
//
// Options::getCommandLine -- returns a string which contains the
//     command-line call to the program.
//

const char* Options::getCommandLine(void) {
   if (commandString != NULL) {
      return commandString;
   }

   int length = 0;
   int i;
   for (i=0; i<gargc; i++) {
      length += strlen(gargv[i]) + 1;
   }
   length--;   // remove the last space
   commandString = new char[length + 1];
   strcpy(commandString, gargv[0]);
   for (i=1; i<gargc; i++) {
      strcat(commandString, " ");
      strcat(commandString, gargv[i]);
   }

   return commandString;
}
   



//////////////////////////////
//
// Options::getDefinition -- returns the definition
//	for the specified option name.  Returns NULL
//	if there is no entry for the option name.
//	It is assumed that the option list is unsorted.
//	spaces count in the input option name.
//

const char* Options::getDefinition(const char* optionName) {
   int i;
   for (i=0; i<optionList.getSize(); i++) {
      if (strcmp(optionName, optionList[i]->getName()) == 0) {
         return optionRegister[optionList[i]->getIndex()]->getDefinition();
      }
   }
   return (const char*)NULL;
}



//////////////////////////////
//
// Options::getDouble -- returns the double float associated
//	with the given option.  Returns 0 if there is no
//	number associated with the option.
//

double Options::getDouble(const char* optionName) {
   return strtod(getString(optionName), (char**)NULL);
}



//////////////////////////////
//
// Options::getFlag --
//

char Options::getFlag(void) {
   return optionFlag;
}



//////////////////////////////
//
// Options::getFloat -- returns the floating point number
//	associated with the given option.
//

float Options::getFloat(const char* optionName) {
   return (float)getDouble(optionName);
}



//////////////////////////////
//
// Options::getInt -- returns the integer argument.  Can handle
//	hexadecimal, decimal, and octal written in standard 
//	C syntax.
//

int Options::getInt(const char* optionName) {
   return (int)strtol(getString(optionName), (char**)NULL, 0);
}

int Options::getInteger(const char* optionName) {
   return getInt(optionName);
}



//////////////////////////////
//
// Options::getString --
//

const char* Options::getString(const char* optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return "UNKNOWN OPTION";
   } else {
      return optionRegister[index]->getOption();
   }
}



//////////////////////////////
//
// Options::optionsArg -- returns true if the --options is present
//    on the command line, otherwise returns false.
//

int Options::optionsArg(void) {
   return optionsArgument;
}



//////////////////////////////
//
// Options::print --
//

void Options::print(void) {
   for (int i=0; i<optionRegister.getSize(); i++) {
      cout << optionRegister[i]->getDefinition() << endl;
   }
}



//////////////////////////////
//
// Options::reset --
//

void Options::reset(void) {
   int i;
   for (i=0; i<optionRegister.getSize(); i++) {
      delete optionRegister[i];
   }
   optionRegister.setSize(0);
   for (i=0; i<optionList.getSize(); i++) {
      delete optionList[i];
   }
   optionList.setSize(0);
   for (i=0; i<argument.getSize(); i++) {
      delete [] argument[i];
   }
   argument.setSize(0);
  
   if (commandString != NULL) {
      delete [] commandString;
   }

   extraArgv.setSize(0);
   extraArgv_strings.setSize(0);

   gargc = 0;
   gargv = NULL;
}



//////////////////////////////
//
// Options::setFlag -- set the character used to indicate an 
//	option.  For unix this is usually '-', in MS-DOS,
//	this is usually '/';  But the syntax of the Options
//	class is for Unix-style options.
//

void Options::setFlag(char aFlag) {
   optionFlag = aFlag;
}




//////////////////////////////
//
// setModified -- 
//

void Options::setModified(const char* optionName, const char* aString) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return;
   }
   
   optionRegister[getRegIndex(optionName)]->setModified(aString);
}




//////////////////////////////
//
// setOptions -- 
//

void Options::setOptions(int argc, char** argv) {
   processedQ = 0;

   // data used to be stored directly here:
   //gargc = argc;
   //gargv = argv;
   // but now gets interfaced to: extraArgv and extraArgv_strings:


/*
 * int oldsize = extraArgv.getSize();
 * extraArgv.setSize(oldsize + argc);
 * extraArgv_strings.setSize(oldsize + argc);
 * 
 * for setOptions, the old options are erased.  If you want to
 * add additional options, then use appendOptions instead.
 */
   extraArgv.setSize(1);
   extraArgv[0] = NULL;
   extraArgv.setSize(argc);
   extraArgv_strings.setSize(argc);
   int oldsize = 0;

   int i;
   int length;
   for (i=0; i<argc; i++) {
      length = strlen(argv[i]);
      extraArgv_strings[i+oldsize].setSize(length+1);
      strcpy(extraArgv_strings[i+oldsize].getBase(), argv[i]);
      extraArgv[i] = extraArgv_strings[i].getBase();
   }

   gargc = extraArgv.getSize();
   gargv = extraArgv.getBase();
}



//////////////////////////////
//
// appendOptions -- add argc and argv data to the current
// data residing inside the Options class variable.
//

void Options::appendOptions(int argc, char** argv) {
   processedQ = 0;

   // data used to be stored directly here:
   //gargc = argc;
   //gargv = argv;
   // but now gets interfaced to: extraArgv and extraArgv_strings:

   int oldsize = extraArgv.getSize();
   extraArgv.setSize(oldsize + argc);
   extraArgv_strings.setSize(oldsize + argc);

   int i;
   int length;
   for (i=0; i<argc; i++) {
      length = strlen(argv[i]);
      extraArgv_strings[i+oldsize].setSize(length+1);
      strcpy(extraArgv_strings[i+oldsize].getBase(), argv[i]);
      extraArgv[i+oldsize] = extraArgv_strings[i+oldsize].getBase();
   }

   gargc = extraArgv.getSize();
   gargv = extraArgv.getBase();

}



//////////////////////////////
//
// appendOptions -- parse the string like command-line arguments.
//   Either double or single quotes can be used to encapsulate
//   a command-line token.  If double quotes are used to encapsulate,
//   then you will not have to back quote single quotes inside the
//   token string, but you will have to backslash double quotes:
//      "-T \"\"" but "-T ''"
//   Likewise for single quotes in reverse with double quotes:
//      '-T \'\'' is equal to: '-T ""'
//

void Options::appendOptions(const char* strang) {
   int i;
   int doublequote = 0;
   int singlequote = 0;

   Array<Array<char> > tokens;
   Array<char*> tempargv;

   tokens.setSize(100);
   tokens.setGrowth(100);
   tokens.setSize(0);
   tempargv.setSize(100);
   tempargv.setGrowth(100);
   tempargv.setSize(0);


   Array<char> tempvalue;
   tempvalue.setSize(1000);
   tempvalue.setGrowth(1000);
   tempvalue.setSize(0);

   char ch;

   int length = strlen(strang);
   for (i=0; i<length; i++) {

      if (!singlequote && (strang[i] == '"')) {
         if ((i>0) && (strang[i-1] != '\\')) {
            doublequote = !doublequote;
            if (doublequote == 0) {
               // finished a doublequoted section of data, so store
               // even if it is the empty string
               ch = '\0';
               tempvalue.append(ch);
               tokens.append(tempvalue);
               tempvalue.setSize(0);
               continue;
            } else {
               // don't store the leading ":
               continue;
            }
         }
      } else if (!doublequote && (strang[i] == '\'')) {
         if ((i>0) && (strang[i-1] != '\\')) {
            singlequote = !singlequote;
            if (singlequote == 0) {
               // finished a singlequote section of data, so store
               // even if it is the empty string
               ch = '\0';
               tempvalue.append(ch);
               tokens.append(tempvalue);
               tempvalue.setSize(0);
               continue;
            } else {
               // don't store the leading ":
               continue;
            }
         }
      }


      if ((!doublequote && !singlequote) && std::isspace(strang[i])) {
         if (tempvalue.getSize() > 0) {
            ch = '\0';
            tempvalue.append(ch);
            tokens.append(tempvalue);
            tempvalue.setSize(0);
         }
      } else {
         ch = strang[i];
         tempvalue.append(ch);
      }
   }
   if (tempvalue.getSize() > 0) {
      ch = '\0';
      tempvalue.append(ch);
      tokens.append(tempvalue);
      tempvalue.setSize(0);
   }

   // now that the input string has been chopped up into pieces,
   // assemble the argv structure

   tempargv.setSize(tokens.getSize());
   for (i=0; i<tempargv.getSize(); i++) {
      tempargv[i] = tokens[i].getBase();
   }

   // now store the argv and argc data in opts:

   int targc = tempargv.getSize();
   char** targv = tempargv.getBase();

   // now store the parsed command-line simulated tokens
   // for actual storage:
   appendOptions(targc, targv);
}



//////////////////////////////
//
// Options:getType -- returns the type of the option
//

char Options::getType(const char* optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return -1;
   } else {
      return optionRegister[getRegIndex(optionName)]->getType();
   }
}



//////////////////////////////
//
// Options::process -- same as xverify
//   	default values: error_check = 1, suppress = 0;
//

void Options::process(int argc, char** argv, int error_check, int suppress) {
   setOptions(argc, argv);
   xverify(error_check, suppress);
}


void Options::process(int error_check, int suppress) {
   xverify(error_check, suppress);
}



//////////////////////////////
//
// Options::xverify --
//	default value: error_check = 1, suppress = 0;
//

void Options::xverify(int error_check, int suppress) {
   options_error_check = error_check;
   int gargp = 1;
   int optionend = 0;

   if (suppress) {
      suppressQ = 1;
   } else {
      suppressQ = 0;
   }

   // if calling xverify again, must remove previous argument list.
   if (argument.getSize() != 0) {
      for (int j=0; j<argument.getSize(); j++) {
         delete argument[j];
      }
      argument.setSize(0);
   }

   char* tempargument;
   tempargument = new char[strlen(gargv[0])+1];
   strcpy(tempargument, gargv[0]);
   argument.append(tempargument);

   int oldgargp;
   int position = 0;
   int running = 0;
   while (gargp < gargc && optionend == 0) {
      if (optionQ(gargv[gargp], gargp)) {
         oldgargp = gargp;
         gargp = storeOption(gargp, position, running);
         if (gargp != oldgargp) {
            running = 0;
            position = 0;
         }
      } else {
         if ( strlen(gargv[gargp]) == 2 && gargv[gargp][0] == getFlag() &&
            gargv[gargp][2] == getFlag() ) {
               optionend = 1;
            gargp++;
            break;
         } else {                          // this is an argument
            tempargument = new char[strlen(gargv[gargp])+1];
            strcpy(tempargument, gargv[gargp]);
            argument.append(tempargument);
            gargp++;
         }
      }
   }

   while (gargp < gargc) {
      tempargument = new char[strlen(gargv[gargp])+1];
      strcpy(tempargument, gargv[gargp]);
      argument.append(tempargument);
      gargp++;
   }

}


void Options::xverify(int argc, char** argv, int error_check, int suppress) {
   setOptions(argc, argv);
   xverify(error_check, suppress);
}


   

///////////////////////////////////////////////////////////////////////////
//
// private functions
//


//////////////////////////////
//
// getRegIndex -- returns the index of the option associated
//	with this name.
//

int Options::getRegIndex(const char* optionName) {
   if (suppressQ && strcmp("options", optionName) == 0) {
         return -1;
   } 

   if (sortedQ == 0) {
      sortOptionNames();
   }
   option_list key(optionName, -1);
   option_list* keyp = &key;
   void* searchresult;
   searchresult = bsearch(&keyp, optionList.getBase(), 
      optionList.getSize(), sizeof(option_list*), optionListCompare);
   if (searchresult != NULL) {
      return (*((option_list**)searchresult))->getIndex();
   } else if (strcmp("options", optionName) == 0) {
      print();
      exit(0);
   } 

   if (options_error_check) {
      cout << "Error: unknown option \"" << optionName << "\"." << endl;
      print();
      exit(1);
   }
 
   return -1;   
}



//////////////////////////////
//
// optionQ --  returns true if the string is an option
//	"--" is not an option, also '-' is not an option.
//	aString is assumed to not be NULL.
//

int Options::optionQ(const char* aString, int& argp) {
   if (aString[0] == getFlag()) {
      if (aString[1] == '\0') {
         argp++; 
         return 0;
      } else if (aString[1] == getFlag()) {
         if (aString[2] == '\0') {
            argp++;
            return 0;
         } else {
            return 1;
         }
      } else {
         return 1;
      }
   } else {
      return 0;
   }
}




//////////////////////////////
//
// sortOptionNames --
//

void Options::sortOptionNames(void) {
   qsort(optionList.getBase(), optionList.getSize(), 
      sizeof(option_list*), optionListCompare);
   sortedQ = 1;
}



//////////////////////////////
//
// storeOption -- 
//

#define OPTION_FORM_SHORT     0
#define OPTION_FORM_LONG      1
#define OPTION_FORM_CONTINUE  2

int Options::storeOption(int gargp, int& position, int& running) {
   int optionForm;
   char tempname[1024];
   char optionType = '\0';

   if (running) {
      optionForm = OPTION_FORM_CONTINUE;
   } else if (gargv[gargp][1] == getFlag()) {
      optionForm = OPTION_FORM_LONG;
   } else {
      optionForm = OPTION_FORM_SHORT;
   }

   switch (optionForm) {
      case OPTION_FORM_CONTINUE:
         position++;
         tempname[0] = gargv[gargp][position];
         tempname[1] = '\0';
         optionType = getType(tempname);
         if (optionType != OPTION_BOOLEAN_TYPE) {
            running = 0;
            position++;
         }
         break;
      case OPTION_FORM_SHORT:   
         position = 1;
         tempname[0] = gargv[gargp][position];
         tempname[1] = '\0';
         optionType = getType(tempname);
         if (optionType != OPTION_BOOLEAN_TYPE) {
            position++;
         }
         break;
      case OPTION_FORM_LONG:   
         position = 2;
         while (gargv[gargp][position] != '=' && 
               gargv[gargp][position] != '\0') {
            tempname[position-2] = gargv[gargp][position];
            position++;    
         }
         tempname[position-2] = '\0';
         optionType = getType(tempname);
         if (optionType == -1) {         // suppressed --options option
            optionsArgument = 1;
            break;   
         }
         if (gargv[gargp][position] == '=') {
            if (optionType == OPTION_BOOLEAN_TYPE) {
               cout << "Error: boolean variable cannot have any options: " 
                    << tempname << endl;
               exit(1);
            }
            position++;
         }
         break;
   }

   if (optionType == -1) {              // suppressed --options option
      optionsArgument = 1;
      gargp++;
      position = 0;
      return gargp;
   }

   if (gargv[gargp][position] == '\0' && 
         optionType != OPTION_BOOLEAN_TYPE) {
      gargp++;
      position = 0;
   } 

   if (optionForm != OPTION_FORM_LONG && optionType == OPTION_BOOLEAN_TYPE &&
         gargv[gargp][position+1] != '\0') {
      running = 1;
   } else if (optionType == OPTION_BOOLEAN_TYPE &&
         gargv[gargp][position+1] == '\0') {
      running = 0;
   }

   if (gargp >= gargc) {
      cout << "Error: last option requires a parameter" << endl;
      exit(1);
   }
   setModified(tempname, &gargv[gargp][position]);

   if (!running) {
      gargp++;
   }
   return gargp;
} 


///////////////////////////////////////////////////////////////////////////
//
// helping function
//

//////////////////////////////
//
// optionListCompare -- for sorting the option list
//

int optionListCompare(const void* a, const void* b) {
//cerr << "       comparing: " << (*((option_list**)a))->getName() 
//      << " i=" << (*((option_list**)a))->getIndex() 
//      << " :to: "
//      << (*((option_list**)b))->getName() 
//      << " i=" << (*((option_list**)b))->getIndex() << endl;
   return strcmp((*((option_list**)a))->getName(), 
                 (*((option_list**)b))->getName());
}



// md5sum: 5a09b3a7a4a44da97fcbbd1b4f2c3464 Options.cpp [20050403]
