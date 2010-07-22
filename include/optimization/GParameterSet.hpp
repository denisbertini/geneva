/**
 * @file GParameterSet.hpp
 */

/* Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of Technology
 * (University of the State of Baden-Wuerttemberg and National Laboratory
 * of the Helmholtz Association)
 *
 * Contact: info [at] gemfony (dot) com
 *
 * This file is part of the Geneva library, Gemfony scientific's optimization
 * library.
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
#include <sstream>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost header files go here

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/function.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

#ifndef GPARAMETERSET_HPP_
#define GPARAMETERSET_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// GenEvA headers go here
#include "GMutableSetT.hpp"
#include "GObject.hpp"
#include "GParameterBase.hpp"
#include "common/GExceptions.hpp"

#ifdef GENEVATESTING
#include "GUnitTestFrameworkT.hpp"
#endif /* GENEVATESTING */

namespace Gem {
namespace GenEvA {

/**************************************************************************/
/**
 * This class implements a collection of GParameterBase objects. It
 * will form the basis of many user-defined individuals.
 */
class GParameterSet:
	public GMutableSetT<Gem::GenEvA::GParameterBase>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;
	  ar & make_nvp("GMutableSetT_GParameterBase",
			  boost::serialization::base_object<GMutableSetT<Gem::GenEvA::GParameterBase> >(*this));
	}
	///////////////////////////////////////////////////////////////////////
public:
	/** @brief The default constructor */
	GParameterSet();
	/** @brief The copy constructor */
	GParameterSet(const GParameterSet&);
	/** @brief The destructor */
	virtual ~GParameterSet();

	/** @brief Standard assignment operator */
	const GParameterSet& operator=(const GParameterSet&);

	/** @brief Checks for equality with another GParameterSet object */
	bool operator==(const GParameterSet&) const;
	/** @brief Checks for inequality with another GParameterSet object */
	bool operator!=(const GParameterSet&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Common::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

	/** @brief Registers an evaluation function */
	void registerEvaluator(const boost::function<double (const GParameterSet&)>&);

	/** @brief Allows to randomly initialize parameter members */
	void randomInit();
	/** @brief Initializes double-based parameters with a given value */
	void fixedValueInit(const double&);
	/** @brief Multiplies double-based parameters with a given value */
	void multiplyBy(const double&);

	/**********************************************************************/
	/**
	 * This function returns a parameter set at a given position of the data set.
	 * If compiled in DEBUG mode, it also checks that the accesses position matches
	 * the size of the vector. Note that this function will only be accessible to the
	 * compiler if parameter_type is a derivative of GParameterBase, thanks to the magic
	 * of Boost's enable_if and Type Traits libraries.
	 *
	 * @param pos The position in our data array that shall be converted
	 * @return A converted version of the GParameterBase object, as required by the user
	 */
	template <typename parameter_type>
	inline const boost::shared_ptr<parameter_type> pc_at(
			  const std::size_t& pos
			, typename boost::enable_if<boost::is_base_of<GParameterBase, parameter_type> >::type* dummy = 0
	)  const {
#ifdef DEBUG
		if(pos >= data.size()) {
			std::ostringstream error;
			error << "In GParameterSet::pc_at<>() : Error" << std::endl
				  << "Tried to access index " << pos << " which is >= the size " << data.size() << " of the vector." << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}

		boost::shared_ptr<parameter_type> p = boost::static_pointer_cast<parameter_type>(data[pos]);

		if(p) return p;
		else {
			std::ostringstream error;
			error << "In GParameterSet::pc_at<>() : Conversion error" << std::endl;
			throw(Gem::Common::gemfony_error_condition(error.str()));
		}
#else
		return boost::static_pointer_cast<parameter_type>(data[pos]);
#endif /* DEBUG */
	}

	/**************************************************************************************************/
	/** @brief Triggers updates when the optimization process has stalled */
	virtual bool updateOnStall();

#ifdef GENEVATESTING
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests();
#endif /* GENEVATESTING */

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object */
	virtual GObject* clone_() const;

	/** @brief The actual fitness calculation takes place here */
	virtual double fitnessCalculation();

	/** @brief Allows to store an evaluation function for this object */
	boost::function<double (const GParameterSet&)> eval_;
};

} /* namespace GenEvA */
} /* namespace Gem */

#ifdef GENEVATESTING
// Tests of this class (and parent classes)
/*************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************************/
/** @brief We need to provide a specialization of the factory function that creates objects of this type. */
template <> boost::shared_ptr<Gem::GenEvA::GParameterSet> TFactory_GUnitTests<Gem::GenEvA::GParameterSet>();

/*************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************************/
#endif /* GENEVATESTING */

#endif /* GPARAMETERSET_HPP_ */
