//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sun Apr  5 13:07:18 PDT 1998
// Last Modified: Mon Jan 18 18:25:23 PST 2021 Some cleanup
// Filename:      midifile/src/Options.cpp
// Web Address:   http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 noexpandtab
//
// Description:   Interface to command-line options.
//

#include "Options.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>


namespace smf {

///////////////////////////////////////////////////////////////////////////
//
// Option_register class function definitions.
//


//////////////////////////////
//
// Option_register::Option_register -- Constructor.
//

Option_register::Option_register(void) {
	setType(OPTION_TYPE_string);
	m_modifiedQ = false;
}


Option_register::Option_register(const std::string& aDefinition, char aType,
		const std::string& aDefaultOption) {
	m_modifiedQ = false;
	setType(aType);
	setDefinition(aDefinition);
	setDefault(aDefaultOption);
}

Option_register::Option_register(const std::string& aDefinition, char aType,
		const std::string& aDefaultOption, const std::string& aModifiedOption) {
	setType(aType);
	setDefinition(aDefinition);
	setDefault(aDefaultOption);
	setModified(aModifiedOption);
}



//////////////////////////////
//
// Option_register::~Option_register -- Destructor.
//

Option_register::~Option_register() {
	// do nothing
}



//////////////////////////////
//
// Option_register::clearModified -- Clear any changes in the option value.
//

void Option_register::clearModified(void) {
	m_modifiedOption.clear();
	m_modifiedQ = false;
}



//////////////////////////////
//
// Option_register::getDefinition -- Returns the initial definition.
//	string used to define this entry.
//

const std::string& Option_register::getDefinition(void) {
	return m_definition;
}



//////////////////////////////
//
// Option_register::getDescription -- Return the textual description
//      of the entry.
//

const std::string& Option_register::getDescription(void) {
	return m_description;
}



//////////////////////////////
//
// Option_register::getDefault --  Return the default value string.
//

const std::string& Option_register::getDefault(void) {
	return m_defaultOption;
}



//////////////////////////////
//
// Option_register::getModified -- Return the modified option string.
//

const std::string& Option_register::getModified(void) {
	return m_modifiedOption;
}



//////////////////////////////
//
// Option_register::isModified -- Return true if option has been
//    set on the command-line.
//

bool Option_register::isModified(void) {
	return m_modifiedQ;
}



//////////////////////////////
//
// Option_register::getType -- Return the data type of the option.
//

char Option_register::getType(void) {
	return m_type;
}



//////////////////////////////
//
// Option_register::getOption -- return the modified option
//  	or the default option if no modified option.
//

const std::string& Option_register::getOption(void) {
	if (isModified()) {
		return getModified();
	} else {
		return getDefault();
	}
}



//////////////////////////////
//
// Option_register::reset -- deallocate space for all
//	strings in object.  (but default string is set to "")
//

void Option_register::reset(void) {
	m_type = OPTION_TYPE_string;
	m_definition.clear();
	m_defaultOption.clear();
	m_modifiedOption.clear();
}



//////////////////////////////
//
// Option_register::setDefault -- Set the default value.
//

void Option_register::setDefault(const std::string& aString) {
	m_defaultOption = aString;
}



//////////////////////////////
//
// Option_register::setDefinition -- Set the option definition.
//

void Option_register::setDefinition(const std::string& aString) {
	m_definition = aString;
}



//////////////////////////////
//
// Option_register::setDescription -- Set the textual description.
//

void Option_register::setDescription(const std::string& aString) {
	m_description = aString;
}



//////////////////////////////
//
// Option_register::setModified -- Set the modified value.
//

void Option_register::setModified(const std::string& aString) {
	m_modifiedOption = aString;
	m_modifiedQ = true;
}



//////////////////////////////
//
// Option_register::setType -- Set the option type.
//

void Option_register::setType(char aType) {
	m_type = aType;
}



//////////////////////////////
//
// Option_register::print -- Print the state of the option register entry.
//     Useful for debugging.
//

std::ostream& Option_register::print(std::ostream& out) {
	out << "definition:\t"     << m_definition     << std::endl;
	out << "description:\t"    << m_description    << std::endl;
	out << "defaultOption:\t"  << m_defaultOption  << std::endl;
	out << "modifiedOption:\t" << m_modifiedOption << std::endl;
	out << "modifiedQ:\t\t"    << m_modifiedQ      << std::endl;
	out << "type:\t\t"         << m_type           << std::endl;
	return out;
};




///////////////////////////////////////////////////////////////////////////
//
// Options class function definitions.
//

//////////////////////////////
//
// Options::Options -- Constructor.
//

Options::Options(void) {
	m_oargc               = -1;
	m_suppressQ           =  0;
	m_processedQ          =  0;
	m_optionsArgument     =  0;
	m_options_error_check =  1;
	m_optionFlag          = '-';

	m_extraArgv.reserve(100);
	m_extraArgv_strings.reserve(100);
}


Options::Options(int argc, char** argv) {
	m_oargc               = -1;
	m_suppressQ           =  0;
	m_processedQ          =  0;
	m_optionsArgument     =  0;
	m_options_error_check =  1;
	m_optionFlag          = '-';

	m_extraArgv.reserve(100);
	m_extraArgv_strings.reserve(100);

	setOptions(argc, argv);
}



//////////////////////////////
//
// Options::~Options -- Destructor.
//

Options::~Options() {
	reset();
}



//////////////////////////////
//
// Options::argc -- returns the argument count as input from main().
//

int Options::argc(void) const {
	return m_oargc;
}



//////////////////////////////
//
// Options::argv -- returns the arguments strings as input from main().
//

const std::vector<std::string>& Options::argv(void) const {
	return m_oargv;
}



//////////////////////////////
//
// Options::define -- store an option definition in the register.  Option
//     definitions have this structure:
//        option-name|alias-name1|alias-name2=option-type:option-default
// option-name :: name of the option (one or more character, not including
//      spaces or equal signs.
// alias-name  :: equivalent name(s) of the option.
// option-type :: single charater indicating the option data type.
// option-default :: default value for option if no given on the command-line.
//

int Options::define(const std::string& aDefinition) {
	Option_register* definitionEntry = NULL;

	// Error if definition string doesn't contain an equals sign
	auto location = aDefinition.find("=");
	if (location == std::string::npos) {
		std::cerr << "Error: no \"=\" in option definition: " << aDefinition << std::endl;
		exit(1);
	}

	std::string aliases = aDefinition.substr(0, location);
	std::string rest    = aDefinition.substr(location+1);
	std::string otype   = rest;
	std::string ovalue  = "";

	location = rest.find(":");
	if (location != std::string::npos) {
		otype  = rest.substr(0, location);
		ovalue = rest.substr(location+1);
	}

	// Remove any spaces in the option type field
	otype.erase(remove_if(otype.begin(), otype.end(), ::isspace), otype.end());

	// Option types are only a single charater (b, i, d, c or s)
	if (otype.size() != 1) {
		std::cerr << "Error: option type is invalid: " << otype
			  << " in option definition: " << aDefinition << std::endl;
		exit(1);
	}

	// Check to make sure that the type is known
	if (otype[0] != OPTION_TYPE_string  &&
		 otype[0] != OPTION_TYPE_int     &&
		 otype[0] != OPTION_TYPE_float   &&
		 otype[0] != OPTION_TYPE_double  &&
		 otype[0] != OPTION_TYPE_boolean &&
		 otype[0] != OPTION_TYPE_char ) {
		std::cerr << "Error: unknown option type \'" << otype[0]
			  << "\' in defintion: " << aDefinition << std::endl;
		exit(1);
	}

	// Set up space for a option entry in the register
	definitionEntry = new Option_register(aDefinition, otype[0], ovalue);

	int definitionIndex = (int)m_optionRegister.size();

	// Store option aliases
	std::string optionName;
	unsigned int i;
	aliases += '|';
	for (i=0; i<aliases.size(); i++) {
		if (::isspace(aliases[i])) {
			continue;
		} else if (aliases[i] == '|') {
			if (isDefined(optionName)) {
				std::cerr << "Option \"" << optionName << "\" from definition:" << std::endl;
				std::cerr << "\t" << aDefinition << std::endl;
				std::cerr << "is already defined in: " << std::endl;
				std::cerr << "\t" << getDefinition(optionName) << std::endl;
				exit(1);
			}
			if (optionName.size() > 0) {
				m_optionList[optionName] = definitionIndex;
			}
			optionName.clear();
		} else {
			optionName += aliases[i];
		}
	}

	// Store definition in register and return its indexed location.
	// This location will be used to link option aliases to the main
	// command name.
	m_optionRegister.push_back(definitionEntry);
	return definitionIndex;
}


int Options::define(const std::string& aDefinition,
		const std::string& aDescription) {
	int index = define(aDefinition);
	m_optionRegister[index]->setDescription(aDescription);
	return index;
}



//////////////////////////////
//
// Options::isDefined -- Return true if option is present in register.
//

bool Options::isDefined(const std::string& name) {
	return m_optionList.find(name) == m_optionList.end() ? false : true;
}



//////////////////////////////
//
// Options::getArg -- returns the specified argument.
//	argument 0 is the command name.
//

const std::string& Options::getArg(int index) {
	if (index < 0 || index >= (int)m_argument.size()) {
		std::cerr << "Error: m_argument " << index << " does not exist." << std::endl;
		exit(1);
	}
	return m_argument[index];
}

// Alias:

const std::string& Options::getArgument(int index) {
	return getArg(index);
}



//////////////////////////////
//
// Options::getArgCount --  number of arguments on command line.
//	does not count the options or the command name.
//

int Options::getArgCount(void) {
	return ((int)m_argument.size()) - 1;
}

// Alias:

int Options::getArgumentCount(void) {
	return getArgCount();
}



//////////////////////////////
//
// Options::getArgList -- return a string vector of the arguments
//     after the options have been parsed out of it.
//

const std::vector<std::string>& Options::getArgList(void) {
	return m_argument;
}

// Alias:

const std::vector<std::string>& Options::getArgumentList(void) {
	return getArgList();
}



//////////////////////////////
//
// Options::getBoolean --  returns true if the option was
//	used on the command line.
//

bool Options::getBoolean(const std::string& optionName) {
	int index = getRegIndex(optionName);
	if (index < 0) {
		return 0;
	}
	return m_optionRegister[index]->isModified();
}



//////////////////////////////
//
// Options::getCommand -- returns argv[0] (the first string
//     in the original argv list.
//

std::string Options::getCommand(void) {
	if (m_argument.size() == 0) {
		return "";
	} else {
		return m_argument[0];
	}
}



//////////////////////////////
//
// Options::getCommandLine -- returns a string which contains the
//     command-line call to the program.  Deal with spaces in arguments...
//

const std::string& Options::getCommandLine(void) {
	if (m_commandString.size()) {
		return m_commandString;
	}

	m_commandString = m_oargv[0];

	int i;
	for (i=1; i<m_oargc; i++) {
		m_commandString += " ";
		m_commandString += m_oargv[i];
	}

	return m_commandString;
}




//////////////////////////////
//
// Options::getDefinition -- returns the definition for the specified
//      option name.  Returns empty string if there is no entry for
//      the option name.  spaces count in the input option name.
//

std::string Options::getDefinition(const std::string& optionName) {
	auto it = m_optionList.find(optionName);
	if (it == m_optionList.end()) {
		return "";
	} else {
		return m_optionRegister[it->second]->getDefinition();
	}
}



//////////////////////////////
//
// Options::getDouble -- returns the double float associated
//	with the given option.  Returns 0 if there is no
//	number associated with the option.
//

double Options::getDouble(const std::string& optionName) {
	return strtod(getString(optionName).c_str(), (char**)NULL);
}



//////////////////////////////
//
// Options::getChar -- Return the first character in the option string;
//      If the length is zero, then return '\0'.
//

char Options::getChar(const std::string& optionName) {
	return getString(optionName).c_str()[0];
}



//////////////////////////////
//
// Options::getFloat -- Return the floating point number
//	associated with the given option.
//

float Options::getFloat(const std::string& optionName) {
	return (float)getDouble(optionName);
}



//////////////////////////////
//
// Options::getInt -- Return the integer argument.  Can handle
//	hexadecimal, decimal, and octal written in standard
//	C syntax.
//

int Options::getInt(const std::string& optionName) {
	return (int)strtol(getString(optionName).c_str(), (char**)NULL, 0);
}

int Options::getInteger(const std::string& optionName) {
	return getInt(optionName);
}



//////////////////////////////
//
// Options::getString -- Return the option argument string.
//

std::string Options::getString(const std::string& optionName) {
	int index = getRegIndex(optionName);
	if (index < 0) {
		return "UNKNOWN OPTION";
	} else {
		return m_optionRegister[index]->getOption();
	}
}



//////////////////////////////
//
// Options::optionsArg -- Return true if --options is present
//    on the command line, otherwise returns false.
//

int Options::optionsArg(void) {
	return m_optionsArgument;
}



//////////////////////////////
//
// Options::print -- Print a list of the defined options.
//

std::ostream& Options::print(std::ostream& out) {
	for (auto &item : m_optionRegister) {
		out << item->getDefinition() << "\t"
			  << item->getDescription() << std::endl;
	}
	return out;
}



//////////////////////////////
//
// Options::reset -- Clear all defined options.
//

void Options::reset(void) {
	unsigned int i;
	for (i=0; i<m_optionRegister.size(); i++) {
		delete m_optionRegister[i];
		m_optionRegister[i] = NULL;
	}
	m_optionRegister.clear();

	m_argument.clear();
	m_commandString.clear();
	m_extraArgv.clear();
	m_extraArgv_strings.clear();

	m_oargc = -1;
	m_oargv.clear();
}



//////////////////////////////
//
// Options::getFlag -- Set the character which is usually set to a dash.
//

char Options::getFlag(void) {
	return m_optionFlag;
}



//////////////////////////////
//
// Options::setFlag -- Set the character used to indicate an
//	option.  For unix this is usually '-', in MS-DOS,
//	this is usually '/';  But the syntax of the Options
//	class is for Unix-style options.
//

void Options::setFlag(char aFlag) {
	m_optionFlag = aFlag;
}



//////////////////////////////
//
// Options::setModified --
//

void Options::setModified(const std::string& optionName,
		const std::string& aString) {
	int index = getRegIndex(optionName);
	if (index < 0) {
		return;
	}

	m_optionRegister[getRegIndex(optionName)]->setModified(aString);
}




//////////////////////////////
//
// Options::setOptions --  Store the input list of options.
//

void Options::setOptions(int argc, char** argv) {
	m_processedQ = 0;

	m_extraArgv.resize(argc);
	m_extraArgv_strings.resize(argc);
	int oldsize = 0;

	int i;
	for (i=0; i<argc; i++) {
		m_extraArgv_strings[i+oldsize] = argv[i];
		m_extraArgv[i] = m_extraArgv_strings[i];
	}

	m_oargc  = (int)m_extraArgv.size();
	m_oargv  = m_extraArgv;
}



//////////////////////////////
//
// Options::appendOptions -- Add argc and argv data to the current
//      list residing inside the Options class variable.
//

void Options::appendOptions(int argc, char** argv) {
	m_processedQ = 0;

	// data used to be stored directly here:
	//gargc = argc;
	//gargv = argv;
	// but now gets interfaced to: m_extraArgv and m_extraArgv_strings:

	int oldsize = (int)m_extraArgv.size();
	m_extraArgv.resize(oldsize + argc);
	m_extraArgv_strings.resize(oldsize + argc);

	int i;
	for (i=0; i<argc; i++) {
		m_extraArgv_strings[i+oldsize] = argv[i];
		m_extraArgv[i+oldsize] = m_extraArgv_strings[i+oldsize];
	}

	m_oargc = (int)m_extraArgv.size();
	m_oargv = m_extraArgv;
}


void Options::appendOptions(const std::vector<std::string>& argv) {
	m_processedQ = 0;

	int oldsize = (int)m_extraArgv.size();
	m_extraArgv.resize(oldsize + argv.size());
	m_extraArgv_strings.resize(oldsize + argv.size());

	unsigned int i;
	for (i=0; i<argv.size(); i++) {
		m_extraArgv_strings[i+oldsize] = argv[i];
		m_extraArgv[i+oldsize] = m_extraArgv_strings[i+oldsize];
	}

	m_oargc = (int)m_extraArgv.size();
	m_oargv = m_extraArgv;
}



//////////////////////////////
//
// Options::appendOptions -- parse the string like command-line arguments.
//   Either double or single quotes can be used to encapsulate
//   a command-line token.  If double quotes are used to encapsulate,
//   then you will not have to back quote single quotes inside the
//   token string, but you will have to backslash double quotes:
//      "-T \"\"" but "-T ''"
//   Likewise for single quotes in reverse with double quotes:
//      '-T \'\'' is equal to: '-T ""'
//

void Options::appendOptions(const std::string& strang) {
	int doublequote = 0;
	int singlequote = 0;

	std::vector<std::string> tokens;
	std::vector<std::string> tempargv;
	std::string tempvalue;

	tokens.reserve(100);
	tempargv.reserve(100);
	tempvalue.reserve(1000);

	char ch = '\0';

	int length = (int)strang.size();
	for (int i=0; i<length; i++) {

		if (!singlequote && (strang[i] == '"')) {
			if ((i>0) && (strang[i-1] != '\\')) {
				doublequote = !doublequote;
				if (doublequote == 0) {
					// finished a doublequote section of data, so store
					// even if it is the empty string
					ch = '\0';
					tempvalue += (ch);
					tokens.push_back(tempvalue);
					tempvalue.clear();
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
					tempvalue += ch;
					tokens.push_back(tempvalue);
					tempvalue.clear();
					continue;
				} else {
					// don't store the leading ":
					continue;
				}
			}
		}


		if ((!doublequote && !singlequote) && std::isspace(strang[i])) {
			if (tempvalue.size() > 0) {
				tempvalue += ch;
				tokens.push_back(tempvalue);
				tempvalue.clear();
			}
		} else {
			ch = strang[i];
			tempvalue += ch;
		}
	}
	if (tempvalue.size() > 0) {
		tokens.push_back(tempvalue);
		tempvalue.clear();
	}

	// now that the input string has been chopped up into pieces,
	// assemble the argv structure

	tempargv.reserve(tokens.size());
	for (int i=0; i<(int)tempargv.size(); i++) {
		tempargv[i] = tokens[i];
	}

	// now store the argv and argc data in opts:

	// now store the parsed command-line simulated tokens
	// for actual storage:
	appendOptions(tempargv);
}



//////////////////////////////
//
// Options:getType -- Return the type of the option.
//

char Options::getType(const std::string& optionName) {
	int index = getRegIndex(optionName);
	if (index < 0) {
		return -1;
	} else {
		return m_optionRegister[getRegIndex(optionName)]->getType();
	}
}



//////////////////////////////
//
// Options::process -- Same as xverify.
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
	m_options_error_check = error_check;
	int gargp = 1;
	int optionend = 0;

	if (suppress) {
		m_suppressQ = 1;
	} else {
		m_suppressQ = 0;
	}

	// if calling xverify again, must remove previous argument list.
	if (m_argument.size() != 0) {
		m_argument.clear();
	}

	m_argument.push_back(m_oargv[0]);
	int oldgargp;
	int position = 0;
	int running = 0;
	while (gargp < m_oargc && optionend == 0) {
		if (optionQ(m_oargv[gargp], gargp)) {
			oldgargp = gargp;
			gargp = storeOption(gargp, position, running);
			if (gargp != oldgargp) {
				running = 0;
				position = 0;
			}
		} else {
			if (m_oargv[gargp].size() == 2 && m_oargv[gargp][0] == getFlag() &&
				m_oargv[gargp][2] == getFlag() ) {
					optionend = 1;
				gargp++;
				break;
			} else {                          // this is an argument
				m_argument.push_back(m_oargv[gargp]);
				gargp++;
			}
		}
	}

	while (gargp < m_oargc) {
		m_argument.push_back(m_oargv[gargp]);
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
// Options::getRegIndex -- returns the index of the option associated
//	with this name.
//

int Options::getRegIndex(const std::string& optionName) {
	if (m_suppressQ && (optionName == "options")) {
			return -1;
	}

	if (optionName == "options") {
		print(std::cout);
		exit(0);
	}


	auto it = m_optionList.find(optionName);
	if (it == m_optionList.end()) {
		if (m_options_error_check) {
			std::cerr << "Error: unknown option \"" << optionName << "\"." << std::endl;
			print(std::cout);
			exit(1);
		} else {
			return -1;
		}
	} else {
		return it->second;
	}
}



//////////////////////////////
//
// Options::optionQ --  returns true if the string is an option
//	"--" is not an option, also '-' is not an option.
//	aString is assumed to not be NULL.
//

int Options::optionQ(const std::string& aString, int& argp) {
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
// Options::storeOption --
//

#define OPTION_FORM_short     0
#define OPTION_FORM_long      1
#define OPTION_FORM_continue  2

int Options::storeOption(int gargp, int& position, int& running) {
	int optionForm;
	char tempname[4096];
	char optionType = OPTION_TYPE_unknown;

	if (running) {
		optionForm = OPTION_FORM_continue;
	} else if (m_oargv[gargp][1] == getFlag()) {
		optionForm = OPTION_FORM_long;
	} else {
		optionForm = OPTION_FORM_short;
	}

	switch (optionForm) {
		case OPTION_FORM_continue:
			position++;
			tempname[0] = m_oargv[gargp][position];
			tempname[1] = '\0';
			optionType = getType(tempname);
			if (optionType != OPTION_TYPE_boolean) {
				running = 0;
				position++;
			}
			break;
		case OPTION_FORM_short:
			position = 1;
			tempname[0] = m_oargv[gargp][position];
			tempname[1] = '\0';
			optionType = getType(tempname);
			if (optionType != OPTION_TYPE_boolean) {
				position++;
			}
			break;
		case OPTION_FORM_long:
			position = 2;
			while (m_oargv[gargp][position] != '=' &&
					m_oargv[gargp][position] != '\0') {
				tempname[position-2] = m_oargv[gargp][position];
				position++;
			}
			tempname[position-2] = '\0';
			optionType = getType(tempname);
			if (optionType == OPTION_TYPE_unknown) { // suppressed --options option
				m_optionsArgument = 1;
				break;
			}
			if (m_oargv[gargp][position] == '=') {
				if (optionType == OPTION_TYPE_boolean) {
					std::cerr << "Error: boolean variable cannot have any options: "
					     << tempname << std::endl;
					exit(1);
				}
				position++;
			}
			break;
	}

	if (optionType == OPTION_TYPE_unknown) { // suppressed --options option
		m_optionsArgument = 1;
		gargp++;
		position = 0;
		return gargp;
	}

	if (m_oargv[gargp][position] == '\0' &&
			optionType != OPTION_TYPE_boolean) {
		gargp++;
		position = 0;
	}

	if ((optionForm != OPTION_FORM_long) && (optionType == OPTION_TYPE_boolean) &&
			(m_oargv[gargp][position+1] != '\0')) {
		running = 1;
	} else if ((optionType == OPTION_TYPE_boolean) &&
			(m_oargv[gargp][position+1] == '\0')) {
		running = 0;
	}

	if (gargp >= m_oargc) {
		std::cerr << "Error: last option requires a parameter" << std::endl;
		exit(1);
	}
	setModified(tempname, &m_oargv[gargp][position]);

	if (!running) {
		gargp++;
	}
	return gargp;
}



//////////////////////////////
//
// Options::printOptionList --
//

std::ostream& Options::printOptionList(std::ostream& out) {
	for (auto &item : m_optionList) {
		out << item.first << "\t" << item.second << std::endl;
	}
	return out;
}



//////////////////////////////
//
// Options::printOptionBooleanState --
//

std::ostream& Options::printOptionListBooleanState(std::ostream& out) {
	for (auto &item : m_optionList) {
		out << item.first << "\t"
			 << m_optionRegister[item.second]->isModified() << std::endl;
	}
	return out;
}



//////////////////////////////
//
// Options::printRegister --
//

std::ostream& Options::printRegister(std::ostream& out) {
	for (auto &item : m_optionRegister) {
		item->print(out);
	}
	return out;
}


} // end namespace smf



