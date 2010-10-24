/**
 * @file GOptimizer.hpp
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

// Standard header files go here
#include <vector>
#include <fstream>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost header files go here
#include <boost/utility.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/cast.hpp>

#ifndef GOPTIMIZER_HPP_
#define GOPTIMIZER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here
#include "geneva/GMutableSetT.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva/GMutableSetT.hpp"
#include "geneva/GParameterSet.hpp"
#include "geneva/GEvolutionaryAlgorithm.hpp"
#include "geneva/GMultiThreadedEA.hpp"
#include "geneva/GBrokerEA.hpp"
#include "geneva/GSwarm.hpp"
#include "geneva/GMultiThreadedSwarm.hpp"
#include "geneva/GBrokerSwarm.hpp"
#include "geneva/GGradientDescent.hpp"
#include "geneva/GMultiThreadedGD.hpp"
#include "geneva/GenevaHelperFunctionsT.hpp"
#include "common/GExceptions.hpp"
#include "hap/GRandomT.hpp"
#include "courtier/GAsioTCPConsumerT.hpp"
#include "courtier/GAsioTCPClientT.hpp"

namespace Gem {
namespace Geneva {

/**************************************************************************************/
// Default values for the variables used by the optimizer
const personality GO_DEF_PERSONALITY=EA;
const parMode GO_DEF_PARALLELIZATIONMODE=MULTITHREADED;
const std::string GO_DEF_DEFAULTCONFIGFILE="optimizationAlgorithm.cfg";
const bool GO_DEF_SERVERMODE=true;
const parMode GO_DEF_DEFAULPARALLELIZATIONMODE=MULTITHREADED;
const Gem::Common::serializationMode GO_DEF_DEFAULTSERIALIZATIONMODE=Gem::Common::SERIALIZATIONMODE_BINARY;
const std::string GO_DEF_IP="localhost";
const unsigned int GO_DEF_PORT=10000;
const bool GO_DEF_DEFAULTVERBOSE=false;
const bool GO_DEF_COPYBESTONLY=true;
const boost::uint16_t GO_DEF_MAXSTALLED=0;
const boost::uint16_t GO_DEF_MAXCONNATT=100;
const bool GO_DEF_RETURNREGARDLESS=true;
const boost::uint16_t GO_DEF_NPRODUCERTHREADS=0;
const std::size_t GO_DEF_ARRAYSIZE=1000;
const boost::uint16_t GO_DEF_NEVALUATIONTHREADS=0;
const Gem::Common::serializationMode GO_DEF_SERIALIZATIONMODE=Gem::Common::SERIALIZATIONMODE_TEXT;
const boost::uint32_t GO_DEF_WAITFACTOR=0;
const boost::uint32_t GO_DEF_MAXITERATIONS=1000;
const long GO_DEF_MAXMINUTES=0;
const boost::uint32_t GO_DEF_REPORTITERATION=1;
const std::size_t GO_DEF_EAPOPULATIONSIZE=100;
const std::size_t GO_DEF_EANPARENTS=1;
const recoScheme GO_DEF_EARECOMBINATIONSCHEME=VALUERECOMBINE;
const sortingMode GO_DEF_EASORTINGSCHEME=MUCOMMANU;
const bool GO_DEF_EATRACKPARENTRELATIONS=false;
const std::size_t GO_DEF_SWARMNNEIGHBORHOODS=5;
const std::size_t GO_DEF_SWARMNNEIGHBORHOODMEMBERS=10;
const bool GO_DEF_SWARMRANDOMFILLUP=1;
const float GO_DEF_SWARMCLOCAL=2.;
const float GO_DEF_SWARMCCGLOBAL=2.;
const float GO_DEF_SWARMCCDELTA=0.4;
const updateRule GO_DEF_SWARMUPDATERULE=CLASSIC;
const std::size_t GO_DEF_GDNSTARTINGPOINTS=1;
const float GO_DEF_GDFINITESTEP=0.01;
const float GO_DEF_GDSTEPSIZE=0.1;

/**************************************************************************************/
/**
 * This class acts as a wrapper around the various optimization algorithms in the
 * Geneva library. Its aim is to facilitate the usage of the various algorithms,
 * relieving users from having to write any other code than is needed by their parameter
 * descriptions. The class parses a configuration file covering the most common options of
 * the various optimization algorithms. The class will not touch the command line. The
 * user can make the name of a configuration file known to the class. If none is provided,
 * the class will attempt to load the data from a default file name.
 */
class GOptimizer
	: public GMutablesetT<GParameterSet>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GIndividual)
		 & make_nvp("GMutableSetT_GParameterSet", boost::serialization::base_object<GMutaböeSetT<GParameterSet> >(*this))
		 & BOOST_SERIALIZATION_NVP(pers_);
	  	 & BOOST_SERIALIZATION_NVP(parMode_)
	     & BOOST_SERIALIZATION_NVP(serverMode_)
	     & BOOST_SERIALIZATION_NVP(serializationMode_)
	     & BOOST_SERIALIZATION_NVP(ip_)
	     & BOOST_SERIALIZATION_NVP(port_)
	     & BOOST_SERIALIZATION_NVP(configFilename_)
	     & BOOST_SERIALIZATION_NVP(verbose_)
	     & BOOST_SERIALIZATION_NVP(ea_om_ptr_)
	     & BOOST_SERIALIZATION_NVP(swarm_om_ptr_)
	     & BOOST_SERIALIZATION_NVP(gd_om_ptr_)
	     & BOOST_SERIALIZATION_NVP(copyBestOnly_)
	     & BOOST_SERIALIZATION_NVP(maxStalledDataTransfers_)
	     & BOOST_SERIALIZATION_NVP(maxConnectionAttempts_)
	     & BOOST_SERIALIZATION_NVP(returnRegardless_)
	     & BOOST_SERIALIZATION_NVP(nProducerThreads_)
	     & BOOST_SERIALIZATION_NVP(arraySize_)
	     & BOOST_SERIALIZATION_NVP(nEvaluationThreads_)
	     & BOOST_SERIALIZATION_NVP(serializationMode_)
	     & BOOST_SERIALIZATION_NVP(waitFactor_)
	     & BOOST_SERIALIZATION_NVP(maxIterations_)
	     & BOOST_SERIALIZATION_NVP(maxMinutes_)
	     & BOOST_SERIALIZATION_NVP(reportIteration_)
	     & BOOST_SERIALIZATION_NVP(eaPopulationSize_)
	     & BOOST_SERIALIZATION_NVP(eaNParents_)
	     & BOOST_SERIALIZATION_NVP(eaRecombinationScheme_)
	     & BOOST_SERIALIZATION_NVP(eaSortingScheme_)
	     & BOOST_SERIALIZATION_NVP(eaTrackParentRelations_)
	     & BOOST_SERIALIZATION_NVP(swarmNNeighborhoods_)
	     & BOOST_SERIALIZATION_NVP(swarmNNeighborhoodMembers_)
	     & BOOST_SERIALIZATION_NVP(swarmRandomFillUp_)
	     & BOOST_SERIALIZATION_NVP(swarmCLocal_)
	     & BOOST_SERIALIZATION_NVP(swarmCGlobal_)
	     & BOOST_SERIALIZATION_NVP(swarmCDelta_)
	     & BOOST_SERIALIZATION_NVP(swarmUpdateRule_)
	     & BOOST_SERIALIZATION_NVP(gdNStartingPoints_)
	     & BOOST_SERIALIZATION_NVP(gdFiniteStep_)
	     & BOOST_SERIALIZATION_NVP(gdStepSize_);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GOptimizer();
	/** @brief A constructor that first parses the command line for relevant parameters and then loads data from a config file */
	GOptimizer(int, char **);
	/** @brief A copy constructor */
	GOptimizer(const GOptimizer&);

	/** @brief The destructor */
	virtual ~GOptimizer();

	/** @brief Standard assignment operator */
	const GOptimizer& operator=(const GOptimizer&);

	/** @brief Checks for equality with another GOptimizer object */
	bool operator==(const GOptimizer&) const;
	/** @brief Checks for inequality with another GOptimizer object */
	bool operator!=(const GOptimizer&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(
			const GObject&
			, const Gem::Common::expectation&
			, const double&
			, const std::string&
			, const std::string&
			, const bool&
	) const;

	/** @brief Allows to specify an optimization monitor to be used with evolutionary algorithms */
	void registerOptimizationMonitor(boost::shared_ptr<GEvolutionaryAlgorithm::GEAOptimizationMonitor>);
	/** @brief Allows to specify an optimization monitor to be used with swarm algorithms */
	void registerOptimizationMonitor(boost::shared_ptr<GSwarm::GSwarmOptimizationMonitor>);
	/** @brief Allows to specify an optimization monitor to be used with gradient descents */
	void registerOptimizationMonitor(boost::shared_ptr<GGradientDescent::GGDOptimizationMonitor>);

	/** @brief Triggers execution of the client loop */
	bool clientRun();

	/** @brief Checks whether server mode has been requested for this object */
	bool serverMode() const;
	/** @brief Checks whether this object is running in client mode */
	bool clientMode() const;

	/**************************************************************************************/
	/**
	 * Starts the optimization cycle, using the optimization algorithm that has been requested.
	 * Returns the best individual found, converted to the desired type.
	 *
	 * @return The best individual found during the optimization process, converted to the desired type
	 */
	template <typename ind_type>
	boost::shared_ptr<ind_type> optimize() {
		boost::shared_ptr<ind_type> result;

#ifdef DEBUG
		// We need at least one individual to start with
		if(this->empty()) {
			std::ostringstream error;
			error << "In GOptimizer::optimize(): Error!" << std::endl
					<< "You need to register at least one individual." << std::endl
					<< "Found none." << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}
#endif

		// Which algorithm are we supposed to use ?
		switch(pers_) {
		case EA: // Evolutionary algorithms
		{
			result = eaOptimize<ind_type>();
			return result;
		}
		break;

		case SWARM: // Swarm algorithms
		{
			result = swarmOptimize<ind_type>();
			return result;
		}
		break;

		case GD: // Gradient descents
		{
			result = gdOptimize<ind_type>();
			return result;
		}
		break;

		case NONE:
		{
			std::ostringstream error;
			error << "In GOptimizer::optimize(): Error!" << std::endl
					<< "No optimization algorithm was specified." << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}
		break;
		};

		// Make the compiler happy
		return boost::shared_ptr<ind_type>(); // Empty smart pointer
	}

	/**************************************************************************************/
	/**
	 * Outputs a configuration file with default values
	 *
	 * @param configFile The name of the file to which the configuration should be written
	 */
	static void writeConfigurationFile(const std::string& configFile) {
		std::ofstream cf(configFile.c_str());
		if(!cf) {
			std::ostringstream error;
			error << "In GOptimzer::writeConfigurationFile() : Error!" << std::endl
				  << "Could not open output file " << configFile << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}

		cf << "################################################################" << std::endl
		   << "# This is a configuration file for the optimization            #" << std::endl
		   << "# algorithms implemented in the Geneva library.                #" << std::endl
		   << "# It is meant to be accessed through the GOptimizer            #" << std::endl
		   << "# class.                                                       #" << std::endl
		   << "#                                                              #" << std::endl
		   << "# This file was automatically created by the Geneva library    #" << std::endl
		   << "################################################################" << std::endl
		   << "#" << std::endl
	       << "# General options applicable to all optimization algorithms" << std::endl
	       << std::endl
	       << "# The maximum number of data transfers without result." << std::endl
	       << "# 0 means \"no limit\"" << std::endl
	       << "maxStalledDataTransfers = " << GO_DEF_MAXSTALLED << std::endl
	       << std::endl
	       << "# The maximum number of failed connection attempts between" << std::endl
	       << "# client and server. 0 means \"no limit\"" << std::endl
	       << "maxConnectionAttempts = " << GO_DEF_MAXCONNATT << std::endl
	       << std::endl
	       << "# Specifies whether the optimizer should copy only the best individuals" << std::endl
	       << "# at the end of the optimization or the entire population" << std::endl
	       << "copyBestOnly = " << GO_DEF_COPYBESTONLY << std::endl
	       << "# Indicates whether clients should return their payload even" << std::endl
	       << "# if no better result was found" << std::endl
	       << "returnRegardless = " << GO_DEF_RETURNREGARDLESS << std::endl
	       << std::endl
	       << "# Determines the number of threads simultaneously producing" << std::endl
	       << "# random numbers. 0 means \"automatic\"" << std::endl
	       << "nProducerThreads = " << GO_DEF_NPRODUCERTHREADS << std::endl
	       << std::endl
	       << "# Specifies the number of entries in random number packages" << std::endl
	       << "# coming from the factory" << std::endl
	       << "arraySize = " << GO_DEF_ARRAYSIZE << std::endl
	       << std::endl
	       << "# Determines the number of threads simultaneously performing" << std::endl
	       << "# evaluations in multi-threaded mode. 0 means \"automatic\"" << std::endl
	       << "nEvaluationThreads = " << GO_DEF_NEVALUATIONTHREADS << std::endl
	       << std::endl
	       << "# Specifies whether client-server transfers should be done in" << std::endl
	       << "# text-mode (0), xml-mode (1) or binary-mode (2)" << std::endl
	       << "serializationMode = " << GO_DEF_SERIALIZATIONMODE << std::endl
	       << std::endl
	       << "# Specifies how long the server should wait for arrivals. 1 means:" << std::endl
	       << "\"wait the same amount it has taken the first answer to return\"" << std::endl
	       << "waitFactor = " << GO_DEF_WAITFACTOR << std::endl
	       << std::endl
	       << "# Indicates the maximum number of iterations in the optimization" << std::endl
	       << "maxIterations = " << GO_DEF_MAXITERATIONS << std::endl
	       << std::endl
	       << "# Specifies the maximum amount of time that may pass before the" << std::endl
	       << "# optimization ends. 0 mean \"no limit\""
	       << "maxMinutes = " << GO_DEF_MAXMINUTES << std::endl
	       << std::endl
	       << "# Specifies in which intervals information should be emitted" << std::endl
	       << "reportIteration = " << GO_DEF_MAXITERATIONS << std::endl
	       << std::endl
	       << std::endl
	       << "#######################################################" << std::endl
	       << "# Options applicable to evolutionary algorithms" << std::endl
	       << "#" << std::endl
	       << std::endl
	       << "# The size of the entire population in evolutionary algorithms" << std::endl
	       << "eaPopulationSize = " << GO_DEF_EAPOPULATIONSIZE << std::endl
	       << std::endl
	       << "# The number of parents in the evolutionary algorithm" << std::endl
	       << "eaNParents = " << GO_DEF_EANPARENTS << std::endl
	       << std::endl
	       << "# The type of recombination scheme: DEFAULTRECOMBINE (0)," << std::endl
	       << "# RANDOMRECOMBINE (1) or VALUERECOMBINE(2)" << std::endl
	       << "eaRecombinationScheme = " << GO_DEF_EARECOMBINATIONSCHEME << std::endl
	       << std::endl
	       << "# The sorting scheme: MUPLUSNU (0), MUCOMMANU (1) or MUNU1PRETAIN (2)" << std::endl
	       << "eaSortingScheme = " << GO_DEF_EASORTINGSCHEME << std::endl
	       << std::endl
	       << "# Indicates whether the algorithm should track relationships" << std::endl
	       << "# between old parents and new children" << std::endl
	       << "eaTrackParentRelations = " << GO_DEF_EATRACKPARENTRELATIONS << std::endl
	       << std::endl
	       << std::endl
	       << "#######################################################" << std::endl
	       << "# Options applicable to swarm algorithms" << std::endl
	       << "#" << std::endl
	       << std::endl
	       << "# The number of neighborhodds in swarm algorithms" << std::endl
	       << "swarmNNeighborhoods = " << GO_DEF_SWARMNNEIGHBORHOODS << std::endl
	       << std::endl
	       << "# The number of members in each neighborhood" << std::endl
	       << "swarmNNeighborhoodMembers = " << GO_DEF_SWARMNNEIGHBORHOODMEMBERS << std::endl
	       << std::endl
	       << "# Indicates whether all individuals of a neighborhood should" << std::endl
	       << "# start at the same or a random position" << std::endl
	       << "swarmRandomFillUp = " << GO_DEF_SWARMRANDOMFILLUP << std::endl
	       << std::endl
	       << "# A multiplicative factor for local updates" << std::endl
	       << "swarmCLocal = " << GO_DEF_SWARMCLOCAL << std::endl
	       << std::endl
	       << "# A multiplicative factor for global updates" << std::endl
	       << "swarmCGlobal = " << GO_DEF_SWARMCCGLOBAL << std::endl
	       << std::endl
	       << "# A multiplicative factor for velocities" << std::endl
	       << "swarmCDelta = " << GO_DEF_SWARMCCDELTA << std::endl
	       << std::endl
	       << "# Indicates whether the linear (0) or classic (1)" << std::endl
	       << "# update rule should be used" << std::endl
	       << "swarmUpdateRule = " << GO_DEF_SWARMUPDATERULE << std::endl
	       << std::endl
	       << std::endl
	       << "#######################################################" << std::endl
	       << "# Options applicable to gradient descents" << std::endl
	       << "#" << std::endl
	       << std::endl
	       << "# Indicates how many simultaneous gradient descents should" << std::endl
	       << "# be started" << std::endl
	       << "gdNStartingPoints = " << GO_DEF_GDNSTARTINGPOINTS << std::endl
	       << std::endl
	       << "# Specifies the size of the finite step in each direction" << std::endl
	       << "gdFiniteStep = " << GO_DEF_GDFINITESTEP << std::endl
	       << std::endl
	       << "# Specifies the size of the step made into the direction" << std::endl
	       << "# of steepest descent" << std::endl
	       << "gdStepSize = " << GO_DEF_GDSTEPSIZE << std::endl;

		// Clean up
		cf.close();
	}

	/**************************************************************************************/
	/** @brief Loads the configuration data from a given configuration file */
	void parseConfigurationFile(const std::string&);
	/** @brief Loads some configuration data from arguments passed on the command line (or another char ** that is presented to it) */
	void parseCommandLine(int, char **);

protected:
	/** @brief Loads the data of another GOptimzer object */
	virtual void load_(const GObject *);
	/** @brief Creates a deep clone of this object */
	virtual GObject *clone_() const;

private:
	/**************************************************************************************/
	/**
	 * Performs an EA optimization cycle
	 *
	 * @return The best individual found during the optimization process
	 */
	template <typename ind_type>
	boost::shared_ptr<ind_type> eaOptimize() {
		// This smart pointer will hold the different types of evolutionary algorithms
		boost::shared_ptr<GEvolutionaryAlgorithm> ea_ptr;

		switch(parMode_) {
		//----------------------------------------------------------------------------------
		case SERIAL:
		{
			// Create an empty population
			ea_ptr = boost::shared_ptr<GEvolutionaryAlgorithm>(new GEvolutionaryAlgorithm());
		}
		break;

		//----------------------------------------------------------------------------------
		case MULTITHREADED:
		{
			// Create the multi-threaded population
			boost::shared_ptr<GMultiThreadedEA> eaPar_ptr(new GMultiThreadedEA());

			// Population-specific settings
			eaPar_ptr->setNThreads(nEvaluationThreads_);

			// Assignment to the base pointer
			ea_ptr = eaPar_ptr;
		}
		break;

		//----------------------------------------------------------------------------------
		case ASIONETWORKED:
		{
			// Create a network consumer and enrol it with the broker
			boost::shared_ptr<Gem::Courtier::GAsioTCPConsumerT<GIndividual> > gatc(new Gem::Courtier::GAsioTCPConsumerT<GIndividual>(port_));
			gatc->setSerializationMode(serializationMode_);
			GINDIVIDUALBROKER->enrol(gatc);

			// Create the actual broker population
			boost::shared_ptr<GBrokerEA> eaBroker_ptr(new GBrokerEA());
			eaBroker_ptr->setWaitFactor(waitFactor_);

			// Assignment to the base pointer
			ea_ptr = eaBroker_ptr;
		}
		break;

		//----------------------------------------------------------------------------------
		};

		// Specify some specific EA settings
		ea_ptr->setDefaultPopulationSize(eaPopulationSize_,eaNParents_);
		ea_ptr->setRecombinationMethod(eaRecombinationScheme_);
		ea_ptr->setSortingScheme(eaSortingScheme_);
		ea_ptr->setLogOldParents(eaTrackParentRelations_);

		// Set some general population settings
		ea_ptr->setMaxIteration(maxIterations_);
		ea_ptr->setMaxTime(boost::posix_time::minutes(maxMinutes_));
		ea_ptr->setReportIteration(reportIteration_);

		// Register the optimization monitor, if one has been provided
		if(ea_om_ptr_) ea_ptr->registerOptimizationMonitor(ea_om_ptr_);

		// Calculate a suitable number of individuals to copy into the algorithm
		std::size_t nCopy = 0;
		if(this->size() >= ea_ptr->getDefaultPopulationSize()) {
			nCopy = ea_ptr->getDefaultPopulationSize();
		}
		else if(this->size() < ea_ptr->getDefaultPopulationSize()) {
			nCopy = this->size();
		}

		// Transfer the initial parameter sets to the population
		GOptimizer::iterator it;
		for(it=this->begin(); it!=this->begin() + nCopy; ++it) {
			// Note, there will not be a big space overhead here,
			// as what is being copied are smart pointers, not
			// the individuals themselves.
			ea_ptr->push_back(*it);
		}

		// Get rid of the old content -- need to remove the first nCopy items
		// GEvolutionaryAlgorithm and derivatives may or may not clean their
		// own vector and it is safer to re-integrate them from scratch after
		// the optimization run.
		this->erase(this->begin(), this->begin() + nCopy);

		// Do the actual optimization
		ea_ptr->optimize();

		// Transfer the best (i.e. nCopy first) individuals back into our local individual vector
		for(int i=static_cast<int>(nCopy)-1; i>=0; i--) {
#ifdef DEBUG
			boost::shared_ptr<GParameterSet> p =
					boost::dynamic_pointer_cast<GParameterSet>(ea_ptr->at(boost::numeric_cast<std::size_t>(i)));
			this->insert(this->begin(), p);
#else
			this->insert(
					this->begin()
					, boost::static_pointer_cast<GParameterSet>(ea_ptr->at(static_cast<std::size_t>(i)))
			);
#endif /* DEBUG */
		}

		// Retrieve the best individual found
		boost::shared_ptr<GParameterSet> result = ea_ptr->getBestIndividual<ind_type>();

		// Make sure ea_ptr is clean again
		ea_ptr->clear();

		// Return the best individual found
		return result;
	}

	/**************************************************************************************/
	/**
	 * Performs a swarm optimization cycle
	 *
	 * @return The best individual found during the optimization process
	 */
	template <typename ind_type>
	boost::shared_ptr<ind_type> swarmOptimize() {
		// This smart pointer will hold the different types of evolutionary algorithms
		boost::shared_ptr<GSwarm> swarm_ptr;

		switch(parMode_) {
		//----------------------------------------------------------------------------------
		case SERIAL:
		{
			swarm_ptr = boost::shared_ptr<GSwarm>(new GSwarm(swarmNNeighborhoods_, swarmNNeighborhoodMembers_));
		}
		break;

		//----------------------------------------------------------------------------------

		case MULTITHREADED:
		{
			// Create the multi-threaded population
			boost::shared_ptr<GMultiThreadedSwarm> swarmPar_ptr(new GMultiThreadedSwarm(swarmNNeighborhoods_, swarmNNeighborhoodMembers_));

			// Population-specific settings
			swarmPar_ptr->setNThreads(nEvaluationThreads_);

			// Assignment to the base pointer
			swarm_ptr = swarmPar_ptr;
		}
		break;

		//----------------------------------------------------------------------------------

		case ASIONETWORKED:
		{
			// Create a network consumer and enrol it with the broker
			boost::shared_ptr<Gem::Courtier::GAsioTCPConsumerT<GIndividual> > gatc(new Gem::Courtier::GAsioTCPConsumerT<GIndividual>(port_));
			GINDIVIDUALBROKER->enrol(gatc);

			// Create the actual broker population
			boost::shared_ptr<GBrokerSwarm> swarmBroker_ptr(new GBrokerSwarm(swarmNNeighborhoods_, swarmNNeighborhoodMembers_));
			swarmBroker_ptr->setWaitFactor(waitFactor_);

			// Assignment to the base pointer
			swarm_ptr = swarmBroker_ptr;
		}
		break;

		//----------------------------------------------------------------------------------
		};

		// Specify some specific swarm settings
		if(swarmRandomFillUp_) {
			swarm_ptr->sedNeighborhoodsRandomFillUp();
		}
		else {
			swarm_ptr->setNeighborhoodsEqualFillUp();
		}
		swarm_ptr->setCLocal(swarmCLocal_);
		swarm_ptr->setCGlobal(swarmCGlobal_);
		swarm_ptr->setCDelta(swarmCDelta_);
		swarm_ptr->setUpdateRule(swarmUpdateRule_);

		// Set some general population settings
		swarm_ptr->setMaxIteration(maxIterations_);
		swarm_ptr->setMaxTime(boost::posix_time::minutes(maxMinutes_));
		swarm_ptr->setReportIteration(reportIteration_);

		// Register the optimization monitor (if one has been provided)
		if(swarm_om_ptr_) swarm_ptr->registerOptimizationMonitor(swarm_om_ptr_);

		// Return the best individual found
		return swarm_ptr->getBestIndividual<ind_type>();
	}

	/**************************************************************************************/
	/**
	 * Performs a GD optimization cycle
	 *
	 * @return The best individual found during the optimization process
	 */
	template <typename ind_type>
	boost::shared_ptr<ind_type> gdOptimize() {
		// This smart pointer will hold the different types of evolutionary algorithms
		boost::shared_ptr<GGradientDescent> gd_ptr;

		switch(parMode_) {
		//----------------------------------------------------------------------------------
		case SERIAL:
		{
			// Create an empty population
			gd_ptr = boost::shared_ptr<GGradientDescent>(new GGradientDescent(gdNStartingPoints_, gdFiniteStep_, gdStepSize_));
		}
		break;

		//----------------------------------------------------------------------------------
		case MULTITHREADED:
		{
		  // Create the multi-threaded population
		  boost::shared_ptr<GMultiThreadedGD> gdPar_ptr(new GMultiThreadedGD(gdNStartingPoints_, gdFiniteStep_, gdStepSize_));

		  // Population-specific settings
		  gdPar_ptr->setNThreads(nEvaluationThreads_);

		  // Assignment to the base pointer
		  gd_ptr = gdPar_ptr;
		}
		break;

		//----------------------------------------------------------------------------------
		case ASIONETWORKED:
		{
			std::ostringstream error;
			error << "In GOptimizer::gdOptimize(): Error!" << std::endl
				  << "ASIONETWORKED mode not implemented yet for gradient descents." << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}
		break;

		//----------------------------------------------------------------------------------
		};

		// Set some general population settings
		gd_ptr->setMaxIteration(maxIterations_);
		gd_ptr->setMaxTime(boost::posix_time::minutes(maxMinutes_));
		gd_ptr->setReportIteration(reportIteration_);

		// Register the optimization monitor (if one has been provided)
		if(gd_om_ptr_) gd_ptr->registerOptimizationMonitor(gd_om_ptr_);

		// Return the best individual found
		return gd_ptr->getBestIndividual<ind_type>();
	}

	/**********************************************************************/
	// These parameters can enter the object through the constructor
	personality pers_; ///< Indicates which optimization algorithm should be used
	parMode parMode_; ///< The chosen parallelization mode
	bool serverMode_; ///< Specifies whether this object is in server (true) or client (false) mode
	Gem::Common::serializationMode serializationMode_; ///< Indicates whether serialization should be done in Text, XML or Binary form
    std::string ip_; ///< Where the server can be reached
    unsigned short port_; ///< The port on which the server answers
	std::string configFilename_; ///< Indicates where the configuration file is stored
	bool verbose_; ///< Whether additional information should be emitted, e.g. when parsing configuration files

	boost::shared_ptr<GEvolutionaryAlgorithm::GEAOptimizationMonitor> ea_om_ptr_; ///< Holds a specific optimization monitor used for evolutionary algorithms
	boost::shared_ptr<GSwarm::GSwarmOptimizationMonitor> swarm_om_ptr_; ///< Holds a specific optimization monitor used for swarm algorithms
	boost::shared_ptr<GGradientDescent::GGDOptimizationMonitor> gd_om_ptr_; ///< Holds a specific optimization monitor used for gradient descents

	//----------------------------------------------------------------------------------------------------------------
	// These parameters can be read from a configuration file

	// Steering parameters of the optimizer
	bool copyBestOnly_;

	// General parameters
    boost::uint32_t maxStalledDataTransfers_; ///< Specifies how often a client may try to unsuccessfully retrieve data from the server (0 means endless)
    boost::uint32_t maxConnectionAttempts_; ///< Specifies how often a client may try to connect unsuccessfully to the server (0 means endless)
    bool returnRegardless_; ///< Specifies whether unsuccessful processing attempts should be returned to the server
    boost::uint16_t nProducerThreads_; ///< The number of threads that will simultaneously produce random numbers
    std::size_t arraySize_; ///< The size of the random number packages being transferred to the proxy RNGs
    boost::uint16_t nEvaluationThreads_; ///< The number of threads used for evaluations in multithreaded execution
    Gem::Common::serializationMode serializationMode_; ///< The mode used for the (de-)serialization of objects
    boost::uint32_t waitFactor_; ///< Influences the timeout in each iteration on the server side in networked execution
    boost::uint32_t maxIterations_; ///< The maximum number of iterations of the optimization algorithms
    long maxMinutes_; ///< The maximum duration of the optimization
    boost::uint32_t reportIteration_; ///< The number of iterations after which information should be emitted

    // EA parameters
    std::size_t eaPopulationSize_; ///< The desired size of EA populations
    std::size_t eaNParents_; ///< The number of parents in an EA population
    recoScheme eaRecombinationScheme_; ///< The recombination scheme in EA
    sortingMode eaSortingScheme_; ///< The sorting scheme in EA (MUCOMMANU etc.)
    bool eaTrackParentRelations_; ///< Whether relations between children and parents should be tracked in EA

    // SWARM parameters
    std::size_t swarmNNeighborhoods_; ///< The number of neighborhoods in a swarm algorithm
    std::size_t swarmNNeighborhoodMembers_; ///< The number of members ine ach neighborhood
    bool swarmRandomFillUp_; ///< Specifies whether neighborhoods are filled up with random values
	float swarmCLocal_; ///< A factor for multiplication of local bests
	float swarmCGlobal_; ///< A factor for multiplication of global bests
	float swarmCDelta_; ///< A factor for multiplication of deltas
	updateRule swarmUpdateRule_; ///< Specifies how the parameters are updated

    // Gradient descent parameters
    std::size_t gdNStartingPoints_;
    float gdFiniteStep_;
    float gdStepSize_;
};

/**************************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

#endif /* GOPTIMIZER_HPP_ */
