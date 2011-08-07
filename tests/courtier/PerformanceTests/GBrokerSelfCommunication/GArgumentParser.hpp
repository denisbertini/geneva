/**
 * @file GArgumentParser.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection.
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is free software: you can redistribute and/or modify it under
 * the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library. If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */

// Standard headers go here
#include <iostream>
#include <fstream>

// Boost headers go here
#include <boost/version.hpp>

#if BOOST_VERSION < 104000
#error "Error: Boost should at least have version 1.40 !"
#endif /* BOOST_VERSION */


#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>

#ifndef GARGUMENTPARSER_HPP_
#define GARGUMENTPARSER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva headers go here
#include "GBrokerSelfCommunicationEnums.hpp"
#include "common/GCommonEnums.hpp"

namespace Gem {
namespace Courtier {
namespace Tests {

namespace po = boost::program_options;

/********************************************************************************************/
// Default settings
const boost::uint32_t DEFAULTNPRODUCERSAP = 5;
const boost::uint32_t DEFAULTNPRODUCTIONCYLCESAP = 10000;
const bool DEFAULTCOMPLETERETURNREQUIREDAP = false;
const std::size_t DEFAULTMAXRESUBMISSIONSAP = 5;
const boost::uint32_t DEFAULTNCONTAINEROBJECTSAP = 100;
const std::size_t DEFAULTNCONTAINERENTRIESAP = 100;
const boost::uint32_t DEFAULTNWORKERSAP = 4;
const GBSCModes DEFAULTEXECUTIONMODEAP = Gem::Courtier::Tests::MULTITHREADING;
const unsigned short DEFAULTPORTAP=10000;
const std::string DEFAULTIPAP="localhost";
const std::string DEFAULTCONFIGFILEAP="./GBrokerSelfCommunication.cfg";
const boost::uint16_t DEFAULTPARALLELIZATIONMODEAP=0;
const Gem::Common::serializationMode DEFAULTSERMODEAP=Gem::Common::SERIALIZATIONMODE_BINARY;

/********************************************************************************************/

bool parseCommandLine(
		int
	  , char **
	  , std::string& configFile
	  , GBSCModes&
	  , bool&
	  , std::string&
	  , unsigned short&
	  , Gem::Common::serializationMode&
	  , bool&
);

/********************************************************************************************/

bool parseConfigFile(
		const std::string&
	  , boost::uint32_t&
	  , boost::uint32_t&
	  , boost::uint32_t&
	  , std::size_t&
	  , std::size_t&
	  , boost::uint32_t&
);

/********************************************************************************************/

} /* namespace Tests */
} /* namespace Courtier */
} /* namespace Gem */

#endif /* GARGUMENTPARSER_HPP_ */