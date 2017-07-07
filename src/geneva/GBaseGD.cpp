/**
 * @file GBaseGD.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
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
 * http://www.gemfony.eu .
 */

#include "geneva/GBaseGD.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * The default constructor
 */
GBaseGD::GBaseGD()
	: GBaseGD(DEFAULTGDSTARTINGPOINTS, DEFAULTFINITESTEP, DEFAULTSTEPSIZE)
{ /* nothing */ }

/******************************************************************************/
/**
 * Initialization with the number of starting points and other parameters
 *
 * @param nStartingPoints The number of simultaneous starting points for the gradient descent
 * @param finiteStep The desired size of the incremental adaption process
 * @param stepSize The size of the multiplicative factor of the adaption process
 */
GBaseGD::GBaseGD(
	const std::size_t &nStartingPoints
	, const double &finiteStep
	, const double &stepSize
)
	: GOptimizationAlgorithmT<GParameterSet>()
	, nStartingPoints_(nStartingPoints)
	, finiteStep_(finiteStep)
	, stepSize_(stepSize)
{ /* nothing */ }

/******************************************************************************/
/**
 * A standard copy constructor.
 *
 * @param cp A copy of another GradientDescent object
 */
GBaseGD::GBaseGD(const GBaseGD &cp)
	: GOptimizationAlgorithmT<GParameterSet>(cp)
	, nStartingPoints_(cp.nStartingPoints_)
	, nFPParmsFirst_(cp.nFPParmsFirst_)
	, finiteStep_(cp.finiteStep_)
	, stepSize_(cp.stepSize_)
	, stepRatio_(cp.stepRatio_) // Will be recalculated in init()
	, dblLowerParameterBoundaries_(cp.dblLowerParameterBoundaries_) // Will be extracted in init()
	, dblUpperParameterBoundaries_(cp.dblUpperParameterBoundaries_) // Will be extracted in init()
	, adjustedFiniteStep_(cp.adjustedFiniteStep_) // Will be recalculated in init()
{
	// Copying / setting of the optimization algorithm id is done by the parent class. The same
	// applies to the copying of the optimization monitor.
}

/******************************************************************************/
/**
 * The destructor
 */
GBaseGD::~GBaseGD() { /* nothing */ }

/***************************************************************************/
/**
 * The standard assignment operator
 */
const GBaseGD &GBaseGD::operator=(const GBaseGD &cp) {
	this->load_(&cp);
	return *this;
}

/******************************************************************************/
/**
 * Checks for equality with another GBaseGD object
 *
 * @param  cp A constant reference to another GBaseGD object
 * @return A boolean indicating whether both objects are equal
 */
bool GBaseGD::operator==(const GBaseGD &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_EQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Checks for inequality with another GBaseGD object
 *
 * @param  cp A constant reference to another GBaseGD object
 * @return A boolean indicating whether both objects are unequal
 */
bool GBaseGD::operator!=(const GBaseGD &cp) const {
	using namespace Gem::Common;
	try {
		this->compare(cp, Gem::Common::expectation::CE_INEQUALITY, CE_DEF_SIMILARITY_DIFFERENCE);
		return true;
	} catch (g_expectation_violation &) {
		return false;
	}
}

/******************************************************************************/
/**
 * Returns information about the type of optimization algorithm. This function needs
 * to be overloaded by the actual algorithms to return the correct type.
 *
 * @return The type of optimization algorithm
 */
std::string GBaseGD::getOptimizationAlgorithm() const {
	return "PERSONALITY_GD";
}

/******************************************************************************/
/**
 * Retrieves the number of starting points of the algorithm
 *
 * @return The number of simultaneous starting points of the gradient descent
 */
std::size_t GBaseGD::getNStartingPoints() const {
	return nStartingPoints_;
}

/******************************************************************************/
/**
 * Allows to set the number of starting points for the gradient descent
 *
 * @param nStartingPoints The desired number of starting points for the gradient descent
 */
void GBaseGD::setNStartingPoints(std::size_t nStartingPoints) {
	// Do some error checking
	if (nStartingPoints == 0) {
		glogger
		<< "In GBaseGD::setNStartingPoints(const std::size_t&):" << std::endl
		<< "Got invalid number of starting points." << std::endl
		<< GEXCEPTION;
	}

	nStartingPoints_ = nStartingPoints;
}

/******************************************************************************/
/**
 * Set the size of the finite step of the adaption process
 *
 * @param finiteStep The desired size of the adaption
 */
void GBaseGD::setFiniteStep(double finiteStep) {
	// Check that finiteStep_ has an appropriate value
	if (finiteStep_ <= 0. || finiteStep_ > 1000.) { // Specified in per mill of the allowed or preferred value range
		glogger
		<< "In GBaseGD::setFiniteStep(double): Error!" << std::endl
		<< "Invalid values of finiteStep_: " << finiteStep_ << std::endl
		<< "Must be in the range ]0.:1000.]" << std::endl
		<< GEXCEPTION;
	}

	finiteStep_ = finiteStep;
}

/******************************************************************************/
/**
 * Retrieve the size of the finite step of the adaption process
 *
 * @return The current finite step size
 */
double GBaseGD::getFiniteStep() const {
	return finiteStep_;
}

/******************************************************************************/
/**
 * Sets a multiplier for the adaption process
 *
 * @param stepSize A multiplicative factor for the adaption process
 */
void GBaseGD::setStepSize(double stepSize) {
	// Check that stepSize_ has an appropriate value
	if (stepSize_ <= 0. || stepSize_ > 1000.) { // Specified in per mill of the allowed or preferred value range
		glogger
		<< "In GBaseGD::setStepSize(double): Error!" << std::endl
		<< "Invalid values of stepSize_: " << stepSize_ << std::endl
		<< "Must be in the range ]0.:1000.]" << std::endl
		<< GEXCEPTION;
	}

	stepSize_ = stepSize;
}

/******************************************************************************/
/**
 * Retrieves the current step size
 *
 * @return The current value of the step size
 */
double GBaseGD::getStepSize() const {
	return stepSize_;
}

/******************************************************************************/
/**
 * Retrieve the number of processable items in the current iteration.
 *
 * @return The number of processable items in the current iteration
 */
std::size_t GBaseGD::getNProcessableItems() const {
	return this->size(); // Evaluation always needs to be done for the entire population
}

/******************************************************************************/
/**
 * Returns the name of this optimization algorithm
 *
 * @return The name assigned to this optimization algorithm
 */
std::string GBaseGD::getAlgorithmName() const {
	return std::string("Gradient Descent");
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
void GBaseGD::compare(
	const GObject &cp, const Gem::Common::expectation &e, const double &limit
) const {
	using namespace Gem::Common;

	// Check that we are dealing with a GBaseGD reference independent of this object and convert the pointer
	const GBaseGD *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseGD>(cp, this);

	GToken token("GBaseGD", e);

	// Compare our parent data ...
	Gem::Common::compare_base<GOptimizationAlgorithmT<GParameterSet>>(IDENTITY(*this, *p_load), token);

	// ... and then the local data
	compare_t(IDENTITY(nStartingPoints_, p_load->nStartingPoints_), token);
	compare_t(IDENTITY(nFPParmsFirst_, p_load->nFPParmsFirst_), token);
	compare_t(IDENTITY(finiteStep_, p_load->finiteStep_), token);
	compare_t(IDENTITY(stepSize_, p_load->stepSize_), token);
	compare_t(IDENTITY(stepRatio_, p_load->stepRatio_), token);
	compare_t(IDENTITY(dblLowerParameterBoundaries_, p_load->dblLowerParameterBoundaries_), token);
	compare_t(IDENTITY(dblUpperParameterBoundaries_, p_load->dblUpperParameterBoundaries_), token);
	compare_t(IDENTITY(adjustedFiniteStep_, p_load->adjustedFiniteStep_), token);

	// React on deviations from the expectation
	token.evaluate();
}

/******************************************************************************/
/**
 * Emits a name for this class / object
 */
std::string GBaseGD::name() const {
	return std::string("GBaseGD");
}

/******************************************************************************/
/**
 * Loads the data of another population
 *
 * @param cp A pointer to another GBaseGD object, camouflaged as a GObject
 */
void GBaseGD::load_(const GObject *cp) {
	// Check that we are dealing with a GBaseGD reference independent of this object and convert the pointer
	const GBaseGD *p_load = Gem::Common::g_convert_and_compare<GObject, GBaseGD>(cp, this);

	// First load the parent class'es data.
	// This will also take care of copying all individuals.
	GOptimizationAlgorithmT<GParameterSet>::load_(cp);

	// ... and then our own data
	nStartingPoints_ = p_load->nStartingPoints_;
	nFPParmsFirst_ = p_load->nFPParmsFirst_;
	finiteStep_ = p_load->finiteStep_;
	stepSize_ = p_load->stepSize_;
	// stepRatio_ = p_load->stepRatio_; // temporary parameter
	// m_dbl_lower_parameter_boundaries = p_load->m_dbl_lower_parameter_boundaries; // temporary parameter
	// m_dbl_upper_parameter_boundaries = p_load->m_dbl_upper_parameter_boundaries; // temporary parameter
	// adjustedFiniteStep_ = p_load->adjustedFiniteStep_; // temporary parameter
}

/******************************************************************************/
/**
 * The actual business logic to be performed during each iteration.
 *
 * @return The value of the best individual found in this iteration
 */
std::tuple<double, double> GBaseGD::cycleLogic() {
	if (afterFirstIteration()) {
		// Update the parameters of the parent individuals. This
		// only makes sense once the individuals have been evaluated
		this->updateParentIndividuals();
	}

	// Update the individual parameters in each dimension of the "children"
	this->updateChildParameters();

	// Trigger value calculation for all individuals (including parents)
	runFitnessCalculation();

	// Perform post-evaluation updates (mostly of individuals)
	postEvaluationWork();

	std::tuple<double, double> bestFitness = std::make_tuple(this->getWorstCase(), this->getWorstCase());
	std::tuple<double, double> fitnessCandidate = std::make_tuple(this->getWorstCase(), this->getWorstCase());

	// Retrieve information about the best fitness found and disallow re-evaluation
	GBaseGD::iterator it;
	for (it = this->begin(); it != this->begin() + this->getNStartingPoints(); ++it) {
		std::get<G_RAW_FITNESS>(fitnessCandidate) = (*it)->fitness(0, PREVENTREEVALUATION, USERAWFITNESS);
		std::get<G_TRANSFORMED_FITNESS>(fitnessCandidate) = (*it)->fitness(0, PREVENTREEVALUATION,
																									USETRANSFORMEDFITNESS);

		if (this->isBetter(
				std::get<G_TRANSFORMED_FITNESS>(fitnessCandidate)
				, std::get<G_TRANSFORMED_FITNESS>(bestFitness)
			)
		) {
			bestFitness = fitnessCandidate;
		}
	}

	return bestFitness;
}

/******************************************************************************/
/**
 * Updates the individual parameters of children
 */
void GBaseGD::updateChildParameters() {
	// Loop over all starting points
	for (std::size_t i = 0; i < nStartingPoints_; i++) {
		// Extract the fp vector
		std::vector<double> parmVec;
		this->at(i)->streamline<double>(parmVec, activityMode::ACTIVEONLY); // Only extract active parameters

		// Loop over all directions
		for (std::size_t j = 0; j < nFPParmsFirst_; j++) {
			// Calculate the position of the child
			std::size_t childPos = nStartingPoints_ + i * nFPParmsFirst_ + j;

			// Load the current "parent" into the "child"
			this->at(childPos)->GObject::load(this->at(i));

			// Update the child's position in the population
			this->at(childPos)->getPersonalityTraits<GGDPersonalityTraits>()->setPopulationPosition(childPos);

			// Make a note of the current parameter's value
			double origParmVal = parmVec[j];

			// Add the finite step to the feature vector's current parameter
			parmVec[j] += adjustedFiniteStep_[j];

			// Attach the feature vector to the child individual
			this->at(childPos)->assignValueVector<double>(parmVec, activityMode::ACTIVEONLY);

			// Restore the original value in the feature vector
			parmVec[j] = origParmVal;
		}
	}
}

/**********************************************************************************************************/
/**
 * Performs a step of the parent individuals.
 * TODO: keep going in the same direction as long as there is an improvement
 */
void GBaseGD::updateParentIndividuals() {
	for (std::size_t i = 0; i < nStartingPoints_; i++) {
		// Extract the fp vector
		std::vector<double> parmVec;
		this->at(i)->streamline<double>(parmVec, activityMode::ACTIVEONLY);

#ifdef DEBUG
		// Make sure the parents are clean
		if(this->at(i)->isDirty()) {
		   glogger
		   << "In GBaseGD::updateParentIndividuals():" << std::endl
         << "Found individual in position " << i << " with active dirty flag" << std::endl
         << GEXCEPTION;
		}
#endif /* DEBUG */

		// Retrieve the fitness of the individual again
		double parentFitness = this->at(i)->minOnly_fitness();

		// Calculate the adaption of each parameter
		// double gradient = 0.;
		for (std::size_t j = 0; j < nFPParmsFirst_; j++) {
			// Calculate the position of the child
			std::size_t childPos = nStartingPoints_ + i * nFPParmsFirst_ + j;

			// Calculate the step to be performed in a given direction and
			// adjust the parameter vector of each parent
			try {
				parmVec[j] -= boost::numeric_cast<double>(
					stepRatio_ *
					((long double) (this->at(childPos)->minOnly_fitness()) - (long double) (parentFitness))
				);
			} catch (boost::bad_numeric_cast &e) {
				glogger
				<< "In GBaseGD::updateParentIndividuals(): Error!" << std::endl
				<< "Bad conversion with message " << e.what() << std::endl
				<< GEXCEPTION;
			}
		}

		// Load the parameter vector back into the parent
		this->at(i)->assignValueVector<double>(parmVec, activityMode::ACTIVEONLY);
	}
}

/******************************************************************************/
/**
 * Adds local configuration options to a GParserBuilder object
 *
 * @param gpb The GParserBuilder object to which configuration options should be added
 */
void GBaseGD::addConfigurationOptions(
	Gem::Common::GParserBuilder &gpb
) {
	// Call our parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::addConfigurationOptions(gpb);

	// Add local data
	gpb.registerFileParameter<std::size_t>(
		"nStartingPoints" // The name of the variable
		, DEFAULTGDSTARTINGPOINTS // The default value
		, [this](std::size_t nsp) { this->setNStartingPoints(nsp); }
	)
	<< "The number of simultaneous gradient descents";

	gpb.registerFileParameter<double>(
		"finiteStep" // The name of the variable
		, DEFAULTFINITESTEP // The default value
		, [this](double fs) { this->setFiniteStep(fs); }
	)
	<< "The size of the adjustment in the difference quotient," << std::endl
	<< "specified in per mill of the allowed or expected value" << std::endl
	<< "range of a parameter";

	gpb.registerFileParameter<double>(
		"stepSize" // The name of the variable
		, DEFAULTSTEPSIZE // The default value
		, [this](double ss) { this->setStepSize(ss); }
	)
	<< "The size of each step into the" << std::endl
	<< "direction of steepest descent," << std::endl
	<< "specified in per mill of the allowed or expected value" << std::endl
	<< "range of a parameter";
}

/******************************************************************************/
/**
 * Does some preparatory work before the optimization starts
 */
void GBaseGD::init() {
	// To be performed before any other action
	GOptimizationAlgorithmT<GParameterSet>::init();

	// Extract the boundaries of all parameters
	this->at(0)->boundaries(dblLowerParameterBoundaries_, dblUpperParameterBoundaries_, activityMode::ACTIVEONLY);

#ifdef DEBUG
   // Size matters!
   if(dblLowerParameterBoundaries_.size() != dblUpperParameterBoundaries_.size()) {
      glogger
      << "In GBaseGD::init(): Error!" << std::endl
      << "Found invalid sizes: "
      << dblLowerParameterBoundaries_.size() << " / " << dblUpperParameterBoundaries_.size() << std::endl
      << GEXCEPTION;
   }

   // Check that stepSize_ has an appropriate value
   if(stepSize_ <= 0. || stepSize_ > 1000.) { // Specified in per mill of the allowed or preferred value range
      glogger
      << "In GBaseGD::init(): Error!" << std::endl
      << "Invalid values of stepSize_: " << stepSize_ << std::endl
      << "Must be in the range ]0.:1000.]" << std::endl
      << GEXCEPTION;
   }

   // Check that finiteStep_ has an appropriate value
   if(finiteStep_ <= 0. || finiteStep_ > 1000.) { // Specified in permille of the allowed or preferred value range
      glogger
      << "In GBaseGD::init(): Error!" << std::endl
      << "Invalid values of finiteStep_: " << finiteStep_ << std::endl
      << "Must be in the range ]0.:1000.]" << std::endl
      << GEXCEPTION;
   }
#endif /* DEBUG */

	// Set the step ratio. We do the calculation in long double precision to preserve accuracy
	stepRatio_ = ((long double) stepSize_) / ((long double) finiteStep_);

	// Calculate a specific finiteStep_ value for each parameter in long double precision
	try {
		adjustedFiniteStep_.clear();
		long double finiteStepRatio = ((long double) finiteStep_) / ((long double) 1000.);
		for (std::size_t pos = 0; pos < dblLowerParameterBoundaries_.size(); pos++) {
			long double parameterRange =
				(long double) dblUpperParameterBoundaries_[pos] - (long double) dblLowerParameterBoundaries_[pos];
			adjustedFiniteStep_.push_back(boost::numeric_cast<double>(finiteStepRatio * parameterRange));
		}
	} catch (boost::bad_numeric_cast &e) {
		glogger
		<< "In GBaseGD::init(): Error!" << std::endl
		<< "Bad conversion with message " << e.what() << std::endl
		<< GEXCEPTION;
	}

	// Tell individuals about their position in the population
	markIndividualPositions();
}

/******************************************************************************/
/**
 * Does any necessary finalization work
 */
void GBaseGD::finalize() {
	// Last action
	GOptimizationAlgorithmT<GParameterSet>::finalize();
}

/******************************************************************************/
/**
 * Retrieve a GPersonalityTraits object belonging to this algorithm
 */
std::shared_ptr <GPersonalityTraits> GBaseGD::getPersonalityTraits() const {
	return std::shared_ptr<GGDPersonalityTraits>(new GGDPersonalityTraits());
}

/******************************************************************************/
/**
 * Resizes the population to the desired level and does some error checks.
 */
void GBaseGD::adjustPopulation() {
	// Check how many individuals we already have
	std::size_t nStart = this->size();

	// Do some error checking ...

	// We need at least one individual
	if (nStart == 0) {
		glogger
		<< "In GBaseGD::adjustPopulation():" << std::endl
		<< "You didn't add any individuals to the collection. We need at least one." << std::endl
		<< GEXCEPTION;
	}

	// Update the number of active floating point parameters in the individuals
	nFPParmsFirst_ = this->at(0)->countParameters<double>(activityMode::ACTIVEONLY);

	// Check that the first individual has floating point parameters (double for the moment)
	if (nFPParmsFirst_ == 0) {
		glogger
		<< "In GBaseGD::adjustPopulation():" << std::endl
		<< "No floating point parameters in individual." << std::endl
		<< GEXCEPTION;
	}

	// Check that all individuals currently available have the same amount of parameters
#ifdef DEBUG
	for(std::size_t i=1; i<this->size(); i++) {
		if(this->at(i)->countParameters<double>(activityMode::ACTIVEONLY) != nFPParmsFirst_) {
		   glogger
		   << "In GBaseGD::adjustPopulation():" << std::endl
         << "Found individual in position " <<  i << " with different" << std::endl
         << "number of floating point parameters than the first one: " << this->at(i)->countParameters<double>(activityMode::ACTIVEONLY) << "/" << nFPParmsFirst_ << std::endl
         << GEXCEPTION;
		}
	}
#endif

	// Set the default size of the population
	GOptimizationAlgorithmT<GParameterSet>::setDefaultPopulationSize(nStartingPoints_ * (nFPParmsFirst_ + 1));

	// First create a suitable number of start individuals and initialize them as required
	if (nStart < nStartingPoints_) {
		for (std::size_t i = 0; i < (nStartingPoints_ - nStart); i++) {
			// Create a copy of the first individual
			this->push_back(this->at(0)->clone<GParameterSet>());
			// Make sure our start values differ
			this->back()->randomInit(activityMode::ACTIVEONLY);
		}
	} else {
		// Start with a defined size. This will remove surplus items.
		this->resize(nStartingPoints_);
	}

	// Add the required number of clones for each starting point. These will be
	// used for the calculation of the difference quotient for each parameter
	for (std::size_t i = 0; i < nStartingPoints_; i++) {
		for (std::size_t j = 0; j < nFPParmsFirst_; j++) {
			this->push_back(this->at(i)->clone<GParameterSet>());
		}
	}

	// We now should have nStartingPoints_ sets of individuals,
	// each of size nFPParmsFirst_.
#ifdef DEBUG
	if(this->size() != nStartingPoints_*(nFPParmsFirst_ + 1)) {
	   glogger
	   << "In GBaseGD::adjustPopulation():" << std::endl
      << "Population size is " << this->size() << std::endl
      << "but expected " << nStartingPoints_*(nFPParmsFirst_ + 1) << std::endl
      << GEXCEPTION;
	}
#endif /* DEBUG */
}

/******************************************************************************/
/**
 * This helper function lets all individuals know about their position in the
 * population.
 */
void GBaseGD::markIndividualPositions() {
	for (std::size_t pos = 0; pos < this->size(); pos++) {
		this->at(pos)->getPersonalityTraits<GGDPersonalityTraits>()->setPopulationPosition(pos);
	}
}

/******************************************************************************/
/**
 * Applies modifications to this object. This is needed for testing purposes
 */
bool GBaseGD::modify_GUnitTests() {
#ifdef GEM_TESTING
	bool result = false;

	// Call the parent class'es function
	if (GOptimizationAlgorithmT<GParameterSet>::modify_GUnitTests()) result = true;

	return result;
#else /* GEM_TESTING */
   condnotset("GBaseGD::modify_GUnitTests", "GEM_TESTING");
   return false;
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to succeed. This is needed for testing purposes
 */
void GBaseGD::specificTestsNoFailureExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::specificTestsNoFailureExpected_GUnitTests();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GBaseGD::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/
/**
 * Performs self tests that are expected to fail. This is needed for testing purposes
 */
void GBaseGD::specificTestsFailuresExpected_GUnitTests() {
#ifdef GEM_TESTING
	// Call the parent class'es function
	GOptimizationAlgorithmT<GParameterSet>::specificTestsFailuresExpected_GUnitTests();
#else /* GEM_TESTING */ // If this function is called when GEM_TESTING isn't set, throw
   condnotset("GBaseGD::specificTestsNoFailureExpected_GUnitTests", "GEM_TESTING");
#endif /* GEM_TESTING */
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */


