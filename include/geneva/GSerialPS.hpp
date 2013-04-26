/**
 * @file GSerialPS.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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
 * http://www.gemfony.com .
 */

// Standard headers go here

// Boost headers go here


#ifndef GSERIALPS_HPP_
#define GSERIALPS_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GHelperFunctions.hpp"
#include "geneva/GBasePS.hpp"
#include "geneva/GIndividual.hpp"
#include "geneva/GObject.hpp"

#ifdef GEM_TESTING
#include "geneva-individuals/GTestIndividual1.hpp"
#endif /* GEM_TESTING */

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * A serial gradient descent
 */
class GSerialPS :public GBasePS
{
   ///////////////////////////////////////////////////////////////////////
   friend class boost::serialization::access;

   template<typename Archive>
   void serialize(Archive & ar, const unsigned int) {
      using boost::serialization::make_nvp;

      ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GBasePS);
   }

   ///////////////////////////////////////////////////////////////////////

public:
   /** @brief The default constructor */
   GSerialPS();
   /** @brief A standard copy constructor */
   GSerialPS(const GSerialPS&);
   /** @brief The destructor */
   virtual ~GSerialPS();

   /** @brief A standard assignment operator */
   const GSerialPS& operator=(const GSerialPS&);

   /** @brief Checks for equality with another GSerialPS object */
   bool operator==(const GSerialPS&) const;
   /** @brief Checks for inequality with another GSerialPS object */
   bool operator!=(const GSerialPS&) const;

   /** @brief Checks whether this object fulfills a given expectation in relation to another object */
   virtual boost::optional<std::string> checkRelationshipWith(
         const GObject&
         , const Gem::Common::expectation&
         , const double&
         , const std::string&
         , const std::string&
         , const bool&
   ) const;

   /** @brief Adds local configuration options to a GParserBuilder object */
   virtual void addConfigurationOptions (
      Gem::Common::GParserBuilder& gpb
      , const bool& showOrigin
   );

   /** @brief Allows to assign a name to the role of this individual(-derivative) */
   virtual std::string getIndividualCharacteristic() const;

   /** @brief Emits a name for this class / object */
   virtual std::string name() const;

protected:
   /** @brief Loads the data of another population */
   virtual void load_(const GObject *);
   /** @brief Creates a deep clone of this object */
   virtual GObject *clone_() const;

   virtual void init();
   /** @brief Does any necessary finalization work */
   virtual void finalize();

   /** @brief Triggers fitness calculation of a number of individuals */
   virtual double doFitnessCalculation(const std::size_t&);

public:
   /***************************************************************************/
   /** @brief Applies modifications to this object. This is needed for testing purposes */
   virtual bool modify_GUnitTests();
   /** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
   virtual void specificTestsNoFailureExpected_GUnitTests();
   /** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
   virtual void specificTestsFailuresExpected_GUnitTests();
};

} /* namespace Geneva */
} /* namespace Gem */

/******************************************************************************/

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GSerialPS)

/******************************************************************************/

#endif /* GSERIALPS_HPP_ */