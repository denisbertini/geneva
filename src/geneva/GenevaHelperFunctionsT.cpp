/********************************************************************************
 *
 * This file is part of the Geneva library collection. The following license
 * applies to this file:
 *
 * ------------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------------
 *
 * Note that other files in the Geneva library collection may use a different
 * license. Please see the licensing information in each file.
 *
 ********************************************************************************
 *
 * Geneva was started by Dr. Rüdiger Berlich and was later maintained together
 * with Dr. Ariel Garcia under the auspices of Gemfony scientific. For further
 * information on Gemfony scientific, see http://www.gemfomy.eu .
 *
 * The majority of files in Geneva was released under the Apache license v2.0
 * in February 2020.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 ********************************************************************************/

#include "geneva/GenevaHelperFunctionsT.hpp"
#include "geneva/GDoubleGaussAdaptor.hpp"
#include "geneva/GInt32FlipAdaptor.hpp"
#include "geneva/GBooleanAdaptor.hpp"

namespace Gem {
namespace Geneva {

// Specializations for double, std::int32_t and bool
/******************************************************************************/
/**
 * A factory function that returns the default adaptor for the base type "double"
 *
 * @return The default adaptor for the base type "double"
 */
template<>
std::shared_ptr <GAdaptorT<double>> getDefaultAdaptor<double>() {
	return std::shared_ptr<GDoubleGaussAdaptor>(new GDoubleGaussAdaptor());
}

/******************************************************************************/
/**
 * A factory function that returns the default adaptor for the base type "std::int32_t"
 *
 * @return The default adaptor for the base type "std::int32_t"
 */
template<>
std::shared_ptr <GAdaptorT<std::int32_t>> getDefaultAdaptor<std::int32_t>() {
	return std::shared_ptr<GInt32FlipAdaptor>(new GInt32FlipAdaptor());
}

/******************************************************************************/
/**
 * A factory function that returns the default adaptor for the base type "bool"
 *
 * @return The default adaptor for the base type "bool"
 */
template<>
std::shared_ptr <GAdaptorT<bool>> getDefaultAdaptor<bool>() {
	return std::shared_ptr<GBooleanAdaptor>(new GBooleanAdaptor());
}

/******************************************************************************/

} /* namespace Geneva */
} /* namespace Gem */


