/**
 * @file GCommandLineParser.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich  and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library, Gemfony scientific's optimization
 * library.
 *
 * Geneva is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU Affero General Public License
 * as published by the Free Software Foundation.
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with the Geneva library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.com .
 */

// Standard headers go here

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Boost headers go here

#include <boost/version.hpp>

#if BOOST_VERSION < 103600
#error "Error: Boost should at least have version 1.36 !"
#endif /* BOOST_VERSION */

#include <boost/program_options.hpp>

#ifndef GCOMMANDLINEPARSER_HPP_
#define GCOMMANDLINEPARSER_HPP_

// GenEvA headers go here
#include "GEnums.hpp"
#include "GSerializationHelperFunctions.hpp"

namespace Gem
{
namespace GenEvA
{

// Default settings
const std::string DEFAULTCLASSNAME="sampleIndividual";

namespace po = boost::program_options;

bool parseCommandLine(int argc, char **argv,
					  std::string& className,
					  bool& verbose);

} /* namespace GenEvA */
} /* namespace Gem */

#endif /* GCOMMANDLINEPARSER_HPP_ */
