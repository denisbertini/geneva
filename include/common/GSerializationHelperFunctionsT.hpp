/**
 * @file GSerializationHelperFunctionsT.hpp
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>

// Boost headers go here

#include <boost/utility.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cast.hpp>
#include <boost/logic/tribool.hpp>

#ifndef GSERIALIZATIONHELPERFUNCTIONST_HPP_
#define GSERIALIZATIONHELPERFUNCTIONST_HPP_

// Geneva headers go here
#include "common/GCommonEnums.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * Converts a shared_ptr<T> into its string representation. This template function thus assumes that
 * T is serializable using the Boost.Serialization framework.
 *
 * @param gt_ptr A shared_ptr to the object to be serialized
 * @param sm The corresponding serialization mode
 * @return A string representation of gt_ptr
 */
template<typename T>
std::string sharedPtrToString(std::shared_ptr<T> gt_ptr, const Gem::Common::serializationMode &serMod) {
	std::ostringstream oarchive_stream;

	switch (serMod) {
		case Gem::Common::serializationMode::SERIALIZATIONMODE_TEXT: {
			boost::archive::text_oarchive oa(oarchive_stream);
			oa << boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
		} // note: explicit scope here is essential so the oa-destructor gets called

			break;

		case Gem::Common::serializationMode::SERIALIZATIONMODE_XML: {
			boost::archive::xml_oarchive oa(oarchive_stream);
			oa << boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
		}
			break;

		case Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY: {
			boost::archive::binary_oarchive oa(oarchive_stream);
			oa << boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
		}

			break;
	}

	return oarchive_stream.str();
}

/******************************************************************************/
/**
 * Loads a shared_ptr<T> from its string representation. This template function thus assumes that
 * T is de-serializable using the Boost.Serialization framework.
 *
 * @param gt_string A string representation of the object to be restored
 * @param sm The corresponding serialization mode
 * @return A shared_ptr to the restored object
 */
template<typename T>
std::shared_ptr<T> sharedPtrFromString(const std::string &gt_string, const Gem::Common::serializationMode &serMod) {
	std::istringstream istr(gt_string);
	std::shared_ptr<T> gt_ptr;

	try {
		switch (serMod) {
			case Gem::Common::serializationMode::SERIALIZATIONMODE_TEXT: {
				boost::archive::text_iarchive ia(istr);
				ia >> boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
			} // note: explicit scope here is essential so the ia-destructor gets called

				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_XML: {
				boost::archive::xml_iarchive ia(istr);
				ia >> boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
			}

				break;

			case Gem::Common::serializationMode::SERIALIZATIONMODE_BINARY: {
				boost::archive::binary_iarchive ia(istr);
				ia >> boost::serialization::make_nvp("classHierarchyFromT_ptr", gt_ptr);
			}
				break;
		}
	} catch (boost::archive::archive_exception &e) {
		glogger
		<< "In sharedPtrFromString(): Error!" << std::endl
		<< "Caught boost::archive::archive_exception" << std::endl
		<< "with message" << std::endl
		<< e.what() << std::endl
		<< "We will return an empty pointer." << std::endl
		<< GWARNING;

		return std::shared_ptr<T>();
	} catch (std::exception& e) {
		glogger
			<< "In sharedPtrFromString(): Error!" << std::endl
			<< "Caught std::exception with message" << std::endl
			<< e.what() << std::endl
			<< "We will return an empty pointer." << std::endl
			<< GWARNING;

		return std::shared_ptr<T>();
	} catch (...) {
		glogger
		<< "In sharedPtrFromString(): Error!" << std::endl
		<< "Caught unknown exception" << std::endl
		<< GEXCEPTION;
	}

	return gt_ptr;
}

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

namespace boost {
namespace serialization {

/******************************************************************************/
/**
 * Takes care of the serialization of enum types. The function will
 * only be instantiated if enum_type is indeed an enum
 */
template<typename Archive, typename enum_type, typename target_type, class=typename std::enable_if<std::is_enum<enum_type>::value>::type>
void serializeEnumAs(
	Archive& ar
	, enum_type &e
) {
	if (Archive::is_saving::value) { // see the oarchive interface e.g. here: http://www.boost.org/doc/libs/1_61_0/libs/serialization/doc/archives.html
		target_type t = static_cast<target_type>(e);
		ar & t;
	} else {
		target_type t;
		ar & t;
		e = static_cast<enum_type>(t);
	}
}

/******************************************************************************/
/**
 * Saves a tribool variable to an archive
 */
template<typename Archive>
void save(
	Archive &ar
	, const boost::logic::tribool &val
	, unsigned int
) {
	Gem::Common::triboolStates tbs = Gem::Common::triboolStates::TBS_FALSE;
	if (val == true)
		tbs = Gem::Common::triboolStates::TBS_TRUE;
	else if (boost::logic::indeterminate(val))
		tbs = Gem::Common::triboolStates::TBS_INDETERMINATE;

	ar &make_nvp("tbs", tbs);
}

/******************************************************************************/
/**
 * Loads a tribool variable from an archive
 */
template<typename Archive>
void load(
	Archive &ar
	, boost::logic::tribool &val
	, unsigned int
) {
	Gem::Common::triboolStates tbs = Gem::Common::triboolStates::TBS_FALSE;
	ar &make_nvp("tbs", tbs);

	switch (tbs) {
		case Gem::Common::triboolStates::TBS_FALSE:
			val = false;
			break;

		case Gem::Common::triboolStates::TBS_TRUE:
			val = true;
			break;

		case Gem::Common::triboolStates::TBS_INDETERMINATE:
			val = boost::logic::indeterminate;
			break;
	};
}

/******************************************************************************/
/**
 * Saves a std::chrono::duration<double> variable to an archive
 */
template<typename Archive>
void save(
	Archive &ar
	, const std::chrono::duration<double> &val
	, unsigned int
) {
	double chrono_duration = val.count();
	ar & make_nvp("chrono_duration", chrono_duration);
}

/******************************************************************************/
/**
 * Loads a tribool variable from an archive
 */
template<typename Archive>
void load(
	Archive &ar
	, std::chrono::duration<double> &val
	, unsigned int
) {
	double chrono_duration = 0.;
	ar &make_nvp("chrono_duration", chrono_duration);
	val = std::chrono::duration<double>(chrono_duration);
}

/******************************************************************************/

} /* namespace serialization */
} /* namespace boost */

/******************************************************************************/
/**
 * Needed so Boost.Serialization does not search for a serialize function
 */
BOOST_SERIALIZATION_SPLIT_FREE(boost::logic::tribool)
BOOST_SERIALIZATION_SPLIT_FREE(std::chrono::duration<double>)

/******************************************************************************/

#endif /* GSERIALIZATIONHELPERFUNCTIONST_HPP_ */
