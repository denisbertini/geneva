/**
 * @file GHelperFunctionsT.hpp
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
#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <typeinfo>

// Includes check for correct Boost version(s)
#include "GGlobalDefines.hpp"

// Boost headers go here

#include <boost/cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/math/special_functions/next.hpp>
#include <boost/date_time.hpp>
#include <boost/variant.hpp>
#include <boost/limits.hpp>
#include <boost/type_traits.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/utility/enable_if.hpp>

#ifndef GHELPERFUNCTIONST_HPP_
#define GHELPERFUNCTIONST_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif

// Our own headers go here
#include "common/GHelperFunctions.hpp"
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This function takes two smart pointers and copies their contents (if any). Note that this
 * function might yield bad results for virtual types and will not work for purely virtual types.
 *
 * @param from The source smart pointer
 * @param to The target smart pointer
 */
template <typename T>
void copySmartPointer (
	const boost::shared_ptr<T>& from
	, boost::shared_ptr<T>& to
) {
	// Make sure to is empty when from is empty
	if(!from) {
		to.reset();
	} else {
		if(!to) {
			to.reset(new T(*from));
		} else {
			*to = *from;
		}
	}
}

/******************************************************************************/
/**
 * This function takes two vectors of boost::shared_ptr smart pointers and copies
 * one into the other. As we want to make a deep copy of the smart pointers' contents
 * this can be quite complicated. Note that we assume here that the objects pointed to
 * can be copied using an operator=(). The function also assumes the existence of
 * a valid copy constructor.  Note that this function might yield bad results for
 * virtual types.
 *
 * @param from The vector used as the source of the copying
 * @param to The vector used as the target of the copying
 */
template <typename T>
void copySmartPointerVector(
		const std::vector<boost::shared_ptr<T> >& from
		, std::vector<boost::shared_ptr<T> >& to
) {
	typename std::vector<boost::shared_ptr<T> >::const_iterator it_from;
	typename std::vector<boost::shared_ptr<T> >::iterator it_to;

	std::size_t size_from = from.size();
	std::size_t size_to = to.size();

	if(size_from==size_to) { // The most likely case
		for(it_from=from.begin(), it_to=to.begin(); it_to!=to.end(); ++it_from, ++it_to) {
			**it_to=**it_from; // Uses T::operator=()
		}
	}
	else if(size_from > size_to) {
		// First copy the data of the first size_to items
		for(it_from=from.begin(), it_to=to.begin(); it_to!=to.end(); ++it_from, ++it_to) {
			**it_to=**it_from;
		}

		// Then attach copies of the remaining items
		for(it_from=from.begin()+size_to; it_from!=from.end(); ++it_from) {
			boost::shared_ptr<T> p(new T(**it_from));
			to.push_back(p);
		}
	}
	else if(size_from < size_to) {
		// First copy the initial size_foreight items over
		for(it_from=from.begin(), it_to=to.begin(); it_from!=from.end(); ++it_from, ++it_to) {
			**it_to=**it_from;
		}

		// Then resize the local vector. Surplus items will vanish
		to.resize(size_from);
	}
}

/******************************************************************************/
/**
 * This function converts a smart pointer to a target type, throwing an exception
 * if the conversion cannot be done.
 */
template <typename source_type, typename target_type>
boost::shared_ptr<target_type> convertSmartPointer(boost::shared_ptr<source_type> p_raw) {
#ifdef DEBUG
      // Check that we have indeed been given an item and that the pointer isn't empty
      if(!p_raw) {
         glogger
         << "In boost::shared_ptr<dT> convertSmartPointer(boost::shared_ptr<source_type> p_raw) :" << std::endl
         << "Error: Pointer is empty." << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return boost::shared_ptr<target_type>();
      }

      // Do the actual conversion
      boost::shared_ptr<target_type> p = boost::dynamic_pointer_cast<target_type>(p_raw);
      if(p) return p;
      else {
         glogger
         << "In boost::shared_ptr<dT> convertSmartPointer(boost::shared_ptr<source_type> p_raw) :" << std::endl
         << "Error: Invalid conversion" << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return boost::shared_ptr<target_type>();
      }
#else
      return boost::static_pointer_cast<target_type>(p_raw);
#endif /* DEBUG */
}

/******************************************************************************/
/**
 * This function converts a simple pointer to a target type, throwing an exception
 * if the conversion cannot be done.
 */
template <typename source_type, typename target_type>
target_type *convertSimplePointer(source_type *p_raw) {
#ifdef DEBUG
      // Check that we have indeed been given an item and that the pointer isn't empty
      if(!p_raw) {
         glogger
         << "In target_type * convertSimplePointer(source_type *p_raw) :" << std::endl
         << "Error: Pointer is empty." << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return (target_type *)(NULL);
      }

      // Do the actual conversion
      target_type  *p = dynamic_cast<target_type>(p_raw);
      if(p) return p;
      else {
         glogger
         << "In target_type * convertSimplePointer(source_type * p_raw) :" << std::endl
         << "Error: Invalid conversion" << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return (target_type *)(NULL);
      }
#else
      return static_cast<target_type>(p_raw);
#endif /* DEBUG */
}

/******************************************************************************/
/**
 * This function converts a simple pointer to a target type, throwing an exception
 * if the conversion cannot be done.
 */
template <typename source_type, typename target_type>
const target_type *convertSimplePointer(const source_type *p_raw) {
#ifdef DEBUG
      // Check that we have indeed been given an item and that the pointer isn't empty
      if(!p_raw) {
         glogger
         << "In const target_type * convertSimplePointer(const source_type *p_raw) :" << std::endl
         << "Error: Pointer is empty." << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return (const target_type *)(NULL);
      }

      // Do the actual conversion
      const target_type  *p = dynamic_cast<const target_type *>(p_raw);
      if(p) return p;
      else {
         glogger
         << "In target_type * convertSimplePointer(source_type * p_raw) :" << std::endl
         << "Error: Invalid conversion" << std::endl
         << GEXCEPTION;

         // Make the compiler happy
         return (const target_type *)(NULL);
      }
#else
      return static_cast<const target_type *>(p_raw);
#endif /* DEBUG */
}

/******************************************************************************/
/**
 * Splits a string into a vector of user-defined types, according to a seperator character.
 * The only precondition is that the target type is known to boost::lexical_cast, which can
 * be achieved simply by providing related operator<< and operator>> .
 */
template <typename split_type>
std::vector<split_type> splitStringT(const std::string& raw, const char* sep) {
   std::vector<std::string> fragments = Gem::Common::splitString(raw, sep);
   std::vector<split_type> result;
   std::vector<std::string>::iterator it;
   for(it=fragments.begin(); it!=fragments.end(); ++it) {
      result.push_back(boost::lexical_cast<split_type>(*it));
   }
   return result;
}

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GHELPERFUNCTIONST_HPP_ */
