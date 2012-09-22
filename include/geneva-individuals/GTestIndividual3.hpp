/**
 * @file GTestIndividual3.hpp
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

// Standard header files go here
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm> // for std::sort

// Boost header files go here
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/cstdint.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_array.hpp>

#ifndef GTESTINDIVIDUAL3_HPP_
#define GTESTINDIVIDUAL3_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Geneva header files go here
#include "geneva/GParameterSet.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GConstrainedDoubleObjectCollection.hpp"
#include "geneva/GConstrainedDoubleObject.hpp"
#include "geneva/GConstrainedDoubleCollection.hpp"
#include "geneva/GInt32FlipAdaptor.hpp"
#include "geneva/GInt32GaussAdaptor.hpp"
#include "geneva/GConstrainedInt32ObjectCollection.hpp"
#include "geneva/GConstrainedInt32Object.hpp"
#include "geneva/GParameterObjectCollection.hpp"
#include "geneva/GOptimizationEnums.hpp"
#include "geneva-individuals/GExternalEvaluatorIndividual.hpp"
#include "common/GExceptions.hpp"
#include "common/GGlobalOptionsT.hpp"
#include "common/GSingletonT.hpp"
#include "common/GParserBuilder.hpp"
#include "common/GFactoryT.hpp"
#include "common/GMathHelperFunctions.hpp"
#include "common/GMathHelperFunctionsT.hpp"

namespace Gem
{
namespace Tests
{

/************************************************************************************************/
/**
 * This individual tests different access methods for parameter objects inside
 * of the individual.
 */
class GTestIndividual3 :public Gem::Geneva::GParameterSet
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int) {
		using boost::serialization::make_nvp;

		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(GParameterSet);
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GTestIndividual3();
	/** @brief The copy constructor */
	GTestIndividual3(const GTestIndividual3&);
	/** @brief The destructor */
	virtual ~GTestIndividual3();

	/** @brief A standard assignment operator */
	const GTestIndividual3& operator=(const GTestIndividual3&);

	/** @brief Checks for equality with another GTestIndividual3 object */
	bool operator==(const GTestIndividual3& cp) const;
	/** @brief Checks for inequality with another GTestIndividual3 object */
	bool operator!=(const GTestIndividual3& cp) const;

	/** @brief Checks whether a given expectation for the relationship between this object and another object is fulfilled */
	virtual boost::optional<std::string> checkRelationshipWith(
			const GObject&,
			const Gem::Common::expectation&,
			const double&,
			const std::string&,
			const std::string&,
			const bool&
	) const;

   /** @brief Get all data members of this class as a plain array */
   boost::shared_array<float> getPlainData() const;

protected:
	/** @brief Loads the data of another GTestIndividual3 */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object */
	virtual GObject* clone_() const;

	/** @brief The actual fitness calculation takes place here. */
	virtual double fitnessCalculation();

#ifdef GEM_TESTING
public:
	/** @brief Applies modifications to this object. */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. */
	virtual void specificTestsFailuresExpected_GUnitTests();

#endif /* GEM_TESTING */
};

} /* namespace Tests */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Tests::GTestIndividual3)

#endif /* GTESTINDIVIDUAL3_HPP_ */
