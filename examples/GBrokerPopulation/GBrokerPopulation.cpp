/**
 * @file GBrokerPopulation.cpp
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

// Standard header files go here
#include <iostream>
#include <iterator>
#include <cmath>
#include <sstream>
#include <string>

// Boost header files go here
#include <boost/asio.hpp>
#include <boost/function.hpp>

// GenEvA header files go here
#include "GRandom.hpp"
#include "GBasePopulation.hpp"
#include "GDoubleCollection.hpp"
#include "GParameterSet.hpp"
#include "GDoubleGaussAdaptor.hpp"
#include "GLogger.hpp"
#include "GLogTargets.hpp"
#include "GBrokerPopulation.hpp"
#include "GIndividualBroker.hpp"
#include "GAsioTCPConsumer.hpp"
#include "GAsioTCPClient.hpp"

// The individual that should be optimized
// This is a simple parabola
#include "GParabolaIndividual.hpp"

// Parses the command line for all required options
#include "GCommandLineParser.hpp"

using namespace Gem::GenEvA;
using namespace Gem::Util;
using namespace Gem::GLogFramework;

/************************************************************************************************/
/**
 * The main function. We search for the minimum of a parabola, with the help of multiple clients,
 * possibly running on different machines.
 */
int main(int argc, char **argv){
	std::string mode, ip;
	unsigned short port=10000;
	std::size_t parabolaDimension, populationSize, nParents;
	double parabolaMin, parabolaMax;
	boost::uint16_t nProducerThreads;
	boost::uint32_t maxGenerations, reportGeneration;
	long maxMinutes;
	bool verbose;
	recoScheme rScheme;

	// Retrieve command line options
	if(!parseCommandLine(argc, argv,
						 mode,
						 port,
				         parabolaDimension,
				         parabolaMin,
				         parabolaMax,
				         nProducerThreads,
				         populationSize,
				         nParents,
				         maxGenerations,
				         maxMinutes,
				         reportGeneration,
				         rScheme,
				         verbose))
		{ std::terminate(); }

	// Add some log levels to the logger
	LOGGER.addLogLevel(Gem::GLogFramework::CRITICAL);
	LOGGER.addLogLevel(Gem::GLogFramework::WARNING);
	LOGGER.addLogLevel(Gem::GLogFramework::INFORMATIONAL);
	LOGGER.addLogLevel(Gem::GLogFramework::PROGRESS);

	// Add log targets to the system
	LOGGER.addTarget(boost::shared_ptr<GBaseLogTarget>(new GDiskLogger("GBrokerPopulation.log")));
	LOGGER.addTarget(boost::shared_ptr<GBaseLogTarget>(new GConsoleLogger()));

	// Random numbers are our most valuable good. Set the number of threads
	GRANDOMFACTORY.setNProducerThreads(nProducerThreads);

	if(mode == "server"){
		// Create a consumer and enrol it with the broker
		boost::shared_ptr<GAsioTCPConsumer> gatc(new GAsioTCPConsumer(port));
		GINDIVIDUALBROKER.enrol(gatc);

		// Set up a single parabola individual
		boost::shared_ptr<GParabolaIndividual> parabolaIndividual(new GParabolaIndividual(parabolaDimension, parabolaMin,parabolaMax));

		// Create the actual population
		GBrokerPopulation pop;

		// Make the individual known to the population
		pop.append(parabolaIndividual);

		// Specify some population settings
		pop.setPopulationSize(populationSize,nParents);
		pop.setMaxGeneration(maxGenerations);
		pop.setMaxTime(boost::posix_time::minutes(maxMinutes));
		pop.setReportGeneration(reportGeneration);
		pop.setRecombinationMethod(rScheme);

		// Do the actual optimization
		pop.optimize();
	}
	else if(mode == "client"){
		// Just start the client with the required parameters
	    GAsioTCPClient gasiotcpclient(ip,boost::lexical_cast<std::string>(port));
	    gasiotcpclient.run();
	}

	std::cout << "Done ..." << std::endl;

	return 0;
}
