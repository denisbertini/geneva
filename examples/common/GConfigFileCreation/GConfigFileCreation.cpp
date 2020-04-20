/**
 * @file GConfigFileCreation.cpp
 */

/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

// Standard header files go here
#include <iostream>
#include <string>
#include <array>

// Boost header files go here
#include <boost/lexical_cast.hpp>

// Geneva header files go here
#include "common/GParserBuilder.hpp"

using namespace Gem::Common;

// Call-back function for a single parameter
int someGlobalInt;
const int SOMEGLOBALINTDEFAULT = 1;
void setGlobalInt(int globalInt) {
	someGlobalInt = globalInt;
}

// The same can be done with a function object
class twoVariableFunctionObject {
public:
	twoVariableFunctionObject():i_(0), d_(0.) {}
	void operator()(int i, double d) {
		i_ = i;
		d_ = d;
	}

	int getInt() const {return i_;}
	double getDouble() const {return d_;}

private:
	int i_;
	double d_;
};

// Call back function for a std::vector, plus global std::vector<double>
std::vector<double> someGlobalDoubleVec;
void setGlobalDoubleVec(std::vector<double> par) {
	someGlobalDoubleVec = par;
}

// Call back function for a std::array object, plus global array object as target
const std::size_t ARRAYSIZE = 5;
std::array<int,ARRAYSIZE> someGlobalStdArray;
void setGlobalStdArray(std::array<int,ARRAYSIZE> par) {
	someGlobalStdArray = par;
}

/************************************************************************
 * This example illustrates the usage options of the GParserBuilder class
 */
int main(int argc, char **argv) {
	int creationSwitcher;
	bool useOperator;
	std::string fileName;

	// Create the parser builder
	Gem::Common::GParserBuilder gpb;

	//----------------------------------------------------------------
	// Register some command line options
	gpb.registerCLParameter<int>(
		"creationSwitcher,c"
		, creationSwitcher
		, int(0)
		, "Allows to switch between configuration file creation (0) and file parsing (1)"
	);

	// Information may be streamed!
	gpb.registerCLParameter<bool>(
		"useOperator,o"
		, useOperator
		, false
	) << "Allows to enforce usage of gpb.registerFileParameter() << comment";

	gpb.registerCLParameter<std::string>(
		"fileName,f"
		, fileName
		, "./config/configFile.json"
		, "The name of the file information should be written to or read from"
	);

	// Parse the command line and leave if the help flag was given
	if(Gem::Common::GCL_HELP_REQUESTED == gpb.parseCommandLine(argc, argv, true /*verbose*/)) {
		return 0;
	}

	//----------------------------------------------------------------
	// Example 1: Registering a call-back function (which in this
	// case sets a globally defined integer variable)
	if(useOperator) {
		gpb.registerFileParameter<int>(
			"iOption2"
			, SOMEGLOBALINTDEFAULT
			, setGlobalInt
		)
		<< "This is a comment for a call-back option" << std::endl;
	} else {
		gpb.registerFileParameter<int>(
			"iOption2"
			, SOMEGLOBALINTDEFAULT
			, setGlobalInt
			, Gem::Common::VAR_IS_SECONDARY // Could also be VAR_IS_ESSENTIAL
			, "This is a comment for a call-back option"
		);
	}

	//----------------------------------------------------------------
	// Example 2: Registering a function or function object as call-back
	// that expects two parameters. This is meant for parameters that
	// only make sense when set together. Example: Lower and upper boundaries
	// of a random number generator.

	twoVariableFunctionObject tvfo;

	const int I3DEFAULT = 3;
	const double D3DEFAULT = 3.;

	if(useOperator) {
		gpb.registerFileParameter<int,double>(
			"iOption3"
			, "dOption1"
			, I3DEFAULT
			, D3DEFAULT
			, tvfo
			, "combinedLabel"
		)
		<< "A comment concerning the first option"
		<< nextComment() // commentLevel(1) would be another option
		<< "A comment concerning the second option;with a second line";
	} else {
		gpb.registerFileParameter<int,double>(
			"iOption3"
			, "dOption1"
			, I3DEFAULT
			, D3DEFAULT
			, tvfo
			, "combinedLabel"
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "A comment concerning the first option"
			, "A comment concerning the second option;with a second line"
		);
	}

	//----------------------------------------------------------------
	// Example 3: We can directly set a variable by providing a reference to it.
	int i = 0; const int IDEFAULT = 0;

	if(useOperator) {
		gpb.registerFileParameter<int>(
			"iOption"
			, i
			, IDEFAULT
		)
		<< "This is a comment; This is the second line of the comment";
	} else {
		gpb.registerFileParameter<int>(
			"iOption"
			, i
			, IDEFAULT
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "This is a comment; This is the second line of the comment"
		);
	}

	//----------------------------------------------------------------
	// Example 4: Adding a std::vector<> of configurable type to the config
	// file so a set of values can be read in one go
	std::vector<double> defaultDoubleVec4; // The default values
	defaultDoubleVec4.push_back(0.);
	defaultDoubleVec4.push_back(1.);

	if(useOperator) {
		gpb.registerFileParameter<double>(
			"vectorOptionsWithCallback"
			, defaultDoubleVec4
			, setGlobalDoubleVec // The call-back function. See at the beginning of this file
		)
		<< "Yet another comment";
	} else {
		gpb.registerFileParameter<double>(
			"vectorOptionsWithCallback"
			, defaultDoubleVec4
			, setGlobalDoubleVec // The call-back function. See at the beginning of this file
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "Yet another comment"
		);
	}

	//----------------------------------------------------------------
	// Example 5: Adding a reference to a vector of configurable type
	// to the collection.

	std::vector<double> targetDoubleVector; // Will hold the read values

	std::vector<double> defaultDoubleVec5; // The default values
	defaultDoubleVec5.push_back(0.);
	defaultDoubleVec5.push_back(1.);

	if(useOperator) {
		gpb.registerFileParameter<double>(
			"vectorOptionsReference"
			, targetDoubleVector
			, defaultDoubleVec5
		)
		<< "And yet another comment";

	} else {
		gpb.registerFileParameter<double>(
			"vectorOptionsReference"
			, targetDoubleVector
			, defaultDoubleVec5
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "And yet another comment"
		);
	}

	//----------------------------------------------------------------
	// Example 6: Adding an array of fixed size to the collection

	std::array<int,ARRAYSIZE> defValArray;
	i=0; // Already declared above
	for(auto &x: defValArray)  { // Set the default values
		x = boost::numeric_cast<int>(i++);
	}

	if(useOperator) {
		gpb.registerFileParameter<int,ARRAYSIZE>(
			"StdArrayWithCallback"
			, defValArray
			, setGlobalStdArray // The call back function
		)
		<< "A comment regarding arrays with call-back functions";
	} else {
		gpb.registerFileParameter<int,ARRAYSIZE>(
			"StdArrayWithCallback"
			, defValArray
			, setGlobalStdArray // The call back function
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "A comment regarding arrays with call-back functions"
		);
	}

	//----------------------------------------------------------------
	// Example 7: Adding a reference to a std::array object. We
	// use the same default values as in example 6.

	std::array<int,ARRAYSIZE> targetArray;

	if(useOperator) {
		gpb.registerFileParameter<int,ARRAYSIZE>(
			"StdArrayReference"
			, targetArray
			, defValArray
		)
		<< "A comment regarding std::array references";
	} else {
		gpb.registerFileParameter<int,ARRAYSIZE>(
			"StdArrayReference"
			, targetArray
			, defValArray
			, Gem::Common::VAR_IS_ESSENTIAL // Could also be VAR_IS_SECONDARY
			, "A comment regarding std::array references"
		);
	}

	//----------------------------------------------------------------
	//////////////////////////////////////////////////////////////////
	//----------------------------------------------------------------

	// Check the number of registered options
	std::cout << "Got " << gpb.numberOfFileOptions() << " options." << std::endl;

	// Create a suitable path for the config file
	std::filesystem::path file_path(fileName);

	// Depending on the command line argument, write or read a configuration file
	switch(creationSwitcher) {
		case 0: // file creation
		{
		    // writeConfigFile will fail if the config file already exists. Check for the existance of the file
		    // and erase it, if necessary
		    if(std::filesystem::exists(file_path)) std::filesystem::remove(file_path);

			std::string header = "This is a not so complicated header;with a second line;and a third line as well";
			bool writeAll = true; // If set to false, only essential (but no secondary variables) are written
			gpb.writeConfigFile(file_path, header, writeAll);
		}
			break;

		case 1: // file parsing
		{
			gpb.parseConfigFile(file_path);
		}
			break;

		default: // Complain
			throw;
	}
}
