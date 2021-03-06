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

#include "geneva/GBooleanCollection.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Gem::Geneva::GBooleanCollection)

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * Initializes the class with a set of nval random bits.
 *
 * @param nval The size of the collection
 */
GBooleanCollection::GBooleanCollection(const std::size_t &nval)
	: GParameterCollectionT<bool>()
{
	Gem::Hap::GRandomT<Gem::Hap::RANDFLAVOURS::RANDOMLOCAL> gr;
	std::bernoulli_distribution bernoulli_distribution; // defaults to 0.5
	for (std::size_t i = 0; i < nval; i++) {
		this->push_back(bernoulli_distribution(gr));
	}
}

// Tested in this class

/******************************************************************************/
/**
 * Initializes the class with a set of nval variables of identical value
 *
 * @param nval The size of the collection
 * @param val  The value to be assigned to each position
 */
GBooleanCollection::GBooleanCollection(const std::size_t &nval, const bool &val)
	: GParameterCollectionT<bool>(nval, val)
{ /* nothing */ }

// Tested in this class

/******************************************************************************/
/**
 * Initializes the class with nval random bits, of which probability percent
 * have the value true. E.g., a probability value of 0.7 results in approimately
 * 70% "true" values.
 *
 * @param nval The size of the collection
 * @param probability The probability for true values in the collection
 */
GBooleanCollection::GBooleanCollection(
	const std::size_t &nval
	, const double &probability)
	: GParameterCollectionT<bool>()
{
	Gem::Hap::GRandomT<Gem::Hap::RANDFLAVOURS::RANDOMLOCAL> gr;
	std::bernoulli_distribution weighted_bool(probability);
	for (std::size_t i = 0; i < nval; i++) {
		this->push_back(weighted_bool(gr));
	}
}

// Tested in this class

/******************************************************************************/
/**
 * FLips the value at a given position
 */
void GBooleanCollection::flip(const std::size_t& pos) {
#ifdef DEBUG
	if(this->size() <= pos) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GBooleanCollection::flip(const std::size_t& " << pos << "): Error!" << std::endl
				<< "Tried to exist position beyond end of vector of size " << this->size() << std::endl
		);
	}
#endif

	if(true == this->at(pos)) {
		this->at(pos) = false;
	} else {
		this->at(pos) = true;
	}
}

/******************************************************************************/
/**
 * Creates a deep clone of this object
 *
 * @return A deep clone of this object
 */
GObject *GBooleanCollection::clone_() const {
	return new GBooleanCollection(*this);
}

/******************************************************************************/
/**
 * Loads the data of another GBooleanCollection object, camouflaged as
 * a GObject.
 *
 * @param gb A pointer to another GBooleanCollection object, camouflaged as a GObject
 */
void GBooleanCollection::load_(const GObject *cp) {
	// Convert the pointer to our target type and check for self-assignment
	const GBooleanCollection * p_load = Gem::Common::g_convert_and_compare<GObject, GBooleanCollection >(cp, this);

	GParameterCollectionT<bool>::load_(cp);
}

/******************************************************************************/
/**
 * Triggers random initialization of the parameter collection. Note that this
 * function assumes that the collection has been completely set up. Data
 * that is added later will remain unaffected.
 */
bool GBooleanCollection::randomInit_(
	const activityMode &am
	, Gem::Hap::GRandomBase& gr
) {
	bool randomized = false;

	std::bernoulli_distribution bernoulli_distribution; // defaults to 0.5

	// Compare http://stackoverflow.com/questions/15927033/what-is-the-correct-way-of-using-c11s-range-based-for
	for(auto&& b: this->m_data_cnt) {
		b = bernoulli_distribution(gr);
		randomized = true;
	}

	return randomized;
}

/******************************************************************************/
/**
 * Random initialization with a given probability structure. E.g., a probability
 * value of 0.7 results in approimately 70% "true" values.
 *
 * @param probability The probability for true values in the collection
 */
bool GBooleanCollection::randomInit_(
	const double &probability
	, const activityMode &
	, Gem::Hap::GRandomBase& gr
) {
	bool randomized = false;

	// Do some error checks
	if(not Gem::Common::checkRangeCompliance(probability, 0., 1., "GBooleanCollection::randomInit_(probability)")) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GBooleanCollection::randomInit_(probability): Error!" << std::endl
				<< "Probability " << probability << " not in allowed value range [0,1]" << std::endl
		);
	}

	// Obtain access to a random number generator
	std::bernoulli_distribution bernoulli_distribution(probability);

	// Compare http://stackoverflow.com/questions/15927033/what-is-the-correct-way-of-using-c11s-range-based-for
	for(auto&& b: this->m_data_cnt) {
		b = bernoulli_distribution(gr);
		randomized = true;
	}

	return randomized;
}

/******************************************************************************/
/**
 * Random initialization. This is a helper function, without it we'd
 * have to say things like "myGBooleanCollectionObject.GParameterBase::randomInit();".
 */
bool GBooleanCollection::randomInit(
	const activityMode &am
	, Gem::Hap::GRandomBase& gr
) {
	return GParameterBase::randomInit(am, gr); // This will also take into account the "blocked initialization" flag
}

/******************************************************************************/
/**
 * Random initialization with a given probability structure,
 * if re-initialization has not been blocked.
 *
 * @param probability The probability for true values in the collection
 */
bool GBooleanCollection::randomInit(
	const double &probability
	, const activityMode &am
	, Gem::Hap::GRandomBase& gr
) {
	if (not GParameterBase::randomInitializationBlocked() && this->modifiableAmMatchOrHandover(am)) {
		return randomInit_(probability, am, gr);
	} else {
		return false;
	}
}

/***************************************************************************/
/**
 * Returns a "comparative range". In the case of boolean values this must
 * be considered to be more of a "dummy".
 */
bool GBooleanCollection::range() const {
	return true;
}

/******************************************************************************/
/**
 * Searches for compliance with expectations with respect to another object
 * of the same type
 *
 * @param cp A constant reference to another GObject object
 * @param e The expected outcome of the comparison
 * @param limit The maximum deviation for floating point values (important for similarity checks)
 */
void GBooleanCollection::compare_(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GBooleanCollection reference independent of this object and convert the pointer
	const GBooleanCollection *p_load = Gem::Common::g_convert_and_compare<GObject, GBooleanCollection >(cp, this);

	GToken token("GBooleanCollection", e);

	// Compare our parent data ...
	Gem::Common::compare_base_t<GParameterCollectionT<bool>>(*this, *p_load, token);

	// ... no local data

	// React on deviations from the expectation
	token.evaluate();
}

/***********************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GBooleanCollection::name_() const {
	return std::string("GBooleanCollection");
}

/******************************************************************************/
/**
 * Attach our local values to the vector. This is used to collect all parameters of this type
 * in the sequence in which they were registered.
 *
 * @param parVec The vector to which the local values should be attached
 */
void GBooleanCollection::booleanStreamline(
	std::vector<bool> &parVec, const activityMode &
) const {
	GBooleanCollection::const_iterator cit;
	for (cit = this->begin(); cit != this->end(); ++cit) {
		parVec.push_back(*cit);
	}
}

/******************************************************************************/
/**
 * Attach our local values to the map. Names are built from the object name and the
 * position in the array.
 *
 * @param parVec The map to which the local values should be attached
 */
void GBooleanCollection::booleanStreamline(
	std::map<std::string, std::vector<bool>> &parVec, const activityMode &am
) const {
#ifdef DEBUG
	if((this->getParameterName()).empty()) {
		throw gemfony_exception(
			g_error_streamer(DO_LOG,  time_and_place)
				<< "In GBooleanCollection::booleanStreamline(std::map<std::string, std::vector<bool>>& parVec) const: Error!" << std::endl
				<< "No name was assigned to the object" << std::endl
		);
	}
#endif /* DEBUG */

	std::vector<bool> parameters;
	this->booleanStreamline(parameters, am);
	parVec[this->getParameterName()] = parameters;
}

/******************************************************************************/
/**
 * Attach boundaries of type bool to the vectors
 *
 * @param lBndVec A vector of lower bool parameter boundaries
 * @param uBndVec A vector of upper bool parameter boundaries
 */
void GBooleanCollection::booleanBoundaries(
	std::vector<bool> &lBndVec, std::vector<bool> &uBndVec, const activityMode &am
) const {
	GBooleanCollection::const_iterator cit;
	for (cit = this->begin(); cit != this->end(); ++cit) {
		lBndVec.push_back(false);
		uBndVec.push_back(true);
	}
}

/******************************************************************************/
/**
 * Tell the audience that we own a number of bool values
 *
 * @param am An enum indicating whether only information about active, inactive or all parameters of this type should be extracted
 * @return The number of bool parameters
 */
std::size_t GBooleanCollection::countBoolParameters(
	const activityMode &am
) const {
	return this->size();
}

/******************************************************************************/
/**
 * Assigns part of a value vector to the parameter
 */
void GBooleanCollection::assignBooleanValueVector(
	const std::vector<bool> &parVec
	, std::size_t &pos
	, const activityMode &am
) {
	for (GBooleanCollection::iterator it = this->begin(); it != this->end(); ++it) {
#ifdef DEBUG
		// Do we have a valid position ?
		if(pos >= parVec.size()) {
			throw gemfony_exception(
				g_error_streamer(DO_LOG,  time_and_place)
					<< "In GBooleanCollection::assignBooleanValueVector(const std::vector<bool>&, std::size_t&):" << std::endl
					<< "Tried to access position beyond end of vector: " << parVec.size() << "/" << pos << std::endl
			);
		}
#endif

		(*it) = parVec[pos];
		pos++;
	}
}

/******************************************************************************/
/**
 * Assigns part of a value map to the parameter
 */
void GBooleanCollection::assignBooleanValueVectors(
	const std::map<std::string, std::vector<bool>> &parMap, const activityMode &am
) {
	GBooleanCollection::iterator it;
	std::size_t cnt = 0;
	for (it = this->begin(); it != this->end(); ++it) {
		*it = (Gem::Common::getMapItem<std::vector<bool>>(parMap, this->getParameterName())).at(cnt++);
	}
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 *
 * @return A boolean which indicates whether modifications were made
 */
bool GBooleanCollection::modify_GUnitTests_() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GParameterCollectionT<bool>::modify_GUnitTests_()) result = true;

	this->push_back(true);
	return true;

	return result;

#else /* GEM_TESTING */  // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBooleanCollection::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GBooleanCollection::specificTestsNoFailureExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// A few settings
	const std::size_t nItems = 10000;
	const bool FIXEDVALUEINIT = true;
	const double LOWERBND = 0.8, UPPERBND = 1.2;

	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	std::shared_ptr <GAdaptorT<bool>> storedAdaptor;

	if (this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	std::shared_ptr <GBooleanAdaptor> gba_ptr(new GBooleanAdaptor(1.0));
	gba_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gba_ptr->setAdaptionMode(adaptionMode::ALWAYS); // Always adapt
	this->addAdaptor(gba_ptr);

	// Call the parent class'es function
	GParameterCollectionT<bool>::specificTestsNoFailureExpected_GUnitTests_();

	// A random generator
	Gem::Hap::GRandomT<Gem::Hap::RANDFLAVOURS::RANDOMPROXY> gr;

	// --------------------------------------------------------------------------

	{ // Check default constructor
		GBooleanCollection gbc;
		BOOST_CHECK(gbc.empty());
	}

	// --------------------------------------------------------------------------

	{ // Check copy construction
		GBooleanCollection gbc1;
		BOOST_CHECK_NO_THROW(gbc1.push_back(true));
		GBooleanCollection gbc2(gbc1);
		BOOST_CHECK_MESSAGE(
			gbc2.size() == 1 && gbc2.at(0) == true, "\n"
			<< "gbc2.size() = " << gbc2.size()
			<< "gbc2.at(0) = " << gbc2.at(0)
		);
	}

	// --------------------------------------------------------------------------

	{ // Check construction with a number of random bits
		GBooleanCollection gbc(nItems);

		BOOST_CHECK_MESSAGE(
			gbc.size() == nItems, "\n"
			<< "gbc.size() = " << gbc.size() << "\n"
			<< "nItems = " << nItems
		);

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			gbc.at(i) ? nTrue++ : nFalse++;
		}

		// We allow a slight deviation, as the initialization is a random process
		BOOST_REQUIRE(nFalse != 0); // There should be a few false values
		double ratio = double(nTrue) / double(nFalse);
		BOOST_CHECK_MESSAGE(
			ratio > LOWERBND && ratio < UPPERBND, "\n"
			<< "ratio = " << ratio << "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nFalse = " << nFalse << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Check construction with a number of identical bits
		GBooleanCollection gbc(nItems, true);

		BOOST_CHECK_MESSAGE(
			gbc.size() == nItems, "\n"
			<< "gbc.size() = " << gbc.size()
			<< "nItems = " << nItems
		);

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			gbc.at(i) ? nTrue++ : nFalse++;
		}

		BOOST_CHECK_MESSAGE(
			nTrue == nItems, "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nItems = " << nItems << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Check construction with a given probability for the value true
		GBooleanCollection gbc(nItems, 0.5);

		BOOST_CHECK_MESSAGE(
			gbc.size() == nItems, "\n"
			<< "gbc.size() = " << gbc.size()
			<< "nItems = " << nItems
		);

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			gbc.at(i) ? nTrue++ : nFalse++;
		}

		// We allow a slight deviation, as the initialization is a random process
		BOOST_REQUIRE(nFalse != 0); // There should be a few false values
		double ratio = double(nTrue) / double(nFalse);
		BOOST_CHECK_MESSAGE(
			ratio > LOWERBND && ratio < UPPERBND, "\n"
			<< "ratio = " << ratio << "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nFalse = " << nFalse << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Test that random initialization with equal probability will result in roughly the same amount of true and false values
		std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

		// Make sure the collection is empty
		BOOST_CHECK_NO_THROW(p_test->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->push_back(true);
		}

		// Check the size
		BOOST_CHECK(p_test->size() == nItems);

		// Randomly initialize, using the internal function
		BOOST_CHECK_NO_THROW(p_test->randomInit_(activityMode::ALLPARAMETERS, gr));

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->at(i) ? nTrue++ : nFalse++;
		}

		// We allow a slight deviation, as the initialization is a random process
		BOOST_REQUIRE(nFalse != 0); // There should be a few false values
		double ratio = double(nTrue) / double(nFalse);
		BOOST_CHECK_MESSAGE(
			ratio > LOWERBND && ratio < UPPERBND, "\n"
			<< "ratio = " << ratio << "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nFalse = " << nFalse << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Check that initialization with a probabilty of 0. for true results in just false values
		std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

		// Make sure the collection is empty
		BOOST_CHECK_NO_THROW(p_test->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->push_back(true);
		}

		// Randomly initialize, using the internal function
		BOOST_CHECK_NO_THROW(p_test->randomInit_(0., activityMode::ALLPARAMETERS, gr));

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->at(i) ? nTrue++ : nFalse++;
		}

		// Cross-check
		BOOST_CHECK_MESSAGE(
			nTrue == 0, "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nFalse = " << nFalse << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Check that initialization with a probabilty of 1. for true results in just true values
		std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

		// Make sure the collection is empty
		BOOST_CHECK_NO_THROW(p_test->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->push_back(false);
		}

		// Randomly initialize, using the internal function
		BOOST_CHECK_NO_THROW(p_test->randomInit_(1., activityMode::ALLPARAMETERS, gr));

		// Count the number of true and false values
		std::size_t nTrue = 0;
		std::size_t nFalse = 0;
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->at(i) ? nTrue++ : nFalse++;
		}

		// Cross-check
		BOOST_CHECK_MESSAGE(
			nTrue == nItems, "\n"
			<< "nTrue = " << nTrue << "\n"
			<< "nFalse = " << nFalse << "\n"
		);
	}

	// --------------------------------------------------------------------------

	{ // Test that random initialization with a given probability will result in roughly the expected amount of true and false values
		for (double d = 0.1; d < 0.9; d += 0.1) {
			std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

			// Make sure the collection is empty
			BOOST_CHECK_NO_THROW(p_test->clear());

			// Add items of fixed value
			for (std::size_t i = 0; i < nItems; i++) {
				p_test->push_back(false);
			}

			// Randomly initialize, using the internal function and the required probability
			BOOST_CHECK_NO_THROW(p_test->randomInit_(d, activityMode::ALLPARAMETERS, gr));

			// Count the number of true and false values
			std::size_t nTrue = 0;
			std::size_t nFalse = 0;
			for (std::size_t i = 0; i < nItems; i++) {
				p_test->at(i) ? nTrue++ : nFalse++;
			}

			// We allow a slight deviation, as the initialization is a random process
			double expectedTrueMin = 0.8 * d * nItems;
			double expectedTrueMax = 1.2 * d * nItems;

			BOOST_CHECK_MESSAGE(
				double(nTrue) > expectedTrueMin && double(nTrue) < expectedTrueMax, "\n"
				<< "d = " << d << "\n"
				<< "Allowed window = " <<
				expectedTrueMin << " - " <<
				expectedTrueMax << "\n"
				<< "nItems = " << nItems << "\n"
				<< "nTrue = " << nTrue << "\n"
				<< "nFalse = " << nFalse << "\n"
			);
		}
	}

	// --------------------------------------------------------------------------

	{ // Check that random initialization can be blocked for equal distributions
		std::shared_ptr <GBooleanCollection> p_test1 = this->clone<GBooleanCollection>();
		std::shared_ptr <GBooleanCollection> p_test2 = this->clone<GBooleanCollection>();

		// Make sure the collections are empty
		BOOST_CHECK_NO_THROW(p_test1->clear());
		BOOST_CHECK_NO_THROW(p_test2->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test1->push_back(false);
		}

		// Block random initialization and cross check
		BOOST_CHECK_NO_THROW(p_test1->blockRandomInitialization());
		BOOST_CHECK(p_test1->randomInitializationBlocked() == true);

		// Load the data into p_test2
		BOOST_CHECK_NO_THROW(p_test2->load(p_test1));

		// Check that both objects are equal
		BOOST_CHECK(*p_test1 == *p_test2);

		// Check that random initialization is also blocked for p_test2
		BOOST_CHECK(p_test2->randomInitializationBlocked() == true);

		// Try to randomly initialize, using the *external* function
		BOOST_CHECK_NO_THROW(p_test1->randomInit(activityMode::ALLPARAMETERS, gr));

		// Check that both objects are still the same
		BOOST_CHECK(*p_test1 == *p_test2);
	}

	// --------------------------------------------------------------------------

	{ // Check that random initialization can be blocked for distributions with a given probability structure
		std::shared_ptr <GBooleanCollection> p_test1 = this->clone<GBooleanCollection>();
		std::shared_ptr <GBooleanCollection> p_test2 = this->clone<GBooleanCollection>();

		// Make sure the collections are empty
		BOOST_CHECK_NO_THROW(p_test1->clear());
		BOOST_CHECK_NO_THROW(p_test2->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test1->push_back(false);
		}

		// Block random initialization and cross check
		BOOST_CHECK_NO_THROW(p_test1->blockRandomInitialization());
		BOOST_CHECK(p_test1->randomInitializationBlocked() == true);

		// Load the data into p_test2
		BOOST_CHECK_NO_THROW(p_test2->load(p_test1));

		// Check that both objects are equal
		BOOST_CHECK(*p_test1 == *p_test2);

		// Check that random initialization is also blocked for p_test2
		BOOST_CHECK(p_test2->randomInitializationBlocked() == true);

		// Try to randomly initialize, using the *external* function
		BOOST_CHECK_NO_THROW(p_test1->randomInit(0.7, activityMode::ALLPARAMETERS, gr));

		// Check that both objects are still the same
		BOOST_CHECK(*p_test1 == *p_test2);
	}

	// --------------------------------------------------------------------------

	{ // Check that the fp-family of functions doesn't have an effect on this object
		std::shared_ptr <GBooleanCollection> p_test1 = this->GObject::clone<GBooleanCollection>();
		std::shared_ptr <GBooleanCollection> p_test2 = this->GObject::clone<GBooleanCollection>();
		std::shared_ptr <GBooleanCollection> p_test3 = this->GObject::clone<GBooleanCollection>();

		// Add a few items to p_test1
		for (std::size_t i = 0; i < nItems; i++) {
			p_test1->push_back(FIXEDVALUEINIT);
		}

		// Load into p_test2 and p_test3 and test equality
		BOOST_CHECK_NO_THROW(p_test2->load(p_test1));
		BOOST_CHECK_NO_THROW(p_test3->load(p_test1));
		BOOST_CHECK(*p_test2 == *p_test1);
		BOOST_CHECK(*p_test3 == *p_test1);
		BOOST_CHECK(*p_test3 == *p_test2);

		// Check that initialization with a fixed floating point value has no effect on this object
		BOOST_CHECK_NO_THROW(p_test2->fixedValueInit<double>(2., activityMode::ALLPARAMETERS));
		BOOST_CHECK(*p_test2 == *p_test1);

		// Check that multiplication with a fixed floating point value has no effect on this object
		BOOST_CHECK_NO_THROW(p_test2->multiplyBy<double>(2., activityMode::ALLPARAMETERS));
		BOOST_CHECK(*p_test2 == *p_test1);

		// Check that a component-wise multiplication with a random fp value in a given range does not have an effect on this object
		BOOST_CHECK_NO_THROW(p_test2->multiplyByRandom<double>(1., 2., activityMode::ALLPARAMETERS, gr));
		BOOST_CHECK(*p_test2 == *p_test1);

		// Check that a component-wise multiplication with a random fp value in the range [0:1[ does not have an effect on this object
		BOOST_CHECK_NO_THROW(p_test2->multiplyByRandom<double>(activityMode::ALLPARAMETERS, gr));
		BOOST_CHECK(*p_test2 == *p_test1);

		// Check that adding p_test1 to p_test3 does not have an effect
		BOOST_CHECK_NO_THROW(p_test3->add<double>(p_test1, activityMode::ALLPARAMETERS));
		BOOST_CHECK(*p_test3 == *p_test2);

		// Check that subtracting p_test1 from p_test3 does not have an effect
		BOOST_CHECK_NO_THROW(p_test3->subtract<double>(p_test1, activityMode::ALLPARAMETERS));
		BOOST_CHECK(*p_test3 == *p_test2);
	}

	// --------------------------------------------------------------------------

	// Remove the test adaptor
	this->resetAdaptor();

	// Load the old adaptor, if needed
	if (adaptorStored) {
		this->addAdaptor(storedAdaptor);
	}

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBooleanCollection::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GBooleanCollection::specificTestsFailuresExpected_GUnitTests_() {
#ifdef GEM_TESTING
	// A few settings
	std::size_t nItems = 10000;

	// Make sure we have an appropriate adaptor loaded when performing these tests
	bool adaptorStored = false;
	std::shared_ptr <GAdaptorT<bool>> storedAdaptor;

	if (this->hasAdaptor()) {
		storedAdaptor = this->getAdaptor();
		adaptorStored = true;
	}

	std::shared_ptr <GBooleanAdaptor> gba_ptr(new GBooleanAdaptor(1.0));
	gba_ptr->setAdaptionThreshold(0); // Make sure the adaptor's internal parameters don't change through the adaption
	gba_ptr->setAdaptionMode(adaptionMode::ALWAYS); // Always adapt
	this->addAdaptor(gba_ptr);

	// Call the parent class'es function
	GParameterCollectionT<bool>::specificTestsFailuresExpected_GUnitTests_();

	// A random generator
	Gem::Hap::GRandomT<Gem::Hap::RANDFLAVOURS::RANDOMPROXY> gr;

	// --------------------------------------------------------------------------

	{ // Check that random initialization with a probability < 0. throws
		std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

		// Make sure the collection is empty
		BOOST_CHECK_NO_THROW(p_test->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->push_back(true);
		}

		// Randomly initialize, using the internal function
		BOOST_CHECK_THROW(p_test->randomInit_(-1., activityMode::ALLPARAMETERS, gr), gemfony_exception);
	}

	// --------------------------------------------------------------------------

	{ // Check that random initialization with a probability > 1. throws
		std::shared_ptr <GBooleanCollection> p_test = this->clone<GBooleanCollection>();

		// Make sure the collection is empty
		BOOST_CHECK_NO_THROW(p_test->clear());

		// Add items of fixed value
		for (std::size_t i = 0; i < nItems; i++) {
			p_test->push_back(true);
		}

		// Randomly initialize, using the internal function
		BOOST_CHECK_THROW(p_test->randomInit_(2., activityMode::ALLPARAMETERS, gr), gemfony_exception);
	}

	// --------------------------------------------------------------------------

	// Remove the test adaptor
	this->resetAdaptor();

	// Load the old adaptor, if needed
	if (adaptorStored) {
		this->addAdaptor(storedAdaptor);
	}

#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
	Gem::Common::condnotset("GBooleanCollection::specificTestsFailuresExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */
