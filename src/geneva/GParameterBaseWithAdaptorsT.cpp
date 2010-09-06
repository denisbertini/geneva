/**
 * @file GParameterBaseWithAdaptorsT.cpp
 */

/*
 * Copyright (C) Authors of the Geneva library collection and Karlsruhe
 * Institute of Technology (University of the State of Baden-Wuerttemberg
 * and National Laboratory of the Helmholtz Association).
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: info [at] gemfony (dot) com
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

#include "geneva/GParameterBaseWithAdaptorsT.hpp"

namespace Gem {
namespace Geneva {

/***********************************************************************************************/
/////////////////// Specialization for std::vector<bool> ////////////////////////////////////////
/***********************************************************************************************/
/**
 * This function applies the first adaptor of the adaptor sequence to a collection of values.
 * Note that the parameter of this function will get changed. This is a specialization of a
 * generic template function.
 *
 * @param collection A vector of values that shall be adapted
 */
template<>
void GParameterBaseWithAdaptorsT<bool>::applyAdaptor(std::vector<bool>& collection) {
#ifdef DEBUG
		if(!adaptor_) {
			std::ostringstream error;
			error << "In GParameterBaseWithAdaptorsT<T>::applyAdaptor(std::vector<bool>& collection):" << std::endl
				  << "Error: No adaptor was found." << std::endl;
			throw Gem::Common::gemfony_error_condition(error.str());
		}
#endif /* DEBUG */

	std::vector<bool>::iterator it;
	for (it = collection.begin(); it != collection.end(); ++it)	{
		bool value = *it;
		adaptor_->adapt(value);
		*it = value;
	}
}

/***********************************************************************************************/

} /* namespace Geneva  */
} /* namespace Gem */
