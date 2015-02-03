/**
 * @file GDoubleObject.hpp
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

// For Microsoft-compatible compilers
#include "common/GWindowsDefines.hpp"

// Standard headers go here

// Boost headers go here

#ifndef GDOUBLEOBJECT_HPP_
#define GDOUBLEOBJECT_HPP_

// Geneva headers go here

#include "geneva/GNumFPT.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"

namespace Gem {
namespace Geneva {

/******************************************************************************/
/**
 * This class encapsulates a double type. This might appear heavy weight,
 * and indeed for most applications this is not the recommended solution -
 * use the GDoubleCollection class or individual GConstrainedDoubleObject objects
 * instead.
 */
class GDoubleObject
	:public GNumFPT<double>
{
	///////////////////////////////////////////////////////////////////////
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive & ar, const unsigned int){
	  using boost::serialization::make_nvp;

	  ar & make_nvp("GNumFPT_double", boost::serialization::base_object<GNumFPT<double> >(*this));
	}
	///////////////////////////////////////////////////////////////////////

public:
	/** @brief The default constructor */
	GDoubleObject();
	/** @brief The copy constructor */
	GDoubleObject(const GDoubleObject&);
	/** @brief Initialization by contained value */
	explicit GDoubleObject(const double&);
	/** @brief Random initialization in a given range */
	GDoubleObject(const double&, const double&);
	/** @brief Initialization with a fixed value and the initialization range */
	GDoubleObject(const double&, const double&, const double&);
	/** @brief The destructor */
	virtual ~GDoubleObject();

	/** @brief An assignment operator for the contained value type */
	virtual double operator=(const double&);

	/** @brief A standard assignment operator */
	const GDoubleObject& operator=(const GDoubleObject&);

	/** @brief Checks for equality with another GDoubleObject object */
	bool operator==(const GDoubleObject&) const;
	/** @brief Checks for inequality with another GDoubleObject object */
	bool operator!=(const GDoubleObject&) const;

	/** @brief Checks whether this object fulfills a given expectation in relation to another object */
	virtual boost::optional<std::string> checkRelationshipWith(
      const GObject&
      , const Gem::Common::expectation&
      , const double&
      , const std::string&
      , const std::string&
      , const bool&
	) const OVERRIDE;

   /** @brief Emits a name for this class / object */
   virtual std::string name() const OVERRIDE;

protected:
	/** @brief Loads the data of another GObject */
	virtual void load_(const GObject*) OVERRIDE;
	/** @brief Creates a deep clone of this object. */
	virtual GObject* clone_() const OVERRIDE;

   /** @brief Attach our local value to the vector. */
   virtual void doubleStreamline(std::vector<double>&, const activityMode& am) const OVERRIDE;
   /** @brief Attach boundaries of type double to the vectors */
   virtual void doubleBoundaries(std::vector<double>&, std::vector<double>&, const activityMode& am) const OVERRIDE;
   /** @brief Tell the audience that we own a double value */
   virtual std::size_t countDoubleParameters(const activityMode& am) const OVERRIDE;
   /** @brief Assigns part of a value vector to the parameter */
   virtual void assignDoubleValueVector(const std::vector<double>&, std::size_t&, const activityMode& am) OVERRIDE;
   /** @brief Attach our local value to the map. */
   virtual void doubleStreamline(std::map<std::string, std::vector<double> >&, const activityMode& am) const OVERRIDE;
   /** @brief Assigns part of a value map to the parameter */
   virtual void assignDoubleValueVectors(const std::map<std::string, std::vector<double> >&, const activityMode& am) OVERRIDE;

   /** @brief Multiplication with a random value in a given range */
   virtual void doubleMultiplyByRandom(const double& min, const double& max, const activityMode& am) OVERRIDE;
   /** @brief Multiplication with a random value in the range [0,1[ */
   virtual void doubleMultiplyByRandom(const activityMode& am) OVERRIDE;
   /** @brief Multiplication with a constant value */
   virtual void doubleMultiplyBy(const double& value, const activityMode& am) OVERRIDE;
   /** @brief Initialization with a constant value */
   virtual void doubleFixedValueInit(const double& value, const activityMode& am) OVERRIDE;
   /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
   virtual void doubleAdd(boost::shared_ptr<GParameterBase>, const activityMode& am) OVERRIDE;
   /** @brief Adds the "same-type" parameters of another GParameterBase object to this one */
   virtual void doubleSubtract(boost::shared_ptr<GParameterBase>, const activityMode& am) OVERRIDE;

public:
	/** @brief Applies modifications to this object. This is needed for testing purposes */
	virtual bool modify_GUnitTests() OVERRIDE;
	/** @brief Performs self tests that are expected to succeed. This is needed for testing purposes */
	virtual void specificTestsNoFailureExpected_GUnitTests() OVERRIDE;
	/** @brief Performs self tests that are expected to fail. This is needed for testing purposes */
	virtual void specificTestsFailuresExpected_GUnitTests() OVERRIDE;
};


/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */

BOOST_CLASS_EXPORT_KEY(Gem::Geneva::GDoubleObject)

#endif /* GDOUBLEOBJECT_HPP_ */
