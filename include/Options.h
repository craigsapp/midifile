//
// Copyright 1998-2018 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Mon Jan 18 18:25:23 PST 2021 Some cleanup
// Filename:      midifile/include/Options.h
// Web Address:   http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 noexpandtab
//
// Description:   Interface for command-line options.
//

#ifndef _OPTIONS_H_INCLUDED
#define _OPTIONS_H_INCLUDED

#include <iostream>
#include <map>
#include <string>
#include <vector>

#define OPTION_TYPE_unknown   '\0'
#define OPTION_TYPE_boolean   'b'
#define OPTION_TYPE_char      'c'
#define OPTION_TYPE_double    'd'
#define OPTION_TYPE_float     'f'
#define OPTION_TYPE_int       'i'
#define OPTION_TYPE_string    's'

namespace smf {

class Option_register {
	public:
		                  Option_register    (void);
		                  Option_register    (const std::string& aDefinition,
		                                      char aType,
		                                      const std::string& aDefaultOption);
		                  Option_register    (const std::string& aDefinition,
		                                      char aType,
		                                      const std::string& aDefaultOption,
		                                      const std::string& aModifiedOption);

		                 ~Option_register    ();

	  void               clearModified      (void);
	  const std::string& getDefinition      (void);
	  const std::string& getDefault         (void);
	  const std::string& getOption          (void);
	  const std::string& getModified        (void);
	  const std::string& getDescription     (void);
	  bool               isModified         (void);
	  char               getType            (void);
	  void               reset              (void);
	  void               setDefault         (const std::string& aString);
	  void               setDefinition      (const std::string& aString);
	  void               setDescription     (const std::string& aString);
	  void               setModified        (const std::string& aString);
	  void               setType            (char aType);
	  std::ostream&      print              (std::ostream& out);

	protected:
		std::string       m_definition;
		std::string       m_description;
		std::string       m_defaultOption;
		std::string       m_modifiedOption;
		bool              m_modifiedQ;
		char              m_type;

};



class Options {
	public:
		                   Options           (void);
		                   Options           (int argc, char** argv);

		                  ~Options           ();

		int                argc              (void) const;
		const std::vector<std::string>& argv (void) const;
		int                define            (const std::string& aDefinition);
		int                define            (const std::string& aDefinition,
		                                      const std::string& description);
		const std::string& getArg            (int index);
		const std::string& getArgument       (int index);
		int                getArgCount       (void);
		int                getArgumentCount  (void);
		const std::vector<std::string>& getArgList      (void);
		const std::vector<std::string>& getArgumentList (void);
		bool               getBoolean        (const std::string& optionName);
		std::string        getCommand        (void);
		const std::string& getCommandLine    (void);
		std::string        getDefinition     (const std::string& optionName);
		double             getDouble         (const std::string& optionName);
		char               getFlag           (void);
		char               getChar           (const std::string& optionName);
		float              getFloat          (const std::string& optionName);
		int                getInt            (const std::string& optionName);
		int                getInteger        (const std::string& optionName);
		std::string        getString         (const std::string& optionName);
		char               getType           (const std::string& optionName);
		int                optionsArg        (void);
		std::ostream&      print             (std::ostream& out);
		std::ostream&      printOptionList   (std::ostream& out);
		std::ostream&      printOptionListBooleanState(std::ostream& out);
		void               process           (int error_check = 1, int suppress = 0);
		void               process           (int argc, char** argv, int error_check = 1,
		                                      int suppress = 0);
		void               reset             (void);
		void               xverify           (int argc, char** argv,
		                                      int error_check = 1,
		                                      int suppress = 0);
		void               xverify           (int error_check = 1, int suppress = 0);
		void               setFlag           (char aFlag);
		void               setModified       (const std::string& optionName,
		                                      const std::string& optionValue);
		void               setOptions        (int argc, char** argv);
		void               appendOptions     (int argc, char** argv);
		void               appendOptions     (const std::string& strang);
		void               appendOptions     (const std::vector<std::string>& argv);
		std::ostream&      printRegister     (std::ostream& out);
		bool               isDefined         (const std::string& name);

	protected:
		int                           m_options_error_check;  // verify command
		int                           m_oargc;
		std::vector<std::string>      m_oargv;
		std::string                   m_commandString;
		char                          m_optionFlag;
		std::vector<std::string>      m_argument;

		std::vector<Option_register*> m_optionRegister;
		std::map<std::string, int>    m_optionList;

		bool                          m_processedQ;
		bool                          m_suppressQ;       // prevent --options
		bool                          m_optionsArgument; // --options present

		std::vector<std::string>      m_extraArgv;
		std::vector<std::string>      m_extraArgv_strings;

	private:
		int                getRegIndex       (const std::string& optionName);
		int                optionQ           (const std::string& aString, int& argp);
		int                storeOption       (int gargp, int& position, int& running);

};

} // end of namespace smf


#endif  /* _OPTIONS_H_INCLUDED */



