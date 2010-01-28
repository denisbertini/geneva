/**
 * @file GBooleanCollection.cpp
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

#include "GBooleanCollection.hpp"

// Included here so no conflicts occur. See explanation at
// http://www.boost.org/libs/serialization/doc/special.html#derivedpointers
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Gem::GenEvA::GBooleanCollection)

namespace Gem
{
namespace GenEvA
{
  /**********************************************************************/
  /**
   * The standard constructor. As we have no local data, all work is done
   * by the parent class.
   */
  GBooleanCollection::GBooleanCollection()
	: GParameterCollectionT<bool>()
  { /* nothing */ }

  /**********************************************************************/
  /**
   * Initializes the class with a set of nval random bits.
   *
   * @param nval The size of the collection
   */
  GBooleanCollection::GBooleanCollection(const std::size_t& nval)
    : GParameterCollectionT<bool>()
  {
    this->addRandomData(nval);
  }

  /**********************************************************************/
  /**
   * Initializes the class with nval random bits, of which probability percent
   * have the value true
   *
   * @param nval The size of the collection
   * @param probability The probability for true values in the collection
   */
  GBooleanCollection::GBooleanCollection(const std::size_t& nval, const double& probability)
	: GParameterCollectionT<bool>()
  {
	this->addRandomData(nval, probability);
  }

  /**********************************************************************/
  /**
   * No local data, hence we can rely on the parent class.
   *
   * @param cp A copy of another GBooleanCollection object
   */
  GBooleanCollection::GBooleanCollection(const GBooleanCollection& cp)
    : GParameterCollectionT<bool>(cp)
  { /* nothing */ }

  /**********************************************************************/
  /**
   * The standard destructor. No local data, hence it is empty.
   */
  GBooleanCollection::~GBooleanCollection()
  { /* nothing */ }

  /**********************************************************************/
  /**
   * A standard assignment operator for  GBooleanCollection objects.
   *
   * @param cp A copy of another GBooleanCollection object
   */
  const GBooleanCollection& GBooleanCollection::operator=(const GBooleanCollection& cp){
    GBooleanCollection::load(&cp);
    return *this;
  }

  /**********************************************************************/
  /**
   * Creates a deep clone of this object
   *
   * @return A deep clone of this object
   */
  GObject *GBooleanCollection::clone_() const{
	  return new GBooleanCollection(*this);
  }

  /**********************************************************************/
  /**
   * Loads the data of another GBooleanCollection object, camouflaged as
   * a GObject.
   *
   * @param gb A pointer to another GBooleanCollection object, camouflaged as a GObject
   */
  void GBooleanCollection::load(const GObject * cp){
    // Check that this object is not accidently assigned to itself.
    // As we do not actually do any calls with this pointer, we
    // can use the faster static_cast<>
    if(static_cast<const GBooleanCollection *>(cp) == this){
		std::ostringstream error;
		error << "In GBooleanCollection::load() : Error!" << std::endl
			  << "Tried to assign an object to itself." << std::endl;

		throw geneva_error_condition(error.str());
    }

    GParameterCollectionT<bool>::load(cp);
  }

  /**********************************************************************/
  /**
   * Adds random bits to the collection, 50% of which have the value false.
   *
   * @param nval The number of boolean values to add to the collection
   */
  void GBooleanCollection::addRandomData(const std::size_t& nval){
	  Gem::Util::GRandom gr;
	  gr.setRnrGenerationMode(Gem::Util::RNRLOCAL);
	  for(std::size_t i= 0; i<nval; i++) this->push_back(gr.boolRandom());
  }

  /**********************************************************************/
  /**
   * Adds random bits to the collection with a given probability structure.
   *
   * @param nval The number of boolean values to add to the collection
   * @param probability The probability for true values in the collection
   */
  void GBooleanCollection::addRandomData(const std::size_t& nval, const double& probability){
	  Gem::Util::GRandom gr;
	  gr.setRnrGenerationMode(Gem::Util::RNRLOCAL);
	  for(std::size_t i= 0; i<nval; i++) this->push_back(gr.boolRandom(probability));
  }

  /**********************************************************************/
  /**
   * Checks for equality with another GBooleanCollection object
   *
   * @param  cp A constant reference to another GBooleanCollection object
   * @return A boolean indicating whether both objects are equal
   */
  bool GBooleanCollection::operator==(const GBooleanCollection& cp) const {
		using namespace Gem::Util;
		// Means: The expectation of equality was fulfilled, if no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_EQUALITY, 0.,"GBooleanCollection::operator==","cp", CE_SILENT);
  }

  /**********************************************************************/
  /**
   * Checks for inequality with another GBooleanCollection object
   *
   * @param  cp A constant reference to another GBooleanCollection object
   * @return A boolean indicating whether both objects are inequal
   */
  bool GBooleanCollection::operator!=(const GBooleanCollection& cp) const {
		using namespace Gem::Util;
		// Means: The expectation of inequality was fulfilled, if no error text was emitted (which converts to "true")
		return !checkRelationshipWith(cp, CE_INEQUALITY, 0.,"GBooleanCollection::operator!=","cp", CE_SILENT);
  }

  /**********************************************************************/
  /**
   * Checks whether a given expectation for the relationship between this object and another object
   * is fulfilled.
   *
   * @param cp A constant reference to another object, camouflaged as a GObject
   * @param e The expected outcome of the comparison
   * @param limit The maximum deviation for floating point values (important for similarity checks)
   * @param caller An identifier for the calling entity
   * @param y_name An identifier for the object that should be compared to this one
   * @param withMessages Whether or not information should be emitted in case of deviations from the expected outcome
   * @return A boost::optional<std::string> object that holds a descriptive string if expectations were not met
   */
  boost::optional<std::string> GBooleanCollection::checkRelationshipWith(const GObject& cp,
  		const Gem::Util::expectation& e,
  		const double& limit,
  		const std::string& caller,
  		const std::string& y_name,
  		const bool& withMessages) const
  {
      using namespace Gem::Util;
      using namespace Gem::Util::POD;

  	// Check that we are indeed dealing with a GParamterBase reference
  	const GBooleanCollection *p_load = GObject::conversion_cast(&cp,  this);

  	// Will hold possible deviations from the expectation, including explanations
      std::vector<boost::optional<std::string> > deviations;

  	// Check our parent class'es data ...
  	deviations.push_back(GParameterCollectionT<bool>::checkRelationshipWith(cp, e, limit, "GBooleanCollection", y_name, withMessages));

  	// no local data ...

  	return POD::evaluateDiscrepancies("GBooleanCollection", caller, deviations, e);
  }

  /**********************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
