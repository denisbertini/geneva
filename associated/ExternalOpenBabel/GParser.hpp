/**
 * @file GParser.hpp
 */

/*
 * Copyright (C) Dr. Ruediger Berlich  and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the eminim2 program, a demo application in the
 * context of Gemfony scientific's optimization library.
 *
 * eminim2 is free software: you can redistribute and/or modify this file
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * eminim2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Geneva library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */


// Standard headers go here
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

// Boost headers go here
#include <boost/program_options.hpp>
#include <boost/cstdint.hpp>

// OpenBabel headers go here
#include <openbabel/babelconfig.h>
#include <openbabel/base.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/forcefield.h>
#include <openbabel/internalcoord.h>

#ifndef GPARSER_HPP_
#define GPARSER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Default settings
const unsigned short DEFAULTLOGLEVEL=0;
const bool DEFAULTADDHYDROGENS=false;
const std::string DEFAULTFORCEFIELD="MMFF94s"; //"MMFF94";
const std::string DEFAULTPROTEINDESCRIPTION="unknown";

const std::string DEFAULTPARAMFILE="empty";
const boost::uint16_t DEFAULTTRANSFERMODE=0; // binary mode
const std::string DEFAULTIDENTIFYER="empty";
const std::string DEFAULTCONFIGFILE="./eminim2.cfg";

namespace po = boost::program_options;

bool parseCommandLine(int argc, char **argv,
			  boost::uint16_t& executionMode,
			  std::string& paramfile,
			  boost::uint16_t& transferMode,
			  std::string& identifyer,
			  std::string& configFile
);

bool parseConfigFile(const std::string& configFile,
		unsigned short& loglevel,
		bool& addhydrogens,
		std::string& forcefield,
		std::string& proteinDescription
);

#endif /* GPARSER_HPP_ */
