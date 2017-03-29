/**
 * @file GStdThreadGroup.cpp
 */

/*
 * This file is part of the Geneva library collection.
 *
 * Note: this class was adapted from an earlier Boost 1.36 version of the
 * thread_group class. The original code contained the following text:
 *
 * ***
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * (C) Copyright 2007-8 Anthony Williams
 * ***
 *
 * As allowed by the license, modifications were applied to the code.
 * These are also covered by the Boost Software License, Version 1.0, and are
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE.
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
 */

#include "common/GStdThreadGroup.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * The default constructor
 */
GStdThreadGroup::GStdThreadGroup()
{ /* nothing */ }

/******************************************************************************/
/**
 * The standard destructor. Assumes that all threads have already
 * been terminated (?), so that the thread objects can be safely deleted.
 * As we are using a vector of std::shared_ptr<thread> objects, we do not have
 * to manually erase anything.
 */
GStdThreadGroup::~GStdThreadGroup()
{ /* nothing */ }

/******************************************************************************/
/**
 * Adds an already created thread to the group
 *
 * @param thrd A pointer to a thread that should be added to the group
 */
void GStdThreadGroup::add_thread(thread_ptr thrd) {
	if (thrd) {
		std::unique_lock<std::mutex> guard(m_mutex);
		m_threads.push_back(thrd);
	}
}

/******************************************************************************/
/**
 * Requests all threads to join
 */
void GStdThreadGroup::join_all() {
	std::unique_lock<std::mutex> guard(m_mutex);

	for (auto& t: m_threads) {
		t->join();
	}
}

/******************************************************************************/
/**
 * Returns the size of the current thread group.
 * @return The size of the current group
 */
std::size_t GStdThreadGroup::size() const {
	std::unique_lock<std::mutex> guard(m_mutex);
	return m_threads.size();
}

/******************************************************************************/
/**
 * Clears the thread vector. Note that this is a very dangerous operation, which
 * is not made publicly available. This function is meant for consumption by the
 * thread GThreadPool class.
 */
void GStdThreadGroup::clearThreads() {
	m_threads.clear();
}

/******************************************************************************/

} /* namespace Common*/
} /* namespace Gem */
