/**
 * @file GThreadGroup.cpp
 */

/* This file is part of Geneva, Gemfony scientific's optimization library.
 *
 * Contact: info [at] gemfony (dot) com
 *
 * Note: This class was adapted from an early Boost 1.36 version of the
 * thread_group class. The original code containted the following text:
 *
 ****
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * (C) Copyright 2007-8 Anthony Williams
 ****
 *
 * Modifications were applied to the code.
 *
 * All modifications to Anthony William's original code, as shown in this
 * file, are also covered by the Boost Software License, Version 1.0,
 * and are Copyright (C) Dr. Ruediger Berlich and Karlsruhe Institute of
 * Technology.
 *
 * In particular, please note that
 *
 ***************************************************************
 * RUEDIGER BERLICH AND FORSCHUNGSZENTRUM KARLSRUHE MAKE NO
 * REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR
 * ANY PURPOSE. IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED
 * WARRANTY.
 ***************************************************************
 */

/* The code in this file is covered by the following license:
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

#include "GThreadGroup.hpp"

namespace Gem {
namespace Util {

/********************************************************************/
/**
 * The standard destructor. Assumes that all threads have already
 * been terminated (?), so that the thread objects can be safely deleted.
 * As we are using a vector of boost::shared_ptr<thread> objects, we do not have
 * to manually erase anything.
 */
GThreadGroup::~GThreadGroup()
{ /* nothing */ }

/********************************************************************/
/**
 * Adds an already created thread to the group
 *
 * @param thrd A pointer to a thread that should be added to the group
 */
void GThreadGroup::add_thread(boost::thread* thrd) {
	if (thrd) {
		boost::lock_guard<boost::mutex> guard(m_);
		thread_ptr p_thrd(thrd);
		threads_.push_back(p_thrd);
	}
}

/********************************************************************/
/**
 * Remove a thread from the group. Does nothing if the thread is empty.
 *
 * @param thrd A pointer to the thread that shall be removed from the group
 */
void GThreadGroup::remove_thread(boost::thread* thrd) {
	if (!thrd) return;

	boost::lock_guard<boost::mutex> guard(m_);
	thread_vector::iterator const cit =
		std::find(threads_.begin(), threads_.end(), thread_ptr(thrd));

	if (cit != threads_.end()) threads_.erase(cit);
}

/********************************************************************/
/**
 * Requests all threads to join
 */
void GThreadGroup::join_all() {
	boost::lock_guard<boost::mutex> guard(m_);

	for (thread_vector::iterator it = threads_.begin(); it!= threads_.end(); ++it){
		(*it)->join();
	}
}

/********************************************************************/
/**
 * Sends all threads the interrupt signal
 */
void GThreadGroup::interrupt_all() {
	boost::lock_guard<boost::mutex> guard(m_);

	for (thread_vector::iterator it = threads_.begin(); it != threads_.end(); ++it)
		(*it)->interrupt();
}

/********************************************************************/
/**
 * Interrupts, joins and finally removes the last thread in the group.
 * Does nothing if the group is already empty.
 */
void GThreadGroup::remove_last() {
	boost::lock_guard<boost::mutex> guard(m_);

	if (threads_.size() == 0) return;

	(threads_.back())->interrupt();
	(threads_.back())->join();
	// boost::shared_ptr takes care of the deletion of the thread object
	threads_.pop_back();
}

/********************************************************************/
/**
 * Interrupts, joins and finally removes the last nThreads threads
 * in the group. Stops if the number of threads would drop below 0.
 *
 * @param nThreads The number of threads at the end of the group that shall be removed
 */
void GThreadGroup::remove_last(const std::size_t& nThreads) {
	for (std::size_t i = 0; i < nThreads; i++) remove_last();
}

/********************************************************************/
/**
 * Returns the size of the current thread group.
 * @return The size of the current group
 */
std::size_t GThreadGroup::size() const {
	boost::lock_guard<boost::mutex> guard(m_);
	return threads_.size();
}

} /* namespace Util*/
} /* namespace Gem */
