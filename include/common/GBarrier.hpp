/**
 * @file
 */

/*
 * This file is part of the Geneva library collection.
 *
 * Note: this class was adapted from boost/barrier.hpp and "translated" to C++11.
 * It is consequently covered by the Boost license v.1.0, as quoted further below.
 *
 * As allowed by the license, modifications were applied to the code.
 * These modifications are also covered by the Boost Software License,
 * version 1.0, and are Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY COLLECTION, UNLESS THIS IS EXPLICITLY STATED IN THE
 * CORRESPONDING FILE!
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
 *
 * Geneva was developed with kind support from Karlsruhe Institute of
 * Technology (KIT) and Steinbuch Centre for Computing (SCC). Further
 * information about KIT and SCC can be found at http://www.kit.edu/english
 * and http://scc.kit.edu .
 *
 * Geneva is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Boost Software License for more details.
 *
 * For further information on Gemfony scientific and Geneva, visit
 * http://www.gemfony.eu .
 */

/*
 * The following license applies to the code in this file:
 *
 * ***************************************************************************
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ***************************************************************************
 */

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

// Boost headers go here

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GErrorStreamer.hpp"
#include "common/GCommonEnums.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A simple barrier for threads. Execution wlll be blocked until the required
 * number of threads has called the wait() function.
 */
class GBarrier {
public:
	 explicit GBarrier(std::uint32_t count) noexcept(false) : m_count(count) {
	 	if(0==count) {
			throw gemfony_exception(
				g_error_streamer(DO_LOG, time_and_place)
					<< "In GBarrier::GBarrier(): Error!" << std::endl
					<< "count cannot be 0" << std::endl
			);
	 	}
	 }

	 ~GBarrier() = default;

	 GBarrier() = delete;
	 GBarrier(GBarrier const&) = delete;
	 GBarrier(GBarrier&&) = delete;
	 GBarrier& operator=(GBarrier const&) = delete;
	 GBarrier& operator=(GBarrier&&) = delete;

	 bool
	 wait() {
		 std::unique_lock<std::mutex> lock(m_mutex);
		 std::uint32_t gen = m_generation;

		 if (--m_count == 0) {
			 m_generation++;
			 m_count = m_count_start;
			 lock.unlock();
			 m_cond.notify_all();
			 return true;
		 }

		 m_cond.wait(
		 	lock
		 	, [&]() {
		 		return gen == m_generation;
		 	}
	 	 );
		 return false;
	 }

private:
	 std::mutex m_mutex;
	 std::condition_variable m_cond;
	 std::uint32_t m_count;
	 std::uint32_t m_count_start = m_count;
	 std::uint32_t m_generation = 0;
};


/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */
