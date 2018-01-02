//
// Copyright 1998-2000 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Sat Mar  1 09:27:49 PST 2014 Implemented with STL.
// Filename:      Options.h
// Web Address:   https://github.com/craigsapp/optionlib/blob/master/include/Options.h
// Documentation: http://sig.sapp.org/doc/classes/Options
// Syntax:        C++11
//
// Description:   Interface to command-line options.
//

#ifndef _OPTIONS_H_INCLUDED
#define _OPTIONS_H_INCLUDED

#include <vector>
#include <map>
#include <string>

using namespace std;

class Option_register {
   public:
                   Option_register    (void);
                   Option_register    (const std::string& aDefinition, char aType,
                                       const std::string& aDefaultOption);
                   Option_register    (const std::string& aDefinition, char aType,
                                       const std::string& aDefaultOption,
                                       const std::string& aModifiedOption);
                  ~Option_register    ();

     void          clearModified      (void);
     const std::string& getDefinition      (void);
     const std::string& getDefault         (void);
     const std::string& getOption          (void);
     const std::string& getModified        (void);
     const std::string& getDescription     (void);
     int           isModified         (void);
     char          getType            (void);
     void          reset              (void);
     void          setDefault         (const std::string& aString);
     void          setDefinition      (const std::string& aString);
     void          setDescription     (const std::string& aString);
     void          setModified        (const std::string& aString);
     void          setType            (char aType);
     std::ostream& print              (std::ostream& out);

   protected:
      std::string  definition;
      std::string  description;
      std::string  defaultOption;
      std::string  modifiedOption;
      int          modifiedQ;
      char         type;

};



class Options {
   public:
                      Options           (void);
                      Options           (int argc, char** argv);
                     ~Options           ();

      int             argc              (void) const;
      const std::vector<std::string>& argv        (void) const;
      int             define            (const std::string& aDefinition);
      int             define            (const std::string& aDefinition,
                                         const std::string& description);
      const std::string&   getArg            (int index);
      const std::string&   getArgument       (int index);
      int             getArgCount       (void);
      int             getArgumentCount  (void);
      const std::vector<std::string>& getArgList  (void);
      const std::vector<std::string>& getArgumentList (void);
      int             getBoolean        (const std::string& optionName);
      std::string     getCommand        (void);
      const std::string&   getCommandLine    (void);
      std::string     getDefinition     (const std::string& optionName);
      double          getDouble         (const std::string& optionName);
      char            getFlag           (void);
      char            getChar           (const std::string& optionName);
      float           getFloat          (const std::string& optionName);
      int             getInt            (const std::string& optionName);
      int             getInteger        (const std::string& optionName);
      std::string     getString         (const std::string& optionName);
      char            getType           (const std::string& optionName);
      int             optionsArg        (void);
      std::ostream&   print             (std::ostream& out);
      std::ostream&   printOptionList   (std::ostream& out);
      std::ostream&   printOptionListBooleanState(std::ostream& out);
      void            process           (int error_check = 1, int suppress = 0);
      void            process           (int argc, char** argv,
                                            int error_check = 1,
                                            int suppress = 0);
      void            reset             (void);
      void            xverify           (int argc, char** argv,
                                            int error_check = 1,
                                            int suppress = 0);
      void            xverify           (int error_check = 1,
                                            int suppress = 0);
      void            setFlag           (char aFlag);
      void            setModified       (const std::string& optionName,
                                         const std::string& optionValue);
      void            setOptions        (int argc, char** argv);
      void            appendOptions     (int argc, char** argv);
      void            appendOptions     (const std::string& strang);
      void            appendOptions     (const std::vector<std::string>& argv);
      std::ostream&   printRegister     (std::ostream& out);
      int             isDefined         (const std::string& name);

   protected:
      int                      options_error_check;  // for verify command
      int                      oargc;
      std::vector<std::string> oargv;
      std::string              commandString;
      char                     optionFlag;
      std::vector<std::string> argument;

      std::vector<Option_register*> optionRegister;
      map<std::string, int>         optionList;

      int                      processedQ;
      int                      suppressQ;       // prevent the --options option
      int                      optionsArgument; // indicates --options present

      std::vector<std::string> extraArgv;
      std::vector<std::string> extraArgv_strings;

      int         getRegIndex             (const std::string& optionName);
      int         optionQ                 (const std::string& aString, int& argp);
      int         storeOption             (int gargp, int& position,
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

