/**
 * @file GBoundedBufferT.hpp
 */

/*
 * This file is part of the Geneva library collection.
 *
 * Note: this class was adapted from a circular_buffer test case
 * ([Boost 1.36 trunk version 17.7.08]/libs/circular_buffer/test/bounded_buffer_comparison.cpp )
 * by Jan Gaspar. The original code contained the following text:
 *
 ***************************************************************
 * Copyright (c) 2003-2007 Jan Gaspar
 * Use, modification, and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ***************************************************************
 *
 * As allowed by the license, modifications were applied to the code.
 * These are also covered by the Boost Software License, Version 1.0, and are
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE!
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here

#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <tuple>

// Boost headers go here

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <boost/date_time.hpp>

#ifndef GBOUNDEDBUFFERT_HPP_
#define GBOUNDEDBUFFERT_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GPlotDesigner.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * These typedefs are needed as it is not clear whether we're running on a 64 or 32 bit
 * machine. We do want, however, to be able to count as high as is possible for a given machine
 * for the port id. This is a poor replacement for a guid, which was not yet officially part of
 * Boost, when this class was created.
 */
#ifdef BOOST_HAS_LONG_LONG
typedef std::uint64_t PORTIDTYPE;
#else
typedef std::uint32_t PORTIDTYPE;
#endif

/******************************************************************************/
/** @brief Class to be thrown as a message in the case of a time-out in GBuffer */
class condition_time_out : public std::exception
{ /* nothing */ };

/******************************************************************************/
/**
 * By default the buffer will have this size. As the buffer
 * dynamically grows and shrinks, we choose a very high value. This
 * is a safeguard against errors like endless loops that might keep
 * filling the buffer until memory is exhausted. In normal work
 * conditions, however, the buffer should never reach its upper
 * limit.
 */
const std::size_t DEFAULTBUFFERSIZE = 20000;

/******************************************************************************/
/**
 * This class implements a bounded buffer. Items can be added to one
 * end by multiple threads and retrieved from the other, also by
 * multiple threads. When the buffer is full, attempts to add items
 * will block until there is again enough space. When the buffer is
 * empty, retrieval of items will block until new items have become
 * available. The class contains a "get" function that times out
 * when no item could be retrieved from the buffer. This allows
 * timeouts for data sinks. This can be important in situations
 * where sources might permanently or temporarily go away (e.g. due
 * to network failure). The underlying data structure is a
 * std::deque. The class works with condition variables.  Note that
 * this class assumes that an operator= is available for the items
 * stored in the buffer.
 */
template<typename T>
class GBoundedBufferT
	: private boost::noncopyable
{
public:

	 typedef typename std::deque<T> container_type;
	 // typedef typename std::list<T> container_type; // Tested to much slower than a deque
	 typedef typename container_type::value_type value_type;

	 /***************************************************************************/
	 /**
	  * The default constructor. Sets up a buffer of size DEFAULTBUFFERSIZE.
	  */
	 GBoundedBufferT()
		 : capacity_(DEFAULTBUFFERSIZE)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A constructor that creates a buffer with custom size "capacity".
	  * It enforces a minimum buffer size of 1.
	  *
	  * @param capacity The desired size of the buffer
	  */
	 explicit GBoundedBufferT(const std::size_t &capacity)
		 : capacity_(capacity ? capacity : 1)
	 { /* nothing */ }

	 /***************************************************************************/
	 /**
	  * A standard destructor. Virtual, as classes such as a producer-type
	  * class might get derived from this one. We do not want the destructor
	  * to throw, hence we try to catch all errors locally. Any error here
	  * means termination of the program. No logging takes place, as we want
	  * this class to be independent of the Geneva framework
	  */
	 virtual ~GBoundedBufferT() {
		 // Any error here is deadly ...
		 try {
			 boost::mutex::scoped_lock lock(m_);
			 container_.clear();
		 }
			 // This is a standard error raised by the lock/mutex
		 catch (boost::thread_resource_error &) {
			 glogger
				 << "Caught thread_resource_error in GBoundedBufferT::~GBoundedBufferT(). Terminating ..." << std::endl
				 << GTERMINATION;
		 }
			 // We do not know whether any of the destructors of the items in the buffer throw anything
		 catch (...) {
			 glogger
				 << "Caught unknown exception in GBoundedBufferT::~GBoundedBufferT(). Terminating ..." << std::endl
				 << GTERMINATION;
		 }
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will block if there is no space in the buffer and continue once
	  * space is available.
	  *
	  * @param item An item to be added to the front of the buffer
	  */
	 void push_front(value_type item) {
		 boost::mutex::scoped_lock lock(m_);
		 // Note that this overload of wait() internally runs a loop on its predicate to
		 // deal with spurious wakeups
		 not_full_.wait(lock, buffer_not_full(container_, capacity_));
		 container_.push_front(std::move(item));
		 not_empty_.notify_one();
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will time out after a given amount of time. This function was
	  * added to Jan Gaspar's original implementation.
	  *
	  * @param item An item to be added to the front of the buffer
	  * @param timeout duration until a timeout occurs
	  */
	 void push_front(value_type item, const boost::posix_time::time_duration &timeout) {
		 boost::mutex::scoped_lock lock(m_);
		 if (!not_full_.timed_wait(lock, timeout, buffer_not_full(container_, capacity_))) {
			 throw Gem::Common::condition_time_out();
		 }
		 container_.push_front(std::move(item));
		 not_empty_.notify_one();
	 }

	 /***************************************************************************/
	 /**
	  * Adds a single item to the front of the buffer. The function
	  * will time out after a given amount of time and return false
	  * in this case.
	  *
	  * @param item An item to be added to the front of the buffer
	  * @param timeout duration until a timeout occurs
	  * @return A boolean indicating whether an item has been successfully submitted
	  */
	 bool push_front_bool(value_type item, const boost::posix_time::time_duration &timeout) {
		 boost::mutex::scoped_lock lock(m_);
		 if (!not_full_.timed_wait(lock, timeout, buffer_not_full(container_, capacity_))) {
			 return false;
		 }
		 container_.push_front(std::move(item));
		 not_empty_.notify_one();
		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The
	  * function will block if no items are available and will continue
	  * once items become available again.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  */
	 void pop_back(value_type &item) {
		 boost::mutex::scoped_lock lock(m_);
		 not_empty_.wait(lock, buffer_not_empty(container_));

#ifdef DEBUG
		 if(container_.empty()) {
			 glogger
				 << "In GBoundedBufferT<T>::pop_back(item): Container is empty when it shouldn't be!" << std::endl
				 << GEXCEPTION;
		 }
#endif /* DEBUG */

		 item = std::move(container_.back());
		 container_.pop_back();
		 not_full_.notify_one();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The function
	  * will time out after a given amount of time. This function was
	  * added to Jan Gaspar's original implementation.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  * @param timeout duration until a timeout occurs
	  */
	 void pop_back(value_type &item, const boost::posix_time::time_duration &timeout) {
		 boost::mutex::scoped_lock lock(m_);
		 if (!not_empty_.timed_wait(lock, timeout, buffer_not_empty(container_))) {
			 throw Gem::Common::condition_time_out();
		 }

#ifdef DEBUG
		 if(container_.empty()) {
			 glogger
				 << "In GBoundedBufferT<T>::pop_back(item,timeout): Container is empty when it shouldn't be!" << std::endl
				 << GEXCEPTION;
		 }
#endif /* DEBUG */

		 item = std::move(container_.back());
		 container_.pop_back();
		 not_full_.notify_one();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves a single item from the end of the buffer. The function
	  * will time out after a given amount of time. It will return false
	  * in this case. "true" will be returned if an item could be retrieved
	  * successfully.
	  *
	  * @param item Reference to a single item that was removed from the end of the buffer
	  * @param timeout duration until a timeout occurs
	  * @return A boolean indicating whether an item has been successfully retrieved
	  */
	 bool pop_back_bool(value_type &item, const boost::posix_time::time_duration &timeout) {
		 boost::mutex::scoped_lock lock(m_);
		 if (!not_empty_.timed_wait(lock, timeout, buffer_not_empty(container_))) {
			 return false;
		 }

#ifdef DEBUG
		 if(container_.empty()) {
			 glogger
				 << "In GBoundedBufferT<T>::pop_back_bool(item,timeout): Container is empty when it shouldn't be!" << std::endl
				 << GEXCEPTION;
		 }
#endif /* DEBUG */

		 item = std::move(container_.back()); // Assign the item at the back of the container
		 container_.pop_back(); // Remove it from the container
		 not_full_.notify_one();
		 return true;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the maximum allowed size of the buffer. No need for
	  * synchronization, as reading the value should be an atomic
	  * operation.
	  *
	  * @return The maximum allowed capacity
	  */
	 std::size_t getCapacity() const {
		 return capacity_;
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the remaining space in the buffer. Note that the capacity
	  * may change once this function has completed. The information taken
	  * from this function can thus only serve as an indication.
	  *
	  * @return The currently remaining space in the buffer
	  */
	 std::size_t remainingSpace() {
		 boost::mutex::scoped_lock lock(m_);
		 return capacity_ - container_.size();
	 }

	 /***************************************************************************/
	 /**
	  * Retrieves the current size of the buffer. Note that the buffer
	  * (and its size) may change immediately after this function has
	  * completed, this value should therefore only be taken as an
	  * indication.
	  *
	  * @return The current size of the buffer
	  */
	 std::size_t size() {
		 boost::mutex::scoped_lock lock(m_);
		 return container_.size();
	 }

	 /***************************************************************************/
	 /**
	  * Returns whether the buffer is empty or not. Note that the buffer
	  * contents may change immediately after this function has
	  * completed, this value should therefore only be taken as an
	  * indication.
	  *
	  * @return True if the buffer is not empty
	  */
	 bool isNotEmpty() {
		 boost::mutex::scoped_lock lock(m_);
		 return !container_.empty();
	 }

	 /***************************************************************************/
	 /*
	  * Retrieves the id_ variable.
	  *
	  * @return The value of the id_ variable
	  */
	 PORTIDTYPE getId() const {
		 return id_;
	 }

	 /***************************************************************************/
	 /*
	  * Allows to set the id_ once. Any subsequent calls to this
	  * function will have no effect.
	  *
	  * @param id The desired value of the id_ variable
	  */
	 void setId(const PORTIDTYPE &id) {
		 if (!idSet_) {
			 id_ = id;
			 idSet_ = true;
		 }
	 }

protected:
	 /***************************************************************************/
	 /*
	  * We want to be able to add custom producer threads. Hence the
	  * following code is protected, not private.
	  */

	 /**
	  * A function object that checks whether a given container is empty or not.
	  * Note that this code is only called in a safe context, hence no protection
	  * is necessary.
	  */
	 struct buffer_not_empty {
	 public:
		  /* @brief Initializes the local container reference */
		  buffer_not_empty(
			  const container_type &c
		  )
			  : c_(c) { /* nothing */ }

		  /** @brief Used for the actual test */
		  bool operator()() const {
			  return (!c_.empty());
		  }

	 private:
		  /** @brief Default constructor; intentionally private and undefined */
		  buffer_not_empty() = delete;

		  const container_type &c_; ///< Holds a reference to the actual container
	 };

	 /**
	  * A function object that checks whether a given container is full or not.
	  * Note that this code is only called in a safe context, hence no protection
	  * is necessary.
	  */
	 struct buffer_not_full {
	 public:
		  /* @brief Initializes the local container reference and the maximum capacity */
		  buffer_not_full(
			  const container_type &c, const std::size_t &capacity
		  )
			  : c_(c), capacity_(capacity) { /* nothing */ }

		  /** @brief Used for the actual test */
		  bool operator()() const {
			  return (c_.size() < capacity_);
		  }

	 private:
		  /** @brief Default constructor; intentionally private and undefined */
		  buffer_not_full() = delete;

		  const container_type &c_;
		  const std::size_t &capacity_;
	 };

	 /***************************************************************************/

	 const std::size_t capacity_; ///< The maximum allowed size of the container
	 container_type container_; ///< The actual data store
	 mutable boost::mutex m_; ///< Used for synchronization of access to the container
	 boost::condition_variable not_empty_; ///< Used for synchronization of access to the container
	 boost::condition_variable not_full_; ///< Used for synchronization of access to the container

private:
	 /***************************************************************************/

	 GBoundedBufferT(const GBoundedBufferT<T> &) = delete; ///< Disabled copy constructor
	 GBoundedBufferT &operator=(const GBoundedBufferT<T> &) = delete; ///< Disabled assign operator

	 volatile PORTIDTYPE id_=0; ///< An id that allows to identify this class
	 volatile bool idSet_=false; ///< Allows control over whether the id has been set before
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

#endif /* GBOUNDEDBUFFERT_HPP_ */
