/**
 * @file GSwarmPersonalityTraits.cpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
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

#include "GSwarmPersonalityTraits.hpp"

// Included here so no conflicts occur. See explanation at
// http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::GenEvA::GSwarmPersonalityTraits)

namespace Gem {
namespace GenEvA {

/*****************************************************************************/
/**
 * The default constructor
 */
GSwarmPersonalityTraits::GSwarmPersonalityTraits()
	: GPersonalityTraits()
	, popPos_(0)
	, command_("")
{ /* nothing */ }

/*****************************************************************************/
/**
 * The copy contructor
 *
 * @param cp A copy of another GSwarmPersonalityTraits object
 */
GSwarmPersonalityTraits::GSwarmPersonalityTraits(const GSwarmPersonalityTraits& cp)
	: GPersonalityTraits(cp)
	, popPos_(cp.popPos_)
	, command_(cp.command_)
{ /* nothing */ }

/*****************************************************************************/
/**
 * The standard destructor
 */
GSwarmPersonalityTraits::~GSwarmPersonalityTraits()
{ /* nothing */ }

/*****************************************************************************/
/**
 * Checks for equality with another GSwarmPersonalityTraits object
 *
 * @param  cp A constant reference to another GSwarmPersonalityTraits object
 * @return A boolean indicating whether both objects are equal
 */
bool GSwarmPersonalityTraits::operator==(const GSwarmPersonalityTraits& cp) const {
	return GSwarmPersonalityTraits::isEqualTo(cp, boost::logic::indeterminate);
}

/*****************************************************************************/
/**
 * Checks for inequality with another GSwarmPersonalityTraits object
 *
 * @param  cp A constant reference to another GSwarmPersonalityTraits object
 * @return A boolean indicating whether both objects are inequal
 */
bool GSwarmPersonalityTraits::operator!=(const GSwarmPersonalityTraits& cp) const {
	return !GSwarmPersonalityTraits::isEqualTo(cp, boost::logic::indeterminate);
}

/*****************************************************************************/
/**
 * Checks for equality with another GSwarmPersonalityTraits object
 *
 * @param  cp A constant reference to another GPersonalityTraits object
 * @return A boolean indicating whether both objects are equal
 */
bool GSwarmPersonalityTraits::isEqualTo(const GObject& cp, const boost::logic::tribool& expected) const {
    using namespace Gem::Util;

	// Check that we are indeed dealing with a GPersonalityTraits reference
	const GSwarmPersonalityTraits *p_load = GObject::conversion_cast(&cp,  this);

	// Check for equality of our parent class
	if(!GPersonalityTraits::isEqualTo(*p_load, expected)) return  false;

	// Then we take care of the local data
	if(checkForInequality("GSwarmPersonalityTraits", popPos_, p_load->popPos_,"popPos_", "p_load->popPos_", expected)) return false;
	if(checkForInequality("GSwarmPersonalityTraits", command_, p_load->command_,"command_", "p_load->command_", expected)) return false;

	return true;
}

/*****************************************************************************/
/**
 * Checks for similarity with another GSwarmPersonalityTraits object
 *
 * @param  cp A constant reference to another GSwarmPersonalityTraits object
 * @param limit A double value specifying the acceptable level of differences of floating point values
 * @return A boolean indicating whether both objects are similar to each other
 */
bool GSwarmPersonalityTraits::isSimilarTo(const GObject& cp, const double& limit, const boost::logic::tribool& expected) const {
    using namespace Gem::Util;

	// Check that we are indeed dealing with a GPersonalityTraits reference
	const GSwarmPersonalityTraits *p_load = GObject::conversion_cast(&cp,  this);

	// Check for equality of our parent class
	if(!GPersonalityTraits::isSimilarTo(*p_load, limit, expected)) return false;

	// Then we take care of the local data
	if(checkForDissimilarity("GSwarmPersonalityTraits", popPos_, p_load->popPos_, limit, "popPos_", "p_load->popPos_", expected)) return false;
	if(checkForDissimilarity("GSwarmPersonalityTraits", command_, p_load->command_, limit, "command_", "p_load->command_", expected)) return false;

	return true;
}

/*****************************************************************************/
/**
 * Checks whether a given expectation for the relationship between this object and another object
 * is fulfilled.
 *
 * @param cp A constant reference to another object, camouflaged as a GObject
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 * @param caller An identifier for the calling entity
 * @param y_name An identifier for the object that should be compared to this one
 * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
 * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
 */
boost::optional<std::string> GSwarmPersonalityTraits::checkRelationshipWith(const GObject& cp,
		const Gem::Util::expectation& e,
		const double& limit,
		const std::string& caller,
		const std::string& y_name,
		const bool& withMessages) const
{
    using namespace Gem::Util;
    using namespace Gem::Util::POD;

	// Check that we are indeed dealing with a GParamterBase reference
	const GSwarmPersonalityTraits *p_load = GObject::conversion_cast(&cp,  this);

	// Will hold possible deviations from the expectation, including explanations
    std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
	deviations.push_back(GPersonalityTraits::checkRelationshipWith(cp, e, limit, "GSwarmPersonalityTraits", y_name, withMessages));

	// ... and then our local data
	deviations.push_back(checkExpectation(withMessages, "GSwarmPersonalityTraits", popPos_, p_load->popPos_, "popPos_", "p_load->popPos_", e , limit));
	deviations.push_back(checkExpectation(withMessages, "GSwarmPersonalityTraits", command_, p_load->command_, "command_", "p_load->command_", e , limit));

	return evaluateDiscrepancies("GEAPersonalityTraits", caller, deviations, e);
}

/*****************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A clone of this object, camouflaged as a GObject
 */
GObject* GSwarmPersonalityTraits::clone_() const {
	return new GSwarmPersonalityTraits(*this);
}

/*****************************************************************************/
/**
 * Loads the data of another GSwarmPersonalityTraits object
 *
 * @param cp A copy of another GSwarmPersonalityTraits object, camouflaged as a GObject
 */
void GSwarmPersonalityTraits::load(const GObject* cp) {
	const GSwarmPersonalityTraits *p_load = this->conversion_cast(cp, this);

	// Load the parent class'es data
	GObject::load(cp);

	// and then the local data
	popPos_ = p_load->popPos_;
	command_ = p_load->command_;
}

/*****************************************************************************/
/**
 * Sets the position of the individual in the population. This is needed so an
 * individual can be re-assigned to the same neighborhood upon return from a
 * network trip. Individuals do not change positions in swarm algorithms.
 *
 * @param popPos The new position of this individual in the population
 */
void GSwarmPersonalityTraits::setPopulationPosition(std::size_t popPos) {
	popPos_ = popPos;
}

/*****************************************************************************/
/**
 * Retrieves the position of the individual in the population
 *
 * @return The current position of this individual in the population
 */
std::size_t GSwarmPersonalityTraits::getPopulationPosition(void) const {
	return popPos_;
}

/*****************************************************************************/
/**
 * Sets a command to be performed by a remote client.
 *
 * @param command The command to be performed by a remote client
 */
void GSwarmPersonalityTraits::setCommand(const std::string& command) {
	if(command != "evaluate") { // The allowed "grammar"
		std::ostringstream error;
		error << "In GSwarmPersonalityTraits::setCommand(): Got invalid command " << command << std::endl;
		throw(Gem::GenEvA::geneva_error_condition(error.str()));
	}

	command_ = command;
}

/*****************************************************************************/
/**
 * Retrieves the command to be performed by a remote client.
 *
 * @return The command to be performed by a remote client.
 */
std::string GSwarmPersonalityTraits::getCommand() const {
	return command_;
}

/*****************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
