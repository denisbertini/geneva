/**
 * @file GBasePopulation.cpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2007-2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of Geneva, Gemfony scientific's optimization library.
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
 */

#include "GBasePopulation.hpp"

/**
 * Included here so no conflicts occur. See explanation at
 * http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
 */
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::GenEvA::GBasePopulation)

namespace Gem {
namespace GenEvA {

/***********************************************************************************/
/**
 * The default constructor, As we do not have any individuals yet, we set the population
 * size, and number of parents to 0. It is the philosophy of this class not
 * to provide constructors for each and every use case. Instead, you should set
 * vital parameters, such as the population size or the parent individuals by hand.
 */
GBasePopulation::GBasePopulation() :
	GIndividualSet(),
	nParents_(0),
	popSize_(0),
	generation_(0),
	maxGeneration_(DEFAULTMAXGEN),
	stallCounter_(0),
	bestPastFitness_(0.), // will be set appropriately in the optimize() function
	maxStallGeneration_(DEFAULMAXTSTALLGEN),
	microTrainingInterval_(DEFAULTMICROTRAININGINTERVAL),
	reportGeneration_(DEFAULTREPORTGEN),
	cpInterval_(DEFAULTCHECKPOINTGEN),
	cpBaseName_(DEFAULTCPBASENAME), // Set in GIndividualSet.hpp
	cpDirectory_(DEFAULTCPDIR), // Set in GIndividualSet.hpp
	recombinationMethod_(DEFAULTRECOMBINE),
	smode_(DEFAULTSMODE),
	maximize_(DEFAULTMAXMODE),
	id_("empty"),
	firstId_(true), // The "real" id will be set in the GBasePopulation::optimize function
	maxDuration_(boost::posix_time::duration_from_string(DEFAULTDURATION)),
	defaultNChildren_(0),
	qualityThreshold_(DEFAULTQUALITYTHRESHOLD),
	hasQualityThreshold_(false),
	mtNChildren_(DEFAULTMTNCHILDREN),
	mtMaxGenerations_(DEFAULTMTMAXGENERATIONS),
	mtAlwaysCopy_(DEFAULTMTALWAYSCOPY),
	mtSMode_(DEFAULTMTSMODE),
	infoFunction_(&GBasePopulation::defaultInfoFunction)
{ /* nothing */ }

/***********************************************************************************/
/**
 * A standard copy constructor. Note that the generation number is reset to 0 and
 * is not copied from the other object. We assume that a new optimization run will
 * be started.
 *
 * @param cp Another GBasePopulation object
 */
GBasePopulation::GBasePopulation(const GBasePopulation& cp) :
	GIndividualSet(cp),
	nParents_(cp.nParents_),
	popSize_(cp.popSize_),
	generation_(0),
	maxGeneration_(cp.maxGeneration_),
	stallCounter_(cp.stallCounter_),
	bestPastFitness_(cp.bestPastFitness_),
	maxStallGeneration_(cp.maxStallGeneration_),
	microTrainingInterval_(cp.microTrainingInterval_),
	reportGeneration_(cp.reportGeneration_),
	cpInterval_(cp.cpInterval_),
	cpBaseName_(cp.cpBaseName_),
	cpDirectory_(cp.cpDirectory_),
	recombinationMethod_(cp.recombinationMethod_),
	smode_(cp.smode_),
	maximize_(cp.maximize_),
	id_("empty"),
	firstId_(true), // We want the id to be re-calculated for a new object
	maxDuration_(cp.maxDuration_),
	defaultNChildren_(cp.defaultNChildren_),
	qualityThreshold_(cp.qualityThreshold_),
	hasQualityThreshold_(cp.hasQualityThreshold_),
	mtNChildren_(cp.mtNChildren_),
	mtMaxGenerations_(cp.mtMaxGenerations_),
	mtAlwaysCopy_(cp.mtAlwaysCopy_),
	mtSMode_(cp.mtSMode_),
	infoFunction_(cp.infoFunction_)
{ /* nothing */ }

/***********************************************************************************/
/**
 * The standard destructor. All work is done in the parent class.
 */
GBasePopulation::~GBasePopulation()
{ /* nothing */ }

/***********************************************************************************/
/**
 * The standard assignment operator.
 *
 * @param cp Another GBasePopulation object
 * @return A constant reference to this object
 */
const GBasePopulation& GBasePopulation::operator=(const GBasePopulation& cp) {
	GBasePopulation::load(&cp);
	return *this;
}

/***********************************************************************************/
/**
 * Loads the data of another GBasePopulation object, camouflaged as a GObject.
 *
 * @param cp A pointer to another GBasePopulation object, camouflaged as a GObject
 */
void GBasePopulation::load(const GObject * cp)
{
	const GBasePopulation *gbp_load = this->conversion_cast(cp,this);

	// First load the parent class'es data ...
	GIndividualSet::load(cp);

	// ... and then our own data
	nParents_ = gbp_load->nParents_;
	popSize_ = gbp_load->popSize_;
	generation_ = 0; // We assume that this is the start of a new optimization run
	maxGeneration_ = gbp_load->maxGeneration_;
	stallCounter_ = gbp_load->stallCounter_;
	bestPastFitness_ = gbp_load->bestPastFitness_;
	maxStallGeneration_ = gbp_load->maxStallGeneration_;
	microTrainingInterval_ = gbp_load->microTrainingInterval_;
	reportGeneration_ = gbp_load->reportGeneration_;
	cpInterval_ = gbp_load->cpInterval_;
	cpBaseName_ = gbp_load->cpBaseName_;
	cpDirectory_ = gbp_load->cpDirectory_;
	recombinationMethod_ = gbp_load->recombinationMethod_;
	smode_ = gbp_load->smode_;
	maximize_ = gbp_load->maximize_;
	id_="empty"; // We need our own id
	firstId_=true, // We want the id to be re-calculated for a new object
	maxDuration_ = gbp_load->maxDuration_;
	defaultNChildren_ = gbp_load->defaultNChildren_;
	qualityThreshold_=gbp_load->qualityThreshold_;
	hasQualityThreshold_=gbp_load->hasQualityThreshold_;
	hasQualityThreshold_=gbp_load->hasQualityThreshold_;
	mtNChildren_=gbp_load->mtNChildren_;
	mtMaxGenerations_=gbp_load->mtMaxGenerations_;
	mtAlwaysCopy_=gbp_load->mtAlwaysCopy_;
	mtSMode_=gbp_load->mtSMode_;
	infoFunction_ = gbp_load->infoFunction_;
}

/***********************************************************************************/
/**
 * Creates a deep copy of this object
 *
 * @return A deep copy of this object, camouflaged as a pointer to a GObject
 */
GObject *GBasePopulation::clone() const  {
	return new GBasePopulation(*this);
}

/***********************************************************************************/
/**
 * Checks for equality with another GBasePopulation object
 *
 * @param  cp A constant reference to another GBasePopulation object
 * @return A boolean indicating whether both objects are equal
 */
bool GBasePopulation::operator==(const GBasePopulation& cp) const {
	return GBasePopulation::isEqualTo(cp,  boost::logic::indeterminate);
}

/***********************************************************************************/
/**
 * Checks for inequality with another GBasePopulation object
 *
 * @param  cp A constant reference to another GBasePopulation object
 * @return A boolean indicating whether both objects are inequal
 */
bool GBasePopulation::operator!=(const GBasePopulation& cp) const {
	return !GBasePopulation::isEqualTo(cp,  boost::logic::indeterminate);
}

/***********************************************************************************/
/**
 * Checks for equality with another GBasePopulation object.
 *
 * @param  cp A constant reference to another GBasePopulation object
 * @return A boolean indicating whether both objects are equal
 */
bool GBasePopulation::isEqualTo(const GObject& cp, const boost::logic::tribool& expected) const {
	using namespace Gem::Util;

	// Check that we are indeed dealing with a GIndividual reference
	const GBasePopulation *gbp_load = GObject::conversion_cast(&cp,  this);

	// First take care of our parent class
	if(!GIndividualSet::isEqualTo( *gbp_load, expected)) return  false;

	// Then we take care of the local data
	if(checkForInequality("GBasePopulation", nParents_, gbp_load->nParents_,"nParents_", "gbp_load->nParents_", expected)) return false;
	if(checkForInequality("GBasePopulation", popSize_, gbp_load->popSize_,"popSize_", "gbp_load->popSize_", expected)) return false;
	if(checkForInequality("GBasePopulation", generation_, gbp_load->generation_,"generation_", "gbp_load->generation_", expected)) return false;
	if(checkForInequality("GBasePopulation", maxGeneration_, gbp_load->maxGeneration_,"maxGeneration_", "gbp_load->maxGeneration_", expected)) return false;
	if(checkForInequality("GBasePopulation", stallCounter_, gbp_load->stallCounter_,"stallCounter_", "gbp_load->stallCounter_", expected)) return false;
	if(checkForInequality("GBasePopulation", bestPastFitness_, gbp_load->bestPastFitness_,"bestPastFitness_", "gbp_load->bestPastFitness_", expected)) return false;
	if(checkForInequality("GBasePopulation", maxStallGeneration_, gbp_load->maxStallGeneration_,"maxStallGeneration_", "gbp_load->maxStallGeneration_", expected)) return false;
	if(checkForInequality("GBasePopulation", microTrainingInterval_, gbp_load->microTrainingInterval_,"microTrainingInterval_", "gbp_load->microTrainingInterval_", expected)) return false;
	if(checkForInequality("GBasePopulation", reportGeneration_, gbp_load->reportGeneration_,"reportGeneration_", "gbp_load->reportGeneration_", expected)) return false;
	if(checkForInequality("GBasePopulation", cpInterval_, gbp_load->cpInterval_,"cpInterval_", "gbp_load->cpInterval_", expected)) return false;
	if(checkForInequality("GBasePopulation", cpBaseName_, gbp_load->cpBaseName_,"cpBaseName_", "gbp_load->cpBaseName_", expected)) return false;
	if(checkForInequality("GBasePopulation", cpDirectory_, gbp_load->cpDirectory_,"cpDirectory_", "gbp_load->cpDirectory_", expected)) return false;
	if(checkForInequality("GBasePopulation", recombinationMethod_, gbp_load->recombinationMethod_,"recombinationMethod_", "gbp_load->recombinationMethod_", expected)) return false;
	if(checkForInequality("GBasePopulation", smode_, gbp_load->smode_,"smode_", "gbp_load->smode_", expected)) return false;
	if(checkForInequality("GBasePopulation", maximize_, gbp_load->maximize_,"maximize_", "gbp_load->maximize_", expected)) return false;
	if(checkForInequality("GBasePopulation", id_, gbp_load->id_,"id_", "gbp_load->id_", expected)) return false;
	if(checkForInequality("GBasePopulation", firstId_, gbp_load->firstId_,"firstId_", "gbp_load->firstId_", expected)) return false;
	if(checkForInequality("GBasePopulation", maxDuration_, gbp_load->maxDuration_,"maxDuration_", "gbp_load->maxDuration_", expected)) return false;
	// startTime_ Not compared, as it is used for temporary storage only.
	if(checkForInequality("GBasePopulation", defaultNChildren_, gbp_load->defaultNChildren_,"defaultNChildren_", "gbp_load->defaultNChildren_", expected)) return false;
	if(checkForInequality("GBasePopulation", qualityThreshold_, gbp_load->qualityThreshold_,"qualityThreshold_", "gbp_load->qualityThreshold_", expected)) return false;
	if(checkForInequality("GBasePopulation", hasQualityThreshold_, gbp_load->hasQualityThreshold_,"hasQualityThreshold_", "gbp_load->hasQualityThreshold_", expected)) return false;
	if(checkForInequality("GBasePopulation", mtNChildren_, gbp_load->mtNChildren_,"mtNChildren_", "gbp_load->mtNChildren_", expected)) return false;
	if(checkForInequality("GBasePopulation", mtMaxGenerations_, gbp_load->mtMaxGenerations_,"mtMaxGenerations_", "gbp_load->mtMaxGenerations_", expected)) return false;
	if(checkForInequality("GBasePopulation", mtAlwaysCopy_, gbp_load->mtAlwaysCopy_,"mtAlwaysCopy_", "gbp_load->mtAlwaysCopy_", expected)) return false;
	if(checkForInequality("GBasePopulation", mtSMode_, gbp_load->mtSMode_,"mtSMode_", "gbp_load->mtSMode_", expected)) return false;


	return true;
}

/***********************************************************************************/
/**
 * Checks for similarity with another GBasePopulation object.
 *
 * @param  cp A constant reference to another GBasePopulation object
 * @param limit A double value specifying the acceptable level of differences of floating point values
 * @return A boolean indicating whether both objects are similar to each other
 */
bool GBasePopulation::isSimilarTo(const GObject& cp, const double& limit, const boost::logic::tribool& expected) const {
	using namespace Gem::Util;

	// Check that we are indeed dealing with a GIndividual reference
	const GBasePopulation *gbp_load = GObject::conversion_cast(&cp,  this);

	// First take care of our parent class
	if(!GIndividualSet::isSimilarTo(*gbp_load, limit, expected)) return  false;

	// Then we take care of the local data
	if(checkForDissimilarity("GBasePopulation", nParents_, gbp_load->nParents_, limit, "nParents_", "gbp_load->nParents_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", popSize_, gbp_load->popSize_, limit, "popSize_", "gbp_load->popSize_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", generation_, gbp_load->generation_, limit, "generation_", "gbp_load->generation_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", maxGeneration_, gbp_load->maxGeneration_, limit, "maxGeneration_", "gbp_load->maxGeneration_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", stallCounter_, gbp_load->stallCounter_, limit, "stallCounter_", "gbp_load->stallCounter_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", bestPastFitness_, gbp_load->bestPastFitness_, limit, "bestPastFitness_", "gbp_load->bestPastFitness_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", maxStallGeneration_, gbp_load->maxStallGeneration_, limit, "maxStallGeneration_", "gbp_load->maxStallGeneration_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", microTrainingInterval_, gbp_load->microTrainingInterval_, limit, "microTrainingInterval_", "gbp_load->microTrainingInterval_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", reportGeneration_, gbp_load->reportGeneration_, limit, "reportGeneration_", "gbp_load->reportGeneration_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", cpInterval_, gbp_load->cpInterval_, limit, "cpInterval_", "gbp_load->cpInterval_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", cpBaseName_, gbp_load->cpBaseName_, limit, "cpBaseName_", "gbp_load->cpBaseName_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", cpDirectory_, gbp_load->cpDirectory_, limit, "cpDirectory_", "gbp_load->cpDirectory_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", recombinationMethod_, gbp_load->recombinationMethod_, limit, "recombinationMethod_", "gbp_load->recombinationMethod_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", smode_, gbp_load->smode_, limit, "smode_", "gbp_load->smode_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", maximize_, gbp_load->maximize_, limit, "maximize_", "gbp_load->maximize_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", id_, gbp_load->id_, limit, "id_", "gbp_load->id_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", firstId_, gbp_load->firstId_, limit, "firstId_", "gbp_load->firstId_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", maxDuration_, gbp_load->maxDuration_, limit, "maxDuration_", "gbp_load->maxDuration_", expected)) return false;
	// startTime_ Not compared, as it is used for temporary storage only.
	if(checkForDissimilarity("GBasePopulation", defaultNChildren_, gbp_load->defaultNChildren_, limit, "defaultNChildren_", "gbp_load->defaultNChildren_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", qualityThreshold_, gbp_load->qualityThreshold_, limit, "qualityThreshold_", "gbp_load->qualityThreshold_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", hasQualityThreshold_, gbp_load->hasQualityThreshold_, limit, "hasQualityThreshold_", "gbp_load->hasQualityThreshold_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", mtNChildren_, gbp_load->mtNChildren_, limit, "mtNChildren_", "gbp_load->mtNChildren_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", mtMaxGenerations_, gbp_load->mtMaxGenerations_, limit, "mtMaxGenerations_", "gbp_load->mtMaxGenerations_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", mtAlwaysCopy_, gbp_load->mtAlwaysCopy_, limit, "mtAlwaysCopy_", "gbp_load->mtAlwaysCopy_", expected)) return false;
	if(checkForDissimilarity("GBasePopulation", mtSMode_, gbp_load->mtSMode_, limit, "mtSMode_", "gbp_load->mtSMode_", expected)) return false;

	return true;
}

/***********************************************************************************/
/**
 * Performs the necessary administratory work of doing check-pointing
 */
void GBasePopulation::checkpoint(const bool& better) const {
	// Save checkpoints if required by the user
	if(cpInterval_ == -1 && better) this->saveCheckpoint();
	else if(cpInterval_ && generation_%cpInterval_ == 0) this->saveCheckpoint();
}

/***********************************************************************************/
/**
 * Saves the state of the class to disc. The function adds the current generation
 * and the fitness to the base name, which is either supplied as an argument or, if
 * left empty (or "empty" is supplied, is created automatically. We do not save the
 * entire population, but only the best individuals, as these contain the "real"
 * information. Note that no real copying of the individual's data takes place here,
 * as we are dealing with boost::shared_ptr objects. Private, as we do not want to
 * accidently trigger value calculation
 */
void GBasePopulation::saveCheckpoint() const {
	// Copy the nParents best individuals to a vector
	std::vector<boost::shared_ptr<Gem::GenEvA::GIndividual> > bestIndividuals;
	GBasePopulation::const_iterator it;
	for(it=this->begin(); it!=this->begin() + this->getNParents(); ++it)
		bestIndividuals.push_back(*it);

#ifdef DEBUG // Cross check so we do not accidently trigger value calculation
				if(this->at(0)->isDirty()) {
					std::ostringstream error;
					error << "In GBasePopulation::saveCheckpoint():" << std::endl
						  << "Error: class member has the dirty flag set" << std::endl;
					throw(Gem::GenEvA::geneva_error_condition(error.str()));
				}
#endif /* DEBUG */
	double newValue = this->at(0)->fitness();

	// Determine a suitable name for the output file
	std::string outputFile = cpDirectory_ + boost::lexical_cast<std::string>(this->getGeneration()) + "_"
		+ boost::lexical_cast<std::string>(newValue) + "_" + cpBaseName_;

	// Create the output stream and check that it is in good order
	std::ofstream checkpointStream(outputFile.c_str());
	if(!checkpointStream) {
		std::ostringstream error;
		error << "In GBasePopulation::saveCheckpoint(const std::string&)" << std::endl
			  << "Error: Could not open output file";
		throw geneva_error_condition(error.str());
	}

	// Write the individuals' data to disc in binary mode
	{
		boost::archive::binary_oarchive oa(checkpointStream);
		oa << boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
	} // note: explicit scope here is essential so the oa-destructor gets called

	// Make sure the stream is closed again
	checkpointStream.close();
}

/***********************************************************************************/
/**
 * Loads the state of the class from disc. We do not load the entire population,
 * but only the best individuals of a former optimization run, as these contain the
 * "real" information.
 */
void GBasePopulation::loadCheckpoint(const std::string& cpFile) {
	// Create a vector to hold the best individuals
	std::vector<boost::shared_ptr<Gem::GenEvA::GIndividual> > bestIndividuals;

	// Check that the file indeed exists
	if(!boost::filesystem::exists(cpFile)) {
		std::ostringstream error;
		error << "In GBasePopulation::loadCheckpoint(const std::string&)" << std::endl
			  << "Got invalid checkpoint file name " << cpFile << std::endl;
		throw geneva_error_condition(error.str());
	}

	// Create the input stream and check that it is in good order
	std::ifstream checkpointStream(cpFile.c_str());
	if(!checkpointStream) {
		std::ostringstream error;
		error << "In GBasePopulation::loadCheckpoint(const std::string&)" << std::endl
			  << "Error: Could not open input file";
		throw geneva_error_condition(error.str());
	}

	// Load the data from disc in binary mode
	{
		boost::archive::binary_iarchive ia(checkpointStream);
	    ia >> boost::serialization::make_nvp("bestIndividuals", bestIndividuals);
	} // note: explicit scope here is essential so the ia-destructor gets called

	// Make sure the stream is closed again
	checkpointStream.close();

	// Load the individuals into this class
	std::size_t thisSize = this->size();
	std::size_t biSize = bestIndividuals.size();
	if(thisSize >= biSize) { // The most likely case
		for(std::size_t ic=0; ic<biSize; ic++) {
			(*this)[ic]->load((bestIndividuals[ic]).get());
		}
	}
	else if(thisSize < biSize) {
		for(std::size_t ic=0; ic<thisSize; ic++) {
			(*this)[ic]->load((bestIndividuals[ic]).get());
		}
		for(std::size_t ic=thisSize; ic<biSize; ic++) {
			this->push_back(bestIndividuals[ic]);
		}
	}
}

/***********************************************************************************/
/**
 * Allows to set the number of generations after which a checkpoint should be written
 *
 * @param cpInterval The number of generations after which a checkpoint should be written
 */
void GBasePopulation::setCheckpointInterval(const boost::int32_t& cpInterval) {
	if(cpInterval < -1) {
		std::ostringstream error;
		error << "In GBasePopulation::setCheckpointInterval():" << std::endl
			  << "Error: received bad checkpoint interval: " << cpInterval << std::endl;
		throw geneva_error_condition(error.str());
	}

	cpInterval_ = cpInterval;
}

/***********************************************************************************/
/**
 * Allows to retrieve the number of generations after which a checkpoint should be written
 *
 * @return The number of generations after which a checkpoint should be written
 */
boost::uint32_t GBasePopulation::getCheckpointInterval() const {
	return cpInterval_;
}

/***********************************************************************************/
/**
 * Allows to set the base name of the checkpoint file and the directory where it
 * should be stored.
 *
 * @param cpDirectory The directory where checkpoint files should be stored
 * @param cpBaseName The base name used for the checkpoint files
 */
void GBasePopulation::setCheckpointBaseName(const std::string& cpDirectory, const std::string& cpBaseName) {
	// Do some basic checks
	if(cpBaseName == "empty" || cpBaseName.empty()) {
		std::ostringstream error;
		error << "In GBasePopulation::setCheckpointBaseName(const std::string&, const std::string&):" << std::endl
			  << "Error: Invalid cpBaseName: " << cpBaseName << std::endl;
		throw geneva_error_condition(error.str());
	}

	if(cpDirectory == "empty" || cpDirectory.empty()) {
		std::ostringstream error;
		error << "In GBasePopulation::setCheckpointBaseName(const std::string&, const std::string&):" << std::endl
			  << "Error: Invalid cpDirectory: " << cpDirectory << std::endl;
		throw geneva_error_condition(error.str());
	}

	cpBaseName_ = cpBaseName;

	// Check that the provided directory exists
	if(!boost::filesystem::exists(cpDirectory) || !boost::filesystem::is_directory(cpDirectory)) {
		std::ostringstream error;
		error << "In GBasePopulation::setCheckpointBaseName(const std::string&, const std::string&):" << std::endl
			  << "Error: directory does not exist: " << cpDirectory << std::endl;
		throw geneva_error_condition(error.str());
	}

	// Add a trailing slash to the directory name, if necessary
    if(cpDirectory[cpDirectory.size() - 1] != '/') cpDirectory_ = cpDirectory + '/';
    else cpDirectory_ = cpDirectory;
}

/***********************************************************************************/
/**
 * Allows to retrieve the base name of the checkpoint file.
 *
 * @return The base name used for checkpoint files
 */
std::string GBasePopulation::getCheckpointBaseName() const {
	return cpBaseName_;
}

/***********************************************************************************/
/**
 * Allows to retrieve the directory where checkpoint files should be stored
 *
 * @return The base name used for checkpoint files
 */
std::string GBasePopulation::getCheckpointDirectory() const {
	return cpDirectory_;
}

/***********************************************************************************/
/**
 * This is the main optimization function and the heart of the GenEvA library.
 * Every time it is called, the number of generations is reseted. The recombination
 * scheme, type of child mutations and the selection scheme are determined in
 * other functions, namely GBasePopulation::recombine(), GBasePopulation::mutateChildren()
 * and GBasePopulation::select() (or overloaded versions in derived classes).
 */
void GBasePopulation::optimize() {
	// Reset the generation counter
	generation_ = 0;

	// Fill up the population as needed
	GBasePopulation::adjustPopulation();

	// Emit the info header, unless we do not want any info. Note that
	// this call needs to come after adjustPopulation(), so we have a
	// "complete" population available.
	if(reportGeneration_) doInfo(INFOINIT);

	// Initialize the start time with the current time. Uses Boost::date_time
	startTime_ = boost::posix_time::second_clock::local_time(); /// Hmmm - not necessarily thread-safe, if each population runs in its own thread ...

	// We want to know when a better value was found.
	bestPastFitness_ = (maximize_?(-DBL_MAX+1):DBL_MAX);
	stallCounter_ = 0;

	do {
		this->recombine(); // create new children from parents
		this->markGeneration(); // Let all individuals know the current generation
		this->markIndividualPositions();
		this->mutateChildren(); // mutate children and calculate their value
		this->select(); // find out the best individuals of the population

		// Check whether a better value was found
		bool better = this->checkProgress();
		//  Do the check-pointing, if necessary
		this->checkpoint(better);

		// Perform micro-training, if requested and necessary
		if(microTrainingInterval_ && stallCounter_%microTrainingInterval_ == 0) {
			this->doMicroTraining();
			stallCounter_ = 0;
		}

		// We want to provide feedback to the user in regular intervals.
		// Set the reportGeneration_ variable to 0 in order not to emit
		// any information.
		if(reportGeneration_ && (generation_%reportGeneration_ == 0)) doInfo(INFOPROCESSING);

		// update the generation_ counter
		generation_++;
	}
	while(!halt()); // allows custom halt criteria

	// Finalize the info output
	if(reportGeneration_) doInfo(INFOEND);
}

/***********************************************************************************/
/**
 * Emits information specific to this population. The function can be overloaded
 * in derived classes. By default we allow the user to register a call-back function
 * using GBasePopulation::registerInfoFunction() . Please note that it is not
 * possible to serialize this function, so it will only be active on the host were
 * it was registered, but not on remote systems.
 *
 * @param im The information mode (INFOINIT, INFOPROCESSING or INFOEND)
 */
void GBasePopulation::doInfo(const infoMode& im) {
	if(!infoFunction_.empty()) infoFunction_(im, this);
}

/***********************************************************************************/
/**
 * The user can specify what information should be emitted in a call-back function
 * that is registered in the setup phase. This functionality is based on boost::function .
 *
 * @param infoFunction A Boost.function object allowing the emission of information
 */
void GBasePopulation::registerInfoFunction(boost::function<void (const infoMode&, GBasePopulation * const)> infoFunction) {
	infoFunction_ = infoFunction;
}

/***********************************************************************************/
/**
 * Sets the number of generations after which the population should
 * report about its inner state.
 *
 * @param reportGeneration The number of generations after which information should be emitted
 */
void GBasePopulation::setReportGeneration(const boost::uint32_t& reportGeneration) {
	reportGeneration_ = reportGeneration;
}

/***********************************************************************************/
/**
 * Returns the number of generations after which the population should
 * report about its inner state.
 *
 * @return The number of generations after which reporting should be done
 */
boost::uint32_t GBasePopulation::getReportGeneration() const {
	return reportGeneration_;
}

/***********************************************************************************/
/**
 * Specifies the initial size of the population plus the number of parents.
 * The population will be filled with additional individuals later, as required --
 * see GBasePopulation::adjustPopulation() . Also, all error checking is done in
 * that function.
 *
 * @param popSize The desired size of the population
 * @param nParents The desired number of parents
 */
void GBasePopulation::setPopulationSize(const std::size_t& popSize, const std::size_t& nParents) {
	popSize_ = popSize;
	nParents_ = nParents;
}

/***********************************************************************************/
/**
 * The function checks that the population size meets the requirements and resizes the
 * population to the appropriate size, if required. An obvious precondition is that at
 * least one individual has been added to the population. It is interpreted as a parent
 * and serves as the template for missing individuals (children and parents). Parents
 * that have already been added will not be replaced. This is one of the few occasions
 * where popSize_ is used directly. In most occasions we refer to the size of the vector
 * instead to allow short-term adjustments of the vector size. Note, though, that
 * GBasePopulation will enforce a minimum number of children, as implied by the initial
 * population size and the number of parents. This function is called once before the
 * optimization cycle.
 */
void GBasePopulation::adjustPopulation() {
	// First check that we have been given suitable values for population size
	// and number of parents

	// Have the population size and number of parents been set at all ?
	if(popSize_ == 0 || nParents_ == 0) {
		std::ostringstream error;
		error << "In GBasePopulation::adjustPopulation() : Error!" << std::endl
			  << "The population size and/or the number of parents have invalid values:" << std::endl
			  << "Did you call GBasePopulation::setPopulationSize() ?" << std::endl
			  << "population size = " << popSize_ << std::endl
			  << "number of parents = " << nParents_ << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}

	// In MUCOMMANU mode we want to have at least as many children as parents,
	// whereas MUPLUSNU only requires the population size to be larger than the
	// number of parents. NUNU1PRETAIN has the same requirements as MUCOMMANU,
	// as it is theoretically possible that all children are better than the former
	// parents, so that the first parent individual will be replaced.
	if(((smode_==MUCOMMANU || smode_==MUNU1PRETAIN) && (popSize_ < 2*nParents_)) || (smode_==MUPLUSNU && popSize_<=nParents_))
	{
		std::ostringstream error;
		error << "In GBasePopulation::adjustPopulation() : Error!" << std::endl
			  << "Requested size of population is too small :" << popSize_ << " " << nParents_ << std::endl
		      << "Sorting scheme is ";

		switch(smode_) {
		case MUPLUSNU:
			error << "MUPLUSNU" << std::endl;
			break;
		case MUCOMMANU:
			error << "MUCOMMANU" << std::endl;
			break;
		case MUNU1PRETAIN:
			error << "MUNU1PRETAIN" << std::endl;
			break;
		}

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}

	// Check how many individuals have been added already. At least one is required.
	std::size_t this_sz = data.size();
	if(this_sz == 0) {
		std::ostringstream error;
		error << "In GBasePopulation::adjustPopulation() : Error!" << std::endl
			  << "size of population is 0. Did you add any individuals?" << std::endl
			  << "We need at least one local individual" << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}

	// Do the smart pointers actually point to any objects ?
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;
	for(it=data.begin(); it!=data.end(); ++it) {
		if(!(*it)) { // shared_ptr can be implicitly converted to bool
			std::ostringstream error;
			error << "In GBasePopulation::adjustPopulation() : Error!" << std::endl
				  << "Found empty smart pointer." << std::endl;

			// throw an exception. Add some information so that if the exception
			// is caught through a base object, no information is lost.
			throw geneva_error_condition(error.str());
		}
	}

	// Fill up as required. We are now sure we
	// have a suitable number of individuals to do so
	if(this_sz >= popSize_) return; // Nothing to do - more children than expected is o.k.
	else { // We need to add members, so that we have a minimum of popSize_ members in the population
		// Missing members are created as copies of the population's first individual
		this->resize_clone(popSize_, data[0]);
	}

	// Let parents know they are parents and children that they are children
	markParents();
	// Let all individuals know about the current generation
	markGeneration();

	// Make sure derived classes (such as GTransferPopulation) have a way of finding out
	// what the desired number of children is. This is particularly important, if - in a
	// network environment, some individuals might not return and some individuals return
	// late. The size of the population then changes and we need to take action.
	defaultNChildren_ = popSize_ - nParents_;
}

/***********************************************************************************/
/**
 * A helper function that helps to determine whether a given value is better than
 * a given older one. As "better" means something different for maximization and minimization,
 * this function helps to make the code easier to understand.
 *
 * @param newValue The new value
 * @param oldValue The old value
 * @return true of newValue is better than oldValue, otherwise false.
 */
bool GBasePopulation::isBetter(double newValue, const double& oldValue) const {
	if(maximize_) {
		if(newValue > oldValue) return true;
		else return false;
	}
	else { // minimization
		if(newValue < oldValue) return true;
		else return false;
	}
}

/***********************************************************************************/
/**
 * Retrieves the id of this object. If this is the first time the function
 * is called, we additionally create the id.
 *
 * @return The value of the id_ variable
 */
std::string GBasePopulation::getId() {
	if(firstId_) {
		id_ = boost::lexical_cast<std::string>(this);
		firstId_=false;
	}

	return id_;
}

/***********************************************************************************/
/**
 * Retrieve the current number of failed optimization attempts in succession
 *
 * @return The current number of failed optimization attempts in succession
 */
boost::uint32_t GBasePopulation::getStallCounter() const {
	return stallCounter_;
}

/***********************************************************************************/
/**
 * Retrieve the current best value found
 *
 * @return The best fitness found so far
 */
double GBasePopulation::getBestFitness() const {
	return bestPastFitness_;
}

/***********************************************************************************/
/**
 * Set the interval in which micro training should be performed. Set the
 * interval to 0 in order to prevent micro training.
 *
 * @param mti The desired new value of the mircoTrainingInterval_ variable
 */
void GBasePopulation::setMicroTrainingInterval(const boost::uint32_t& mti) {
	microTrainingInterval_ = mti;
}

/***********************************************************************************/
/**
 * Retrieve the interval in which micro training should be performed
 *
 * @return The current value of the mircoTrainingInterval_ variable
 */
boost::uint32_t GBasePopulation::getMicroTrainingInterval() const {
	return microTrainingInterval_;
}

/***********************************************************************************/
/**
 * Performs micro-training. If the optimization has stalled for too long: creates
 * copies of the best individuals of the population. Calls their updateOnStall() functions,
 * then does "private" training for a given number of cycles. If better individuals
 * are found this way (or the user requests to always do this), they will replace the former
 * parents
 */
void GBasePopulation::doMicroTraining() {
	// Record the best individual's fitness prior to the micro training
	double startFitness = this->at(0)->fitness();

	// Create a copy of this population, holding the parent objects
	boost::shared_ptr<GBasePopulation> p = this->parent_clone();

	// Call their updateOnStall functions and check that at least one update was made
	bool updatePerformed=false;
	GBasePopulation::iterator it;
	for(it=p->begin(); it!=p->end(); ++it) {
		if((*it)->updateOnStall()) updatePerformed = true;
	}

	// Perform micro training, if at least one parent was updated
	if(updatePerformed) {
		// Update the number of children (set automatically, if requested by the user)
		if(mtNChildren_) p->setPopulationSize(p->getNParents(), mtNChildren_);
		else p->setPopulationSize(p->getNParents(), 10*p->getNParents());

		// Update the number of micro-training generations. If set to 0,
		// we perform training only until an improvement can be seen for the
		// best individual.
		if(mtMaxGenerations_) p->setMaxGeneration(mtMaxGenerations_);
		else {
			p->setMaxGeneration(0); // no generation limit
			p->setQualityThreshold(startFitness); // We want to stop once we are better than the initial fitness
		}

		// Prevent micro-training in the micro-training environment (can lead to an endless loop)
		p->setMicroTrainingInterval(0);

		// Do the actual training
		p->optimize();

		// Check whether an improvement could be seen
		double endFitness = p->at(0)->fitness();

		// Copy the new parents over, if a better result was found (or if the
		// user has asked to copy items over anyway). Note that we use copying
		// instead of loading here for performance reasons. The micro-training
		// population will cease to exist anyway after the end of this function,
		// and copying smart pointers does not mean copying the contents.
		if(mtAlwaysCopy_ || isBetter(endFitness, startFitness)) {
			std::size_t nParents = p->getNParents();
			for(std::size_t i=0; i<nParents; i++) this->at(i) = p->at(i);
		}
	}
}

/***********************************************************************************/
/**
 * Allows to set the number of children used in micro training.
 *
 * @param mtNChildren The number of children used in micro training
 */
void GBasePopulation::setMTNChildren(const boost::uint32_t& mtNChildren) {
	mtNChildren_=mtNChildren;
}

/***********************************************************************************/
/**
 * Retrieves the current value of the mtNChildren_ variable
 *
 * @return The current value of the mtNChildren_ variable
 */
boost::uint32_t GBasePopulation::getMTNChildren() const {
	return mtNChildren_;
}

/***********************************************************************************/
/**
 * Allows to set the number of generations used in micro training.
 *
 * @param mtMaxGenerations The number of generations used in micro training
 */
void GBasePopulation::setMTMaxGenerations(const boost::uint32_t& mtMaxGenerations) {
	mtMaxGenerations_ = mtMaxGenerations;
}

/***********************************************************************************/
/**
 * Retrieves the current value of the mtNChildren_ variable
 *
 * @return The number of generations used in micro training
 */
boost::uint32_t GBasePopulation::getMTMaxGenerations() const {
	return mtMaxGenerations_;
}

/***********************************************************************************/
/**
 * Specifies that parents from a micro training environment should always be copied.
 */
void GBasePopulation::setMTAlwaysCopy() {
	mtAlwaysCopy_=true;
}

/***********************************************************************************/
/**
 * Specifies that parents from a micro training environment should only be copied,
 * if a better result has been achieved
 */
void GBasePopulation::setMTCopyIfBetter() {
	mtAlwaysCopy_=false;
}

/***********************************************************************************/
/**
 * Allows to retrieve the current value of the mtAlwaysCopy_ variable.
 *
 * @return The current value of the mtAlwaysCopy_ variable
 */
bool GBasePopulation::getMTAlwaysCopy() const {
	return mtAlwaysCopy_;
}

/***********************************************************************************/
/**
 * Allows to set the sorting mode used in the micro training environment
 *
 * @param mtSMode The desired sorting mode used in the micro training environment
 */
void GBasePopulation::setMTSortingMode(const sortingMode& mtSMode) {
	mtSMode_=mtSMode;
}

/***********************************************************************************/
/**
 * Allows to retrieve the sorting mode used in the micro training environment
 *
 * @return The sorting mode used in the micro training environment
 */
sortingMode GBasePopulation::getMTSortingMode() const {
	return mtSMode_;
}

/***********************************************************************************/
/**
 * Retrieves the size of the population. Note that the popSize_ parameter set in
 * setPopulationSize() is only needed in the setup phase, particularly in the adjustPopulation
 * function. In all other occasions the population size is assumed to be equal the size of the
 * vector.
 *
 * @return The current size of the population
 */
std::size_t GBasePopulation::getPopulationSize() const {
	return data.size();
}

/***********************************************************************************/
/**
 * Retrieve the number of parents as set by the user. This is a fixed parameter and
 * should not be changed after it has first been set.
 *
 * @return The number of parents in the population
 */
std::size_t GBasePopulation::getNParents() const {
	return nParents_;
}

/***********************************************************************************/
/**
 * Calculates the number of children from the number of parents and the
 * size of the vector.
 *
 * @return The number of children in the population
 */
std::size_t GBasePopulation::getNChildren() const {
	return data.size() - nParents_;
}

/***********************************************************************************/
/**
 * Sets the sorting scheme. In MUPLUSNU, new parents will be selected from the entire
 * population, including the old parents. In MUCOMMANU new parents will be selected
 * from children only. MUNU1PRETAIN means that the best parent of the last generation
 * will also become a new parent (unless a better child was found). All other parents are
 * selected from children only.
 *
 * @param smode The desired sorting scheme
 */
void GBasePopulation::setSortingScheme(const sortingMode& smode) {
	smode_=smode;
}

/***********************************************************************************/
/**
 * Retrieves information about the current sorting scheme (see
 * GBasePopulation::setSortingScheme() for further information).
 *
 * @return The current sorting scheme
 */
sortingMode GBasePopulation::getSortingScheme() const {
	return smode_;
}

/***********************************************************************************/
/**
 * Sets the maximum number of generations allowed for an optimization run. Set
 * to 0 in order for this stop criterion to be disabled.
 *
 * @param The maximum number of allowed generations
 */
void GBasePopulation::setMaxGeneration(const boost::uint32_t& maxGeneration) {
	maxGeneration_ = maxGeneration;
}

/***********************************************************************************/
/**
 * Retrieves the maximum number of generations allowed in an optimization run.
 *
 * @return The maximum number of generations
 */
boost::uint32_t GBasePopulation::getMaxGeneration() const {
	return maxGeneration_;
}

/***********************************************************************************/
/**
 * Sets the maximum number of generations allowed without improvement of the best
 * individual. Set to 0 in order for this stop criterion to be disabled.
 *
 * @param The maximum number of allowed generations
 */
void GBasePopulation::setMaxStallGeneration(const boost::uint32_t& maxStallGeneration) {
	maxStallGeneration_ = maxStallGeneration;
}

/***********************************************************************************/
/**
 * Retrieves the maximum number of generations allowed in an optimization run without
 * improvement of the best individual.
 *
 * @return The maximum number of generations
 */
boost::uint32_t GBasePopulation::getMaxStallGeneration() const {
	return maxStallGeneration_;
}

/***********************************************************************************/
/**
 * Retrieves the currently active generation
 *
 * @return The currently active generation
 */
boost::uint32_t GBasePopulation::getGeneration() const {
	return generation_;
}

/***********************************************************************************/
/**
 * Sets the maximum allowed processing time
 *
 * @param maxDuration The maximum allowed processing time
 */
void GBasePopulation::setMaxTime(const boost::posix_time::time_duration& maxDuration) {
	using namespace boost::posix_time;

	// Only allow "real" values
	if(maxDuration.is_special() || maxDuration.is_negative()) {
		std::ostringstream error;
		error << "In GBasePopulation::setMaxTime() : Error!" << std::endl
			  << "Invalid maxDuration." << std::endl;

		throw geneva_error_condition(error.str());
	}

	maxDuration_ = maxDuration;
}

/***********************************************************************************/
/**
 * Retrieves the value of the maxDuration_ parameter.
 *
 * @return The maximum allowed processing time
 */
boost::posix_time::time_duration GBasePopulation::getMaxTime() {
	return maxDuration_;
}

/***********************************************************************************/
/**
 * This function returns true once a given time (set with GBasePopulation::setMaxTime())
 * has passed. It is used in the GBasePopulation::halt() function.
 *
 * @return A boolean indicating whether a given amount of time has passed
 */
bool GBasePopulation::timedHalt() {
	using namespace boost::posix_time;
	ptime currentTime = microsec_clock::local_time();
	if((currentTime - startTime_) >= maxDuration_) return true;
	return false;
}

/***********************************************************************************/
/**
 * This function returns true once the quality is below or above a given threshold
 * (depending on whether we maximize or minimize).
 *
 * @return A boolean indicating whether the quality is above or below a given threshold
 */
bool GBasePopulation::qualityHalt() {
	bool isDirty;
	if(maximize_) {
		if(this->data.at(0)->getCurrentFitness(isDirty) >= qualityThreshold_) return true;
	}
	else { // minimization
		if(this->data.at(0)->getCurrentFitness(isDirty) <= qualityThreshold_) return true;
	}

	return false;
}

/***********************************************************************************/
/**
 *  Sets a quality threshold beyond which optimization is expected to stop
 *
 *  @param qualityThreshold A threshold beyond which optimization should stop
 */
void GBasePopulation::setQualityThreshold(const double& qualityThreshold) {
	qualityThreshold_ = qualityThreshold;
	hasQualityThreshold_=true;
}

/***********************************************************************************/
/**
 * Retrieves the current value of the quality threshold and also indicates whether
 * the threshold is active
 */
double GBasePopulation::getQualityThreshold(bool& hasQualityThreshold) const {
	hasQualityThreshold = hasQualityThreshold_;
	return qualityThreshold_;
}


/***********************************************************************************/
/**
 * Removes the quality threshold
 */
void GBasePopulation::unsetQualityThreshold() {
	hasQualityThreshold_ = false;
}

/***********************************************************************************/
/**
 * Checks whether a quality threshold has been set
 *
 * @return A boolean indicating whether a quality threshold has been set
 */
bool GBasePopulation::hasQualityThreshold() const {
	return hasQualityThreshold_;
}

/***********************************************************************************/
/**
 * Lets the user specify whether he wants to perform maximization or minimization.
 *
 * @param maximize Indicates whether we want to maximize (true) or minimize (false)
 */
void GBasePopulation::setMaximize(const bool& maximize) {
	maximize_ = maximize;
}

/***********************************************************************************/
/**
 * Retrieves the maximize_ parameter. It indicates whether we are maximizing (true)
 * or minimizing (false).
 *
 * @return The value of the maximize_ parameter
 */
bool GBasePopulation::getMaximize() const {
	return maximize_;
}

/***********************************************************************************/
/**
 * It is possible for users to specify in overloaded versions of this
 * function under which conditions the optimization should be stopped. The
 * function is called from GBasePopulation::halt .
 *
 * @return boolean indicating that a stop condition was reached
 */
bool GBasePopulation::customHalt() {
	/* nothing - specify your own criteria in derived classes */
	return false;
}

/***********************************************************************************/
/**
 * This function assigns a new value to each child individual
 * according to the chosen recombination scheme. It can be
 * overloaded by a user in order to implement his own recombination
 * scheme.
 */
void GBasePopulation::customRecombine() {
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;

	switch(recombinationMethod_){
	case DEFAULTRECOMBINE: // we want the RANDOMRECOMBINE behavior
	case RANDOMRECOMBINE:
		for(it=data.begin()+nParents_; it!= data.end(); ++it) randomRecombine(*it);
		break;

	case VALUERECOMBINE:
		// Recombination according to the parents' fitness only makes sense if
		// we have at least 2 parents. We do the recombination manually otherwise
		if(nParents_==1) {
			for(it=data.begin()+1; it!= data.end(); ++it)
				(*it)->load((data.begin())->get());
		}
		else {
			// Calculate a vector of recombination likelihoods for all parents
			std::size_t i;
			std::vector<double> threshold(nParents_);
			double thresholdSum=0.;
			for(i=0; i<nParents_; i++) {
#ifdef DEBUG
				thresholdSum += 1./(boost::numeric_cast<double>(i)+2.);
#else
				thresholdSum += 1./(static_cast<double>(i)+2.);
#endif /* DEBUG */
			}
			for(i=0; i<nParents_-1; i++) {
				// normalising the sum to 1
#ifdef DEBUG
				threshold[i] = (1./(boost::numeric_cast<double>(i)+2.)) / thresholdSum;
#else
				threshold[i] = (1./(static_cast<double>(i)+2.)) / thresholdSum;
#endif /* DEBUG */

				// Make sure the subsequent range is in the right position
				if(i>0) threshold[i] += threshold[i-1];
			}
			threshold[nParents_-1] = 1.; // Necessary due to rounding errors

			// Do the actual recombination
			for(it=data.begin()+nParents_; it!= data.end(); ++it) {
				// A recombination taking into account the value does not make
				// sense in generation 0, as parents might not have a suitable
				// value. Instead, this function might accidently trigger value
				// calculation in this case. Hence we fall back to random
				// recombination in generation 0. No value calculation takes
				// place there.
				if(generation_ == 0) randomRecombine(*it);
				else valueRecombine(*it, threshold);
			}
		}

		break;
	}
}

/***********************************************************************************/
/**
 * This function implements the RANDOMRECOMBINE scheme. This functions uses BOOST's
 * numeric_cast function for safe conversion between std::size_t and uint16_t.
 *
 * @param pos The position of the individual for which a new value should be chosen
 */
void GBasePopulation::randomRecombine(boost::shared_ptr<GIndividual>& p) {
	std::size_t p_pos;

	// Choose a parent to be used for the recombination. Note that
	// numeric_cat may throw. Exceptions need to be caught in surrounding functions.
	// try/catch blocks would add a non-negligible overhead in this function.
	p_pos = boost::numeric_cast<std::size_t>(gr.discreteRandom(nParents_));

	p->load((data.begin() + p_pos)->get());
}

/***********************************************************************************/
/**
 * This function implements the VALUERECOMBINE scheme. The range [0.,1.[ is divided
 * into nParents_ sub-areas with different size (the largest for the first parent,
 * the smallest for the last). Parents are chosen for recombination according to a
 * random number evenly distributed between 0 and 1. This way parents with higher
 * fitness are more likely to be chosen for recombination.
 *
 * @param pos The child individual for which a parent should be chosen
 * @param threshold A std::vector<double> holding the recombination likelihoods for each parent
 */
void GBasePopulation::valueRecombine(boost::shared_ptr<GIndividual>& p, const std::vector<double>& threshold) {
	bool done=false;
	std::size_t i;
	double randTest = gr.evenRandom(); // get the test value

	for(i=0; i<nParents_; i++) {
		if(randTest<threshold[i]) {
			p->load((data.begin() + i)->get());
			done = true;

			break;
		}
	}

	if(!done) {
		std::ostringstream error;
		error << "In GBasePopulation::valueRecombine(): Error!" << std::endl
			  << "Could not recombine." << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}
}

/***********************************************************************************/
/**
 * This function is called from GBasePopulation::optimize() and performs the
 * actual recombination, based on the recombination schemes defined by the user.
 *
 * Note that this implementation will enforce a minimum number of children,
 * as implied by the initial sizes of the population and the number of parents
 * present. If individuals can get lost in your setting, you must add mechanisms
 * to "repair" the population.
 */
void GBasePopulation::recombine()
{
	// We require at this stage that at least the default number of
	// children is present. If individuals can get lost in your setting,
	// you must add mechanisms to "repair" the population.
	if((data.size()-nParents_) < defaultNChildren_){
		std::ostringstream error;
		error << "In GBasePopulation::recombine(): Error!" << std::endl
			  << "Too few children. Got " << data.size()-nParents_ << "," << std::endl
			  << "but was expecting at least " << defaultNChildren_ << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}

	// Do the actual recombination
	this->customRecombine();

	// Let children know they are children
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;
	for(it=data.begin()+nParents_; it!=data.end(); ++it){
		(*it)->setIsChild();
	}
}

/***********************************************************************************/
/**
 * Mutate all children in sequence. Note that this also triggers their value
 * calculation, so this function needs to be overloaded for optimization in a
 * network context. It is here that you may fix the population size if it
 * has become too small.
 */
void GBasePopulation::mutateChildren()
{
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;

	// We need to make sure that fitness calculation is
	// triggered for all parents. Note that it may well be that at
	// this stage we have several identical parents in the population,
	// due to the actions of the adjustPopulation function.
	if(generation_ == 0)
		for(it=data.begin(); it!=data.begin()+nParents_; ++it) (*it)->fitness();

	// Next we perform the mutation of each child individual in
	// sequence. Note that this could also trigger fitness calculation.
	for(it=data.begin()+nParents_; it!=data.end(); ++it) (*it)->mutate();
}

/***********************************************************************************/
/**
 * Choose new parents, based on the selection scheme set by the user.
 * This function uses Boost.bind and Boost.function (see http://www.boost.org).
 * The function objects used for maximization and minimization are defined
 * in the constructor.
 */
void GBasePopulation::select()
{
#ifdef DEBUG
	// We require at this stage that at least the default number of
	// children is present. If individuals can get lost in your setting,
	// you must add mechanisms to "repair" the population.
	if((data.size()-nParents_) < defaultNChildren_){
		std::ostringstream error;
		error << "In GBasePopulation::select(): Error!" << std::endl
			  << "Too few children. Got " << data.size()-nParents_ << "," << std::endl
			  << "but was expecting at least " << defaultNChildren_ << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}
#endif /* DEBUG */

	switch(smode_) {
	//----------------------------------------------------------------------------
	case MUPLUSNU:
		this->sortMuplusnuMode();
		break;

	//----------------------------------------------------------------------------
	case MUCOMMANU:
		this->sortMucommanuMode();
		break;


	case MUNU1PRETAIN:
		this->sortMunu1pretainMode();
		break;

	//----------------------------------------------------------------------------
	}

	std::vector<boost::shared_ptr<GIndividual> >::iterator it_begin;

	// Let all parents know they are parents
	std::for_each(data.begin(), data.begin()+nParents_,boost::bind(&GIndividual::setIsParent, _1));
}

/***********************************************************************************/
/**
 * Selection, MUPLUSNU style. All individuals of the population (including parents)
 * are sorted. The quality of the population can only increase, but the optimization
 * will stall more easily.
 */
void GBasePopulation::sortMuplusnuMode() {
	// Sort the entire array
	if(maximize_){
		std::partial_sort(data.begin(), data.begin() + nParents_, data.end(),
				boost::bind(&GIndividual::fitness, _1) > boost::bind(&GIndividual::fitness, _2));
	}
	else{
		std::partial_sort(data.begin(), data.begin() + nParents_, data.end(),
				boost::bind(&GIndividual::fitness, _1) < boost::bind(&GIndividual::fitness, _2));
	}
}

/***********************************************************************************/
/**
 * Checks whether a better solution was found and updates the stallCounter_ variable
 * as necessary.
 *
 * @return A boolean indicating whether a better solution was found
 */
bool GBasePopulation::checkProgress() {
#ifdef DEBUG
	if(this->at(0)->isDirty()) {
		std::ostringstream error;
		error << "In GBasePopulation::checkProgress(): Error" << std::endl
			  << "Attempt to calculate fitness of an individual" << std::endl
			  << "whose dirty flag was set." << std::endl;
		throw(Gem::GenEvA::geneva_error_condition(error.str()));
	}
#endif /* DEBUG */

	double newFitness = this->at(0)->fitness(); // the best fitness found in this generation

	// Check whether an improvement has been achieved
	bool better = this->isBetter(newFitness, bestPastFitness_);
	if(better) {
		bestPastFitness_ = newFitness;
		stallCounter_ = 0;
	}
	else {
		stallCounter_++;
	}

	return better;
}

/***********************************************************************************/
/**
 * Selection, MUCOMMANU style. New parents are selected from children only. The quality
 * of the population may decrease from generation to generation, but the optimization
 * is less likely to stall.
 */
void GBasePopulation::sortMucommanuMode() {
	// Only sort the children
	if(maximize_){
		std::partial_sort(data.begin() + nParents_, data.begin() + 2*nParents_, data.end(),
			  boost::bind(&GIndividual::fitness, _1) > boost::bind(&GIndividual::fitness, _2));
	}
	else{
		std::partial_sort(data.begin() + nParents_, data.begin() + 2*nParents_, data.end(),
			  boost::bind(&GIndividual::fitness, _1) < boost::bind(&GIndividual::fitness, _2));
	}
	std::swap_ranges(data.begin(),data.begin()+nParents_,data.begin()+nParents_);
}

/***********************************************************************************/
/**
 * Selection, MUNU1PRETAIN style. This is a hybrid between MUPLUSNU and MUCOMMANU
 * mode. If a better child was found than the best parent of the last generation,
 * all former parents are replaced. If no better child was found than the best
 * parent of the last generation, then this parent stays in place. All other parents
 * are replaced by the (nParents_-1) best children. The scheme falls back to MUPLUSNU
 * mode, of only one parent is available, or if this is the first generation (so we
 * do not accidentally trigger value calculation.
 */
void GBasePopulation::sortMunu1pretainMode() {
	if(nParents_==1 || generation_==0) { // Falls back to MUPLUSNU mode
		this->sortMuplusnuMode();
	} else {
		// Sort the children
		if(maximize_){
			std::partial_sort(data.begin() + nParents_, data.begin() + 2*nParents_, data.end(),
				  boost::bind(&GIndividual::fitness, _1) > boost::bind(&GIndividual::fitness, _2));
		}
		else{
			std::partial_sort(data.begin() + nParents_, data.begin() + 2*nParents_, data.end(),
				  boost::bind(&GIndividual::fitness, _1) < boost::bind(&GIndividual::fitness, _2));
		}

		// Retrieve the best child's and the last generation's best parent's fitness
		double bestChildFitness = (*(data.begin() + nParents_))->fitness();
		double bestParentFitness = (*(data.begin()))->fitness();

		// Leave the best parent in place, if no better child was found
		if(!this->isBetter(bestChildFitness, bestParentFitness)) {
			std::swap_ranges(data.begin()+1,data.begin()+nParents_,data.begin()+nParents_);
		} else { // A better child was found. Overwrite all parents
			std::swap_ranges(data.begin(),data.begin()+nParents_,data.begin()+nParents_);
		}
	}
}


/***********************************************************************************/
/**
 * Possible mutations of a population could involve shifting of individuals.
 * By default, no mutations are defined.
 */
void GBasePopulation::customMutations()
{ /* nothing */}

/***********************************************************************************/
/**
 * Fitness calculation for a population means optimization. The fitness is then determined
 * by the best individual which, after the end of the optimization cycle, can be found in
 * the first position of the array. This is true both for all sorting modes.
 *
 * @return The fitness of the best individual in the population
 */
double GBasePopulation::fitnessCalculation() {
	bool dirty = false;

	this->optimize();

	double val = data.at(0)->getCurrentFitness(dirty);
	// is this the current fitness ? We should at this stage never
	// run across an unevaluated individual.
	if(dirty) {
		std::ostringstream error;
		error << "In GBasePopulation::fitnessCalculation(): Error!" << std::endl
			  << "Came across dirty invididual" << std::endl;

		// throw an exception. Add some information so that if the exception
		// is caught through a base object, no information is lost.
		throw geneva_error_condition(error.str());
	}
	return val;
}

/***********************************************************************************/
/**
 * This helper function marks parents as parents and children as children.
 */
void GBasePopulation::markParents() {
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;
	for(it=data.begin(); it!=data.begin()+nParents_; ++it){
		(*it)->setIsParent();
	}

	for(it=data.begin()+nParents_; it!=data.end(); ++it){
		(*it)->setIsChild();
	}
}

/***********************************************************************************/
/**
 * This helper function lets all individuals know their current generation
 */
void GBasePopulation::markGeneration() {
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;
	for(it=data.begin(); it!=data.end(); ++it){
		(*it)->setParentPopGeneration(generation_);
	}
}

/***********************************************************************************/
/**
 * This helper function lets all individuals know about their position in the
 * population.
 */
void GBasePopulation::markIndividualPositions() {
	std::size_t pos = 0;
	std::vector<boost::shared_ptr<GIndividual> >::iterator it;
	for(it=data.begin(); it!=data.end(); ++it) (*it)->setPopulationPosition(pos++);
}

/***********************************************************************************/
/**
 * Creates a clone of this population that only holds the parent individuals. This function
 * is re-implemented for derived populations, so we get a clone of these classes instead
 * of the base class. This function is used for the micro-training environment
 *
 * @return A smart pointer to a copy of this population, holding only the parent individuals
 */
boost::shared_ptr<GBasePopulation> GBasePopulation::parent_clone() const {
	// Create a copy of this population
	boost::shared_ptr<GBasePopulation> p(new GBasePopulation(*this));

	// Get rid of the child individuals
	p->resize(p->getNParents());

	// Return the result
	return p;
}

/***********************************************************************************/
/**
 * Retrieves the defaultNChildren_ parameter. E.g. in GTransferPopulation::mutateChildren() ,
 * this factor controls when a population is considered to be complete. The corresponding
 * loop which waits for new arrivals will then be stopped, which in turn allows
 * a new generation to start.
 *
 * @return The defaultNChildren_ parameter
 */
std::size_t GBasePopulation::getDefaultNChildren() const {
	return defaultNChildren_;
}

/***********************************************************************************/
/**
 * Retrieves the default size of the population
 *
 * @return The default size of the population
 */
std::size_t GBasePopulation::getDefaultPopulationSize() const {
	return (defaultNChildren_ + nParents_);
}

/***********************************************************************************/
/**
 * This function checks whether a halt criterion has been reached. The most
 * common criterion is the maximum number of generations. Set the maxGeneration_
 * counter to 0 if you want to disable this criterion.
 *
 * @return A boolean indicating whether a halt criterion has been reached
 */
bool GBasePopulation::halt()
{
	// Have we exceeded the maximum number of generations and
	// do we indeed intend to stop in this case ?
	if(maxGeneration_ && (generation_ > maxGeneration_)) return true;

	// Has the optimization stalled too often ?
	if(maxStallGeneration_ && stallCounter_ > maxStallGeneration_) return true;

	// Do we have a scheduled halt time ? The comparatively expensive
	// timedHalt() calculation is only called if maxDuration_
	// is at least one microsecond.
	if(maxDuration_.total_microseconds() && timedHalt()) return true;

	// Are we supposed to stop when the quality has exceeded a threshold ?
	if(hasQualityThreshold_ && qualityHalt()) return true;

	// Has the user specified an additional stop criterion ?
	if(customHalt()) return true;

	// Fine, we can continue.
	return false;
}

/***********************************************************************************/
/**
 * Lets the user set the desired recombination method. No sanity checks for the
 * values are necessary, as we use an enum.
 *
 * @param recombinationMethod The desired recombination method
 */
void GBasePopulation::setRecombinationMethod(const recoScheme& recombinationMethod) {
	recombinationMethod_ = recombinationMethod;
}

/***********************************************************************************/
/**
 * Retrieves the value of the recombinationMethod_ variable
 *
 * @return The value of the recombinationMethod_ variable
 */
recoScheme GBasePopulation::getRecombinationMethod() const {
	return recombinationMethod_;
}

/***********************************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
