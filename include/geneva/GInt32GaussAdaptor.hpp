/**
 * @file GInt32GaussAdaptor.hpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt) and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) com
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


// Standard headers go here

// Boost headers go here

#ifndef GINT32GAUSSADAPTOR_HPP_
#define GINT32GAUSSADAPTOR_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva headers go here

#include "geneva/GNumGaussAdaptorT.hpp"

namespace Gem {
namespace Geneva {

/*************************************************************************/
/**
 * The GInt32GaussAdaptor represents an adaptor used for the adaption of
 * boost::int32_t values through the addition of gaussian-distributed random numbers.
 * See the documentation of GAdaptorT<T> for further information on adaptors
 * in the Geneva context. Most functionality is currently implemented in the
 * GNumGaussAdaptorT parent-class. Note that, for the purpose of adapting integer
 * values, it is generally not useful to choose very small sigma values. A value of
 * 1 might be a good choice. Similarly, the minSigma parameter should be set
 * accordingly, so sigma cannot get too small when being adapted.
 */
class GInt32GaussAdaptor
	:public GNumGaussAdaptorT<boost::int32_t>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GNumGaussAdaptorT_int", boost::serialization::base_object<GNumGaussAdaptorT<boost::int32_t> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GInt32GaussAdaptor();
	/** @brief The copy constructor */
	GInt32GaussAdaptor(const GInt32GaussAdaptor&);
	/** @brief Initialization with a adaption probability */
	explicit GInt32GaussAdaptor(const double&);
	/** @brief Initialization with a number of values belonging to the width of the gaussian */
	GInt32GaussAdaptor(const double&, const double&, const double&, const double&);
	/** @brief Initialization with a number of values belonging to the width of the gaussian and the adaption probability */
	GInt32GaussAdaptor(const double&, const double&, const double&, const double&, const double&);
	/** @brief The destructor */
	virtual ~GInt32GaussAdaptor();

	/** @brief A standard assignment operator */
	const GInt32GaussAdaptor& operator=(const GInt32GaussAdaptor&);

	/** @brief Checks for equality with another GInt32GaussAdaptor object */
	bool operator==(const GInt32GaussAdaptor&) const;
	/** @brief Checks for inequality with another GInt32GaussAdaptor object */
	bool operator!=(const GInt32GaussAdaptor&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(const GObject&, const Gem::Common::expectation&, const double&, const std::string&, const std::string&, const bool&) const;

	/** @brief Retrieves the id of this adaptor */
	virtual Gem::Geneva::adaptorId getAdaptorId() const;

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*);
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const;
	/** The actual adaption performed on the value type */
	virtual void customAdaptions(boost::int32_t&);

#ifdef GENEVATESTING
public:
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests();
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests();
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests();
#endif /* GENEVATESTING */
};


} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GInt32GaussAdaptor)

#endif /* GINT32GAUSSADAPTOR_HPP_ */
