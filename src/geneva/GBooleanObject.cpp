/**
 * @file GBooleanObject.cpp
 */

/*
 * Copyright (C) Authors of the Geneva library collection and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library collection
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

#include "geneva/GBooleanObject.hpp"

/**
 * Included here so no conflicts occur. See explanation at
 * http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
 */
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::Geneva::GBooleanObject)

namespace Gem {
namespace Geneva {

/*******************************************************************************************/
/**
 * The default constructor
 */
GBooleanObject::GBooleanObject()
{ /* nothing */ }

/*******************************************************************************************/
/**
 * The copy constructor
 *
 * @param cp A copy of another GBooleanObject object
 */
GBooleanObject::GBooleanObject(const GBooleanObject& cp)
	: GParameterT<bool>(cp)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * Initialization by contained value
 *
 * @param val A value used for the initialization
 */
GBooleanObject::GBooleanObject(const bool& val)
	: GParameterT<bool>(val)
{ /* nothing */ }

/*******************************************************************************************/
/**
 * The destructor
 */
GBooleanObject::~GBooleanObject()
{ /* nothing */ }

/*******************************************************************************************/
/**
 * An assignment operator
 *
 * @param val The value to be assigned to this object
 * @return The value that was just assigned to this object
 */
bool GBooleanObject::operator=(const bool& val) {
	return GParameterT<bool>::operator=(val);
}

/*******************************************************************************************/
/**
 * A standard assignment operator.
 *
 * @param cp A copy of another GBooleanObject object
 * @return A constant reference to this object
 */
const GBooleanObject& GBooleanObject::operator=(const GBooleanObject& cp){
	GBooleanObject::load_(&cp);
	return *this;
}

/*******************************************************************************************/
/**
 * Creates a deep clone of this object.
 *
 * @return A copy of this object, camouflaged as a GObject
 */
GObject* GBooleanObject::clone_() const {
	return new GBooleanObject(*this);
}

/*******************************************************************************************/
/**
 * Checks for equality with another GBooleanObject object
 *
 * @param  cp A constant reference to another GBooleanObject object
 * @return A boolean indicating whether both objects are equal
 */
bool GBooleanObject::operator==(const GBooleanObject& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GBooleanObject::operator==","cp", CE_SILENT);
}

/*******************************************************************************************/
/**
 * Checks for inequality with another GBooleanObject object
 *
 * @param  cp A constant reference to another GBooleanObject object
 * @return A boolean indicating whether both objects are inequal
 */
bool GBooleanObject::operator!=(const GBooleanObject& cp) const {
	using namespace Gem::Common;
	// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
	return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GBooleanObject::operator!=","cp", CE_SILENT);
}

/*******************************************************************************************/
/**
 * Random initialization. This is a helper function, without it we'd
 * have to say things like "myGBooleanObject.GParameterBase::randomInit();".
 */
void GBooleanObject::randomInit() {
	  GParameterBase::randomInit();
}

/*******************************************************************************************/
/**
 * Triggers random initialization of the parameter object, with a given likelihood structure
 */
void GBooleanObject::randomInit(const double& probability) {
  if(!GParameterBase::randomInitializationBlocked()) randomInit_(probability);
}

/*******************************************************************************************/
/**
 * Triggers random initialization of the parameter object, with a given likelihood structure.
 * This function holds the actual initialization logic, used in the publicly accessible
 * GBooleanObject::randomInit(const double& probability) function.
 */
void GBooleanObject::randomInit_(const double& probability) {
	this->setValue(gr->weighted_bool(probability));
}

/*******************************************************************************************/
/**
 * Triggers random initialization of the parameter object
 */
void GBooleanObject::randomInit_() {
	this->setValue(gr->uniform_bool());
}

/*******************************************************************************************/
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
boost::optional<std::string> GBooleanObject::checkRelationshipWith(const GObject& cp,
		const Gem::Common::expectation& e,
		const double& limit,
		const std::string& caller,
		const std::string& y_name,
		const bool& withMessages) const
{
    using namespace Gem::Common;

	// Check for a possible self-assignment
	GObject::selfAssignmentCheck<GBooleanObject>(&cp);

	// Will hold possible deviations from the expectation, including explanations
    std::vector<boost::optional<std::string> > deviations;

	// Check our parent class'es data ...
	deviations.push_back(GParameterT<bool>::checkRelationshipWith(cp, e, limit, "GBooleanObject", y_name, withMessages));

	// no local data ...

	return evaluateDiscrepancies("GBooleanObject", caller, deviations, e);
}

/*******************************************************************************************/
/**
 * Loads the data of another GObject
 *
 * @param cp A copy of another GBooleanObject object, camouflaged as a GObject
 */
void GBooleanObject::load_(const GObject* cp){
	// Check for a possible self-assignment
	GObject::selfAssignmentCheck<GBooleanObject>(cp);

	// Load our parent class'es data ...
	GParameterT<bool>::load_(cp);

	// ... no local data
}

#ifdef GENEVATESTING

/*******************************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GBooleanObject::modify_GUnitTests() {
	bool result = false;

	// Call the parent class'es function
	if(GParameterT<bool>::modify_GUnitTests()) result = true;

	return result;
}

/*******************************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GBooleanObject::specificTestsNoFailureExpected_GUnitTests() {
	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	boost::shared_ptr<GAdaptorT<bool> > storedAdaptor;

	if(this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	boost::shared_ptr<GBooleanAdaptor> gba_ptr(new GBooleanAdaptor(1.0));
	gba_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gba_ptr->setAdaptionMode(true); // Always adapt
	this->addAdaptor(gba_ptr);

	// Call the parent class'es function
	GParameterT<bool>::specificTestsNoFailureExpected_GUnitTests();

	// Remove the test adaptor
	this->resetAdaptor();

	// Load the old adaptor, if needed
	if(adaptorStored) {
		this->addAdaptor(storedAdaptor);
	}
}

/*******************************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GBooleanObject::specificTestsFailuresExpected_GUnitTests() {
	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	boost::shared_ptr<GAdaptorT<bool> > storedAdaptor;

	if(this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	boost::shared_ptr<GBooleanAdaptor> gba_ptr(new GBooleanAdaptor(1.0));
	gba_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gba_ptr->setAdaptionMode(true); // Always adapt
	this->addAdaptor(gba_ptr);

	// Call the parent class'es function
	GParameterT<bool>::specificTestsFailuresExpected_GUnitTests();

	// Remove the test adaptor
	this->resetAdaptor();

	// Load the old adaptor, if needed
	if(adaptorStored) {
		this->addAdaptor(storedAdaptor);
	}
}

/*******************************************************************************************/

#endif /* GENEVATESTING */

} /* namespace Geneva */
} /* namespace Gem */
