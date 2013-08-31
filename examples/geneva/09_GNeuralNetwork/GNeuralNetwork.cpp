/**
 * @file GNeuralNetwork.cpp
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


// Standard header files go here
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

// Boost header files go here
#include <boost/lexical_cast.hpp>

// Geneva header files go here
#include <geneva/Go2.hpp>

// The individual that should be optimized
#include <geneva-individuals/GNeuralNetworkIndividual.hpp>

using namespace Gem::Geneva;
using namespace Gem::Courtier;
using namespace Gem::Common;
using namespace Gem::Hap;
namespace po = boost::program_options;

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Retrieves additional command line options
 */
std::vector<boost::shared_ptr<po::option_description> > getCustomCLOptions(
   trainingDataType& tdt
   , std::string& trainingDataFile
   , std::string& architecture
   , std::size_t& nDataSets
   , std::string& resultProgram
   , std::string& visualizationFile
) {
   std::vector<boost::shared_ptr<po::option_description> > od;

   boost::shared_ptr<po::option_description> tdt_option(
      new po::option_description(
         "traininDataType"
         , po::value<trainingDataType>(&tdt)->default_value(Gem::Geneva::TDTNONE)
         , "Specify training data to be produced: HYPERCUBE=1, HYPERSPHERE=2, AXISCENTRIC=3, SINUS=4"
      )
   );

   boost::shared_ptr<po::option_description> tdf_option(
      new po::option_description(
         "trainingDataFile"
         , po::value<std::string>(&trainingDataFile)->default_value(trainingDataFile)
         , "The name of the file to which training data should be written"
      )
   );

   boost::shared_ptr<po::option_description> architecture_option(
      new po::option_description(
         "architecture"
         , po::value<std::string>(&architecture)->default_value(architecture)
         , "The architecture of the network"
      )
   );

   boost::shared_ptr<po::option_description> nDataSets_option(
      new po::option_description(
         "nDataSets"
         , po::value<std::size_t>(&nDataSets)->default_value(nDataSets)
         , "The number of data sets to be produced"
      )
   );

   boost::shared_ptr<po::option_description> resultProgram_option(
      new po::option_description(
         "resultProgram"
         , po::value<std::string>(&resultProgram)->default_value(resultProgram)
         , "The name of the result program"
      )
   );

   boost::shared_ptr<po::option_description> visualizationFile_option(
      new po::option_description(
         "visualizationFile"
         , po::value<std::string>(&visualizationFile)->default_value(visualizationFile)
         , "The name of the visualization file"
      )
   );

   od.push_back(tdt_option);
   od.push_back(tdf_option);
   od.push_back(architecture_option);
   od.push_back(nDataSets_option);
   od.push_back(resultProgram_option);
   od.push_back(visualizationFile_option);

   return od;
}

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The main function.
 */
int main(int argc, char **argv){
   //---------------------------------------------------------------------------
   // Assemble additional command line options to be passed to Go2
   trainingDataType tdt = Gem::Geneva::TDTNONE;
   std::string trainingDataFile = "./DataSets/hyper_sphere.dat";
   std::string architecture = "2-4-4-1"; // two input nodes, one output node, two hidden layers with 4 nodes each
   std::size_t nDataSets = 2000;
   std::string resultProgram = "trainedNetwork.hpp";
   std::string visualizationFile = "visualization.C";

   std::vector<boost::shared_ptr<po::option_description> > od = getCustomCLOptions(
      tdt
      , trainingDataFile
      , architecture
      , nDataSets
      , resultProgram
      , visualizationFile
   );

   //---------------------------------------------------------------------------
   // Create the main optimizer-wrapper
   Go2 go(argc, argv, "./config/Go2.json", od);

   //---------------------------------------------------------------------------
   // Produce data sets if we have been asked to do so, then leave
   if(tdt != Gem::Geneva::TDTNONE) {
      GNeuralNetworkIndividual::createNetworkData(tdt, trainingDataFile, architecture, nDataSets);
      return 0;
   } else { // Store the trainingDataFile in the global options, so they can be accessed by the individuals
      GNeuralNetworkOptions->set("trainingDataFile", trainingDataFile);
   }

   //---------------------------------------------------------------------------
   // Client mode
   if(go.clientMode()) {
      return go.clientRun();
   } // Execution will end here in client mode

   //---------------------------------------------------------------------------

   // Create a factory for GNeuralNetworkIndividual objects and perform
   // any necessary initial work.
   boost::shared_ptr<GNeuralNetworkIndividualFactory>
      gnn_ptr(new GNeuralNetworkIndividualFactory("./config/GNeuralNetworkIndividual.json"));

   // Add a content creator so Go2 can generate its own individuals, if necessary
   go.registerContentCreator(gnn_ptr);

   // Perform the actual optimization and retrieve the best individual
   boost::shared_ptr<GNeuralNetworkIndividual> p = go.optimize<GNeuralNetworkIndividual>();

   //---------------------------------------------------------------------------
   // Output the result- and the visualization-program (if available)
   p->writeTrainedNetwork(resultProgram);
   p->writeVisualizationFile(visualizationFile);

   // Terminate Geneva
   return 0;
}

/******************************************************************************/

