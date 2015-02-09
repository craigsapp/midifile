//
// Copyright 1998-2000 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Fri Jan 15 07:24:00 PST 1999
// Last Modified: Sat Mar 27 18:17:59 PST 1999
// Last Modified: Thu Apr 13 14:02:52 PDT 2000 (added 2nd define function)
// Last Modified: Fri May  5 17:57:50 PDT 2000 (added --options suppression)
// Last Modified: Mon Oct 20 07:56:41 PDT 2008 (allow for secondary user input)
// Last Modified: Sat Jun 13 21:16:29 PDT 2009 (verify --> xverify for OSX)
// Filename:      ...sig/maint/code/base/Options/Options.h
// Web Address:   http://sig.sapp.org/include/sigBase/Options.h
// Documentation: http://sig.sapp.org/doc/classes/Options
// Syntax:        C++ 
//
// Description:   Handles command-line options in a graceful manner.
//

#ifndef _OPTIONS_H_INCLUDED
#define _OPTIONS_H_INCLUDED

#include "Array.h"

class option_list;
class option_register;


class Options {
   public:
                    Options             (void);
                    Options             (int argc, char** argv);
                   ~Options             ();

      int           argc                (void) const;
      char**        argv                (void) const;
      void          define              (const char* aDefinition);
      void          define              (const char* aDefinition, 
                                           const char* description);
      char*         getArg              (int index);
      char*         getArgument         (int index);
      int           getArgCount         (void);
      int           getArgumentCount    (void);
      char**        getArgList          (void);
      char**        getArgumentList     (void);
      int           getBoolean          (const char* optionName);
      const char*   getCommand          (void);
      const char*   getCommandLine      (void);
      const char*   getString           (void);
      const char*   getDefinition       (const char* optionName);
      double        getDouble           (const char* optionName);
      char          getFlag             (void);
      float         getFloat            (const char* optionName);
      int           getInt              (const char* optionName);
      int           getInteger          (const char* optionName);
      const char*   getString           (const char* optionName);
      char          getType             (const char* optionName);
      int           optionsArg          (void);
      void          print               (void); 
      void          process             (int error_check = 1, int suppress = 0);
      void          process             (int argc, char** argv,
                                              int error_check = 1,
                                              int suppress = 0);
      void          reset               (void);
      void          xverify             (int argc, char** argv, 
                                              int error_check = 1,
                                              int suppress = 0);
      void          xverify             (int error_check = 1,
                                              int suppress = 0);
      void          setFlag             (char aFlag);
      void          setModified         (const char* optionName, 
                                           const char* optionValue);
      void          setOptions          (int argc, char** argv);
      void          appendOptions       (int argc, char** argv);
      void          appendOptions       (const char* strang);

   protected:
      int                      options_error_check;  // for verify command
      int                      gargc;
      char**                   gargv;
      char*                    commandString;
      char                     optionFlag;
      Array<char*>             argument;
      Array<option_register*>  optionRegister;
      Array<option_list*>      optionList;
      int                      processedQ;
      int                      sortedQ;
      int                      suppressQ;       // prevent the --options option
      int                      optionsArgument; // indicates --options present

      Array<char*>             extraArgv;
      Array<Array<char> >      extraArgv_strings;

      int         getRegIndex                (const char* optionName);
      int         optionQ                    (const char* aString, int& argp);
      void        sortOptionNames            (void);
      int         storeOption                (int gargp, int& position, 
                                                 int& running);

};

#define OPTION_BOOLEAN_TYPE   'b'
#define OPTION_CHAR_TYPE      'c'
#define OPTION_DOUBLE_TYPE    'd'
#define OPTION_FLOAT_TYPE     'f'
#define OPTION_INT_TYPE       'i'
#define OPTION_STRING_TYPE    's'
#define OPTION_UNKNOWN_TYPE   'x'



#endif  /* _OPTIONS_H_INCLUDED */



// md5sum: c59d297a8081cb48f61b534484819f48 Options.h [20030102]
