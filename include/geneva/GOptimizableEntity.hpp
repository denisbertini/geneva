/**
 * @file GOptimizableEntity.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard header files go here
#include <typeinfo>

// Boost header files go here
#include <boost/limits.hpp>
#include <boost/numeric/conversion/bounds.hpp>

#ifndef GOPTIMIZABLEENTITY_HPP_
#define GOPTIMIZABLEENTITY_HPP_

// Geneva header files go here
#include "common/GExceptions.hpp"
#include "common/GHelperFunctionsT.hpp"
#include "common/GMathHelperFunctions.hpp"
#include "common/GLockVarT.hpp"
#include "geneva/GenevaHelperFunctionsT.hpp"
#include "geneva/GObject.hpp"
#include "geneva/GPersonalityTraits.hpp"
#include "geneva/GMutableI.hpp"
#include "geneva/GRateableI.hpp"
#include "geneva/GPersonalityTraits.hpp"
#include "geneva/GMultiConstraintT.hpp"

namespace Gem {
namespace Tests {
class GTestIndividual1; // forward declaration, needed for test purposes
} /* namespace Tests */
} /* namespace Gem */

namespace Gem {
namespace Geneva {

// Forward declarations
class GSerialSwarm;

/******************************************************************************/
/**
 * This class acts as an interface class for all objects that can take part
 * in an evolutionary improvement. Such items must possess adaption functionality
 * and must know how to calculate their fitness. They also need the basic GObject
 * interface. In particular, they absolutely need to be serializable. As this library
 * was designed with particularly expensive evaluation calculations in mind, this
 * class also contains a framework for lazy evaluation, so not all evaluations take
 * place at the same time.
 */
class GOptimizableEntity
	: public GMutableI
	, public GRateableI
	, public GObject
{
	friend class GSerialSwarm; ///< Needed so GSerialSwarm can set the dirty flag
	friend class Gem::Tests::GTestIndividual1; ///< Needed for testing purposes

	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar
	  & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GObject)
	  & BOOST_SERIALIZATION_NVP(currentFitnessVec_)
	  & BOOST_SERIALIZATION_NVP(nFitnessCriteria_)
	  & BOOST_SERIALIZATION_NVP(bestPastPrimaryFitness_)
	  & BOOST_SERIALIZATION_NVP(nStalls_)
	  & BOOST_SERIALIZATION_NVP(dirtyFlag_)
	  & BOOST_SERIALIZATION_NVP(maximize_)
	  & BOOST_SERIALIZATION_NVP(assignedIteration_)
	  & BOOST_SERIALIZATION_NVP(validityLevel_)
	  & BOOST_SERIALIZATION_NVP(pt_ptr_)
	  & BOOST_SERIALIZATION_NVP(evalPolicy_)
	  & BOOST_SERIALIZATION_NVP(individualConstraint_)
	  & BOOST_SERIALIZATION_NVP(steepness_)
	  & BOOST_SERIALIZATION_NVP(barrier_)
	  & BOOST_SERIALIZATION_NVP(worstKnownValids_)
	  & BOOST_SERIALIZATION_NVP(markedAsInvalidByUser_)
	  & BOOST_SERIALIZATION_NVP(maxUnsuccessfulAdaptions_)
	  & BOOST_SERIALIZATION_NVP(maxRetriesUntilValid_)
	  & BOOST_SERIALIZATION_NVP(nAdaptions_)
	  & BOOST_SERIALIZATION_NVP(evaluationID_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	G_API_GENEVA GOptimizableEntity();
	/** @brief The copy constructor */
	G_API_GENEVA GOptimizableEntity(const GOptimizableEntity&);
	/** @brief Initialization with the number of fitness criteria */
	G_API_GENEVA GOptimizableEntity(const std::size_t&);
	/** @brief The destructor */
	virtual G_API_GENEVA ~GOptimizableEntity();

   /** @brief The standard assignment operator */
   G_API_GENEVA const GOptimizableEntity& operator=(const GOptimizableEntity&);

   /** @brief Checks for equality with another GOptimizableEntity object */
   virtual G_API_GENEVA bool operator==(const GOptimizableEntity&) const;
   /** @brief Checks for inequality with another GOptimizableEntity object */
   virtual G_API_GENEVA bool operator!=(const GOptimizableEntity&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual G_API_GENEVA boost::optional<std::string> checkRelationshipWith(
      const GObject&
      , const Gem::Common::expectation&
      , const double&
      , const std::string&
      , const std::string&
      , const bool&
	) const OVERRIDE;

   /** @brief Searches for compliance with expectations with respect to another object of the same type */
   virtual G_API_GENEVA void compare(
      const GObject& // the other object
      , const Gem::Common::expectation& // the expectation for this object, e.g. equality
      , const double& // the limit for allowed deviations of floating point types
   ) const OVERRIDE;

	/** @brief The adaption interface */
	virtual G_API_GENEVA std::size_t adapt() OVERRIDE;

   /** @brief Returns the raw result of the fitness function with id 0 */
   virtual G_API_GENEVA double fitness() const OVERRIDE;
   /** @brief Returns the raw result of a fitness function with a given id */
   virtual G_API_GENEVA double fitness(const std::size_t&) const OVERRIDE;

   /** @brief Calculate or returns the result of a fitness function with a given id */
   virtual G_API_GENEVA double fitness(const std::size_t&, bool, bool) OVERRIDE;
   /** @brief Calculate or returns the result of a fitness function with a given id */
   virtual G_API_GENEVA double fitness(const std::size_t&, bool, bool) const OVERRIDE;

   /** @brief Returns the transformed result of the fitness function with id 0 */
   virtual G_API_GENEVA double transformedFitness() const OVERRIDE;
   /** @brief Returns the transformed result of a fitness function with a given id */
   virtual G_API_GENEVA double transformedFitness(const std::size_t&) const OVERRIDE;

   /** @brief Returns a fitness targetted at optimization algorithms, taking into account maximization and minimization */
   virtual G_API_GENEVA double minOnly_fitness() const OVERRIDE;
   /** @brief Returns a fitness targetted at optimization algorithms, taking into account maximization and minimization */
   virtual G_API_GENEVA double minOnly_fitness(const std::size_t&) const OVERRIDE;

   /** @brief Returns all raw fitness results in a std::vector */
   virtual G_API_GENEVA std::vector<double> fitnessVec() const OVERRIDE;
   /** @brief Returns all raw or transformed results in a std::vector */
   virtual G_API_GENEVA std::vector<double> fitnessVec(bool) const OVERRIDE;
   /** @brief Returns all transformed fitness results in a std::vector */
   virtual G_API_GENEVA std::vector<double> transformedFitnessVec() const OVERRIDE;

   /** @brief A wrapper for the non-const fitness function, so we can bind to it */
   G_API_GENEVA double nonConstFitness(const std::size_t&, bool, bool);
   /** @brief A wrapper for the const fitness function, so we can bind to it */
   G_API_GENEVA double constFitness(const std::size_t&, bool, bool) const;

	/** @brief Adapts and evaluates the individual in one go */
	virtual G_API_GENEVA void adaptAndEvaluate();

   /** @brief Retrieve the current (not necessarily up-to-date) fitness */
	G_API_GENEVA double getCachedFitness(const std::size_t& = 0, const bool& = USETRANSFORMEDFITNESS) const;

	/** @brief Enforce fitness (re-)calculation */
	G_API_GENEVA void enforceFitnessUpdate(boost::function<std::vector<double>()> =  boost::function<std::vector<double>()>());

	/** @brief Registers a new, secondary result value of the custom fitness calculation */
	G_API_GENEVA void registerSecondaryResult(const std::size_t&, const double&);
	/** @brief Determines the overall number of fitness criteria present for this individual */
	G_API_GENEVA std::size_t getNumberOfFitnessCriteria() const;
	/** @brief Allows to reset the number of fitness criteria */
	G_API_GENEVA void setNumberOfFitnessCriteria(std::size_t);
	/** @brief Determines whether more than one fitness criterion is present for this individual */
	G_API_GENEVA bool hasMultipleFitnessCriteria() const;

	/** @brief Checks the worst fitness and updates it when needed */
	G_API_GENEVA void challengeWorstValidFitness(boost::tuple<double, double>&, const std::size_t&);
   /** @brief Retrieve the fitness tuple at a given evaluation position */
	G_API_GENEVA boost::tuple<double,double> getFitnessTuple(const boost::uint32_t& = 0) const;

   /** @brief Check whether this individual is "clean", i.e neither "dirty" nor has a delayed evaluation */
	G_API_GENEVA bool isClean() const;
	/** @brief Check whether the dirty flag is set */
	G_API_GENEVA bool isDirty() const ;
   /** @brief Sets the dirtyFlag_ */
	G_API_GENEVA void setDirtyFlag();
   /** @brief Checks whether evaluation was delayed */
	G_API_GENEVA bool evaluationDelayed() const;

	/** @brief Allows to retrieve the maximize_ parameter */
	G_API_GENEVA bool getMaxMode() const;

	/** @brief Retrieves the worst possible evaluation result, depending on whether we are in maximization or minimization mode */
	virtual G_API_GENEVA double getWorstCase() const BASE;
   /** @brief Retrieves the best possible evaluation result, depending on whether we are in maximization or minimization mode */
   virtual G_API_GENEVA double getBestCase() const BASE;

	/** @brief Retrieves the steepness_ variable (used for the sigmoid transformation) */
   G_API_GENEVA double getSteepness() const;
	/** @brief Sets the steepness variable (used for the sigmoid transformation) */
   G_API_GENEVA void setSteepness(double);

	/** @brief Retrieves the barrier_ variable (used for the sigmoid transformation) */
   G_API_GENEVA double getBarrier() const;
	/** @brief Sets the barrier variable (used for the sigmoid transformation) */
   G_API_GENEVA void setBarrier(double);

	/** @brief Sets the maximum number of calls to customAdaptions() that may pass without actual modifications */
   G_API_GENEVA void setMaxUnsuccessfulAdaptions(std::size_t);
	/** @brief Retrieves the maximum number of calls to customAdaptions that may pass without actual modifications */
   G_API_GENEVA std::size_t getMaxUnsuccessfulAdaptions() const;

   /** @brief Set maximum number of retries until a valid individual was found  */
   G_API_GENEVA void setMaxRetriesUntilValid(std::size_t maxRetriesUntilValid);
   /** Retrieves the maximum number of retries until a valid individual was found. */
   G_API_GENEVA std::size_t getMaxRetriesUntilValid() const;

	/** @brief Retrieves the number of adaptions performed during the last call to adapt() */
   G_API_GENEVA std::size_t getNAdaptions() const;

	/** @brief Allows to set the current iteration of the parent optimization algorithm. */
   G_API_GENEVA void setAssignedIteration(const boost::uint32_t&);
	/** @brief Gives access to the parent optimization algorithm's iteration */
   G_API_GENEVA boost::uint32_t getAssignedIteration() const;

	/** @brief Allows to specify the number of optimization cycles without improvement of the primary fitness criterion */
   G_API_GENEVA void setNStalls(const boost::uint32_t&);
	/** @brief Allows to retrieve the number of optimization cycles without improvement of the primary fitness criterion */
   G_API_GENEVA boost::uint32_t getNStalls() const;

	/** @brief Retrieves an identifier for the current personality of this object */
   G_API_GENEVA std::string getPersonality() const;

	/** @brief Allows to randomly initialize parameter members */
	virtual G_API_GENEVA void randomInit(const activityMode&) BASE = 0;

	/** @brief Retrieves a parameter of a given type at the specified position */
	virtual G_API_GENEVA boost::any getVarVal(
      const std::string&
      , const boost::tuple<std::size_t, std::string, std::size_t>& target
   ) = 0;

   /***************************************************************************/
   /**
    * Retrieves a parameter of a given type at the specified position. Note: This
    * function is a trap. Use one of the overloads for supported types.
    */
   template <typename val_type>
   val_type getVarVal(
      const boost::tuple<std::size_t, std::string, std::size_t>& target
   ) {
      val_type result = val_type(0);

      if(typeid(val_type) == typeid(double)) {
         return boost::numeric_cast<val_type>(boost::any_cast<double>(this->getVarVal("d", target)));
      } else if(typeid(val_type) == typeid(float)) {
         return boost::numeric_cast<val_type>(boost::any_cast<float>(this->getVarVal("f", target)));
      } if(typeid(val_type) == typeid(boost::int32_t)) {
         return boost::numeric_cast<val_type>(boost::any_cast<boost::int32_t>(this->getVarVal("i", target)));
      } if(typeid(val_type) == typeid(bool)) {
         return boost::numeric_cast<val_type>(boost::any_cast<bool>(this->getVarVal("b", target)));
      } else {
         glogger
         << "In GOptimizableEntity::getVarVal<>(): Error!" << std::endl
         << "Received invalid type descriptor " << std::endl
         << GEXCEPTION;
      }

      return result;
   }

	/***************************************************************************/
	/**
	 * The function converts the local personality base pointer to the desired type
	 * and returns it for modification by the corresponding optimization algorithm.
	 * The base algorithms have been declared "friend" of GParameterSet and
	 * can thus access this function. External entities have no need to do so. Note
	 * that this function will only be accessible to the compiler if personality_type
	 * is a derivative of GPersonalityTraits, thanks to the magic of Boost's
	 * enable_if and Type Traits libraries.
	 *
	 * @return A boost::shared_ptr converted to the desired target type
	 */
	template <typename personality_type>
	boost::shared_ptr<personality_type> getPersonalityTraits(
			typename boost::enable_if<boost::is_base_of<GPersonalityTraits, personality_type> >::type* dummy = 0
	) {
#ifdef DEBUG
      // Check that pt_ptr_ actually points somewhere
      if(!pt_ptr_) {
         glogger
         << "In GOptimizableEntity::getPersonalityTraits<personality_type>() : Empty personality pointer found" << std::endl
         << "This should not happen." << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return boost::shared_ptr<personality_type>();
      }
#endif /* DEBUG */

	   // Does error checks on the conversion internally
      return Gem::Common::convertSmartPointer<GPersonalityTraits, personality_type>(pt_ptr_);
	}

	/* ----------------------------------------------------------------------------------
	 * Tested in GOptimizableEntity::specificTestsNoFailureExpected_GUnitTests()
	 * Tested in GOptimizableEntity::specificTestsFailureExpected_GUnitTests()
	 * ----------------------------------------------------------------------------------
	 */

	/***************************************************************************/
	/** @brief This function returns the current personality traits base pointer */
	G_API_GENEVA boost::shared_ptr<GPersonalityTraits> getPersonalityTraits();

	/** @brief Sets the current personality of this individual */
	G_API_GENEVA void setPersonality(
	      boost::shared_ptr<GPersonalityTraits>
	);
	/** @brief Resets the current personality to PERSONALITY_NONE */
	G_API_GENEVA void resetPersonality();

	/** @brief Adds local configuration options to a GParserBuilder object */
	virtual G_API_GENEVA void addConfigurationOptions(
		Gem::Common::GParserBuilder&
	) OVERRIDE;

	/** @brief Allows to assign a name to the role of this individual(-derivative) */
	virtual G_API_GENEVA std::string getIndividualCharacteristic() const = 0;

   /** @brief Emits a name for this class / object */
   virtual G_API_GENEVA std::string name() const OVERRIDE;

   /** @brief Check how valid a given solution is */
   G_API_GENEVA double getValidityLevel() const;
   /** @brief Checks whether all constraints were fulfilled */
   G_API_GENEVA bool constraintsFulfilled() const;
   /** @brief Allows to register a constraint with this individual */
   G_API_GENEVA void registerConstraint(boost::shared_ptr<GPreEvaluationValidityCheckT<GOptimizableEntity> >);

   /** @brief Allows to set the policy to use in case this individual represents an invalid solution */
   G_API_GENEVA void setEvaluationPolicy(evaluationPolicy evalPolicy);
   /** @brief Allows to retrieve the current policy in case this individual represents an invalid solution */
   G_API_GENEVA evaluationPolicy getEvaluationPolicy() const;

   /** @brief Checks whether this is a valid solution; meant to be called for "clean" individuals only */
   G_API_GENEVA bool isValid() const;
   /** @brief Checks whether this solution is invalid */
   G_API_GENEVA bool isInValid() const;

   /** @brief Allows an optimization algorithm to set the worst known valid evaluation up to the current iteration */
   G_API_GENEVA void setWorstKnownValid(const std::vector<boost::tuple<double, double> >&);
   /** @brief Allows to retrieve the worst known valid evaluation up to the current iteration, as set by an external optimization algorithm */
   G_API_GENEVA boost::tuple<double, double> getWorstKnownValid(const boost::uint32_t&) const;
   /** @brief Allows to retrieve all worst known valid evaluations up to the current iteration, as set by an external optimization algorithm */
   G_API_GENEVA std::vector<boost::tuple<double, double> > getWorstKnownValids() const;
   /** @brief Fills the worstKnownValid-vector with best values */
   G_API_GENEVA void populateWorstKnownValid();

   /** @brief Triggers an update of the internal evaluation, if necessary */
   G_API_GENEVA void postEvaluationUpdate();

   /** @brief Allows to set the globally best known primary fitness */
   G_API_GENEVA void setBestKnownPrimaryFitness(const boost::tuple<double, double>&);
   /** @brief Retrieves the value of the globally best known primary fitness */
   G_API_GENEVA boost::tuple<double, double> getBestKnownPrimaryFitness() const;

   /** @brief Retrieve the id assigned to the current evaluation */
   G_API_GENEVA std::string getCurrentEvaluationID() const;

   /** @brief Checks whether a new solution is worse then an older solution, depending on the maxMode */
   virtual G_API_GENEVA bool isWorse(double, const double&) const BASE;
   /** @brief Checks whether a new solution is better then an older solution, depending on the maxMode */
   virtual G_API_GENEVA bool isBetter(double, const double&) const BASE;

   /** @brief Checks whether this object is better than the argument, depending on the maxMode */
   G_API_GENEVA bool isBetterThan(boost::shared_ptr<GOptimizableEntity>) const;
   /** @brief Checks whether this object is worse than the argument, depending on the maxMode */
   G_API_GENEVA bool isWorseThan(boost::shared_ptr<GOptimizableEntity>) const;

protected:
	/***************************************************************************/
	/** @brief Loads the data of another GOptimizableEntity */
	virtual G_API_GENEVA void load_(const GObject*) OVERRIDE;
	/** @brief Creates a deep clone of this object */
	virtual G_API_GENEVA GObject* clone_() const = 0;

	/** @brief The fitness calculation for the main quality criterion takes place here */
	virtual G_API_GENEVA double fitnessCalculation() BASE = 0;
	/** @brief Sets the fitness to a given set of values and clears the dirty flag */
	G_API_GENEVA void setFitness_(const std::vector<double>&);

	/** @brief The actual adaption operations */
	virtual G_API_GENEVA std::size_t customAdaptions() BASE;
	/** @brief Specify whether we want to work in maximization (true) or minimization (false) mode */
	G_API_GENEVA void setMaxMode_(const bool&);
	/** @brief Sets the dirtyFlag_ to any desired value */
	G_API_GENEVA boost::logic::tribool setDirtyFlag(const boost::logic::tribool&) ;

	/** @brief Combines secondary evaluation results by adding the individual results */
	G_API_GENEVA double sumCombiner() const;
	/** @brief Combines secondary evaluation results by adding the absolute values of individual results */
	G_API_GENEVA double fabsSumCombiner() const;
	/** @brief Combines secondary evaluation results by calculating the square root of the squared sum */
	G_API_GENEVA double squaredSumCombiner() const;
	/** @brief Combines secondary evaluation results by calculation the square root of the weighed squared sum */
	G_API_GENEVA double weighedSquaredSumCombiner(const std::vector<double>&) const;

	/** @brief Allows users to mark this solution as invalid in derived classes (usually from within the evaluation function) */
	G_API_GENEVA void markAsInvalid();
	/** @brief Allows to check whether this solution was marked as invalid */
	G_API_GENEVA bool markedAsInvalidByUser() const;

   /** @brief Checks whether this solution has been rated to be valid; meant to be called by internal functions only */
	G_API_GENEVA bool parameterSetFulfillsConstraints(double&) const;

	/***************************************************************************/
	/**
	 * Checks if a given position of a boost::tuple is better then another,
	 * depending on our maximization mode
	 */
	template <std::size_t pos>
	bool isWorse(
      boost::tuple<double, double> newValue
      , boost::tuple<double, double> oldValue
   ) const {
	   if(this->getMaxMode()) {
	      if(boost::get<pos>(newValue) < boost::get<pos>(oldValue)) return true;
	      else return false;
	   } else { // minimization
	      if(boost::get<pos>(newValue) > boost::get<pos>(oldValue)) return true;
	      else return false;
	   }
	}

   /***************************************************************************/
   /**
    * Checks if a given position of a boost::tuple is better then another,
    * depending on our maximization mode
    */
   template <std::size_t pos>
   bool isBetter(
      boost::tuple<double, double> newValue
      , boost::tuple<double, double> oldValue
   ) const {
      if(this->getMaxMode()) {
         if(boost::get<pos>(newValue) > boost::get<pos>(oldValue)) return true;
         else return false;
      } else { // minimization
         if(boost::get<pos>(newValue) < boost::get<pos>(oldValue)) return true;
         else return false;
      }
   }

private:
   /***************************************************************************/
   /** @brief Checks whether all results are at the worst possible value */
   bool allRawResultsAtWorst() const;

	/***************************************************************************/
   /** @brief The total number of fitness criteria */
   std::size_t nFitnessCriteria_;
   /** @brief Holds this object's internal, raw and transformed fitness */
   std::vector<boost::tuple<double, double> > currentFitnessVec_;

   /** @brief The worst known evaluation up to the current iteration */
   std::vector<boost::tuple<double, double> > worstKnownValids_;
   /** @brief Indicates whether the user has marked this solution as invalid inside of the evaluation function */
   Gem::Common::GLockVarT<bool> markedAsInvalidByUser_;

   /** @brief Holds the globally best known primary fitness of all individuals */
   boost::tuple<double, double> bestPastPrimaryFitness_;
   /** @brief The number of stalls of the primary fitness criterion in the entire set of individuals */
   boost::uint32_t nStalls_;
   /** @brief Internal representation of the adaption status of this object */
   boost::logic::tribool dirtyFlag_; // boost::logic::indeterminate refers to "delayed evaluation"
   /** @brief Indicates whether we are running in maximization or minimization mode */
   bool maximize_;
   /** @brief The iteration of the parent algorithm's optimization cycle */
   boost::uint32_t assignedIteration_;
   /** @brief Indicates how valid a given solution is */
   double validityLevel_;
   /** @brief Holds the actual personality information */
   boost::shared_ptr<GPersonalityTraits> pt_ptr_;

   /** @brief Specifies what to do when the individual is marked as invalid */
   evaluationPolicy evalPolicy_;
   /** @brief Determines the "steepness" of a sigmoid function used by optimization algorithms */
   double steepness_;
   /** @brief Determines the extreme values of a sigmoid function used by optimization algorithms */
   double barrier_;

   /** @brief A constraint-check to be applied to one or more components of this individual */
   boost::shared_ptr<GPreEvaluationValidityCheckT<GOptimizableEntity> > individualConstraint_;

   std::size_t maxUnsuccessfulAdaptions_; ///< The maximum number of calls to customAdaptions() in a row without actual modifications
   std::size_t maxRetriesUntilValid_; ///< The maximum number an adaption of an individual should be performed until a valid parameter set was found
   std::size_t nAdaptions_; ///< Stores the actual number of adaptions after a call to "adapt()"

   /** @brief A unique id that is assigned to an evaluation */
   std::string evaluationID_;

public:
   /***************************************************************************/
   /** @brief Applies modifications to this object. This is needed for testing purposes */
   virtual G_API_GENEVA bool modify_GUnitTests() OVERRIDE;
   /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
   virtual G_API_GENEVA void specificTestsNoFailureExpected_GUnitTests() OVERRIDE;
   /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
   virtual G_API_GENEVA void specificTestsFailuresExpected_GUnitTests() OVERRIDE;
};

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/
/**
 * @brief Needed for Boost.Serialization
 */
BOOST_SERIALIZATION_ASSUME_ABSTRACT(Gem::Geneva::GOptimizableEntity)

/******************************************************************************/

#endif /* GOPTIMIZABLEENTITY_HPP_ */
