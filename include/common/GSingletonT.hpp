/*
 * This file is part of the Geneva library collection.
 *
 * See the NOTICE file in the top-level directory of the Geneva library
 * collection for a list of contributors and copyright information.
 *
 * The following license applies to the code IN THIS FILE:
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
 *
 * NOTE THAT THE BOOST-LICENSE DOES NOT APPLY TO ANY OTHER FILES OF THE
 * GENEVA LIBRARY, UNLESS THIS IS EXPLICITLY STATED IN THE CORRESPONDING FILE!
 */

#pragma once

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <iostream>
#include <memory>
#include <mutex>

// Boost headers go here

#include <boost/utility.hpp>

// Geneva headers go here

namespace Gem {
namespace Common {

/******************************************************************************/
/**
 * This function creates a new T object. It can be specialized by the object
 * used inside of Gemfony's singleton, e.g. in case they do not have a default
 * constructor or wish to perform special actions prior to initialization.
 *
 * @return A std::shared_ptr to a newly created T object
 */
template<typename T>
typename std::shared_ptr<T> TFactory_GSingletonT() {
	return std::shared_ptr<T>(new T());
}

/******************************************************************************/
/**
 * This class implements a singleton pattern, augmented so that it returns
 * a std::shared_ptr. This allows other singletons to store a shared_ptr to
 * T, so that it only gets destroyed once it is no longer needed. Note that
 * the static shared_ptr may long have vanished at that time.
 */
template<typename T>
class GSingletonT
	: boost::noncopyable
{
public:
	/***************************************************************************/
	// Deleted constructors, destructor and assignment operators. This class is
	// not meant to be instantible.
	GSingletonT() = delete;
	GSingletonT(GSingletonT<T> const&) = delete;
	GSingletonT(GSingletonT<T> &&) = delete;
	~GSingletonT() = delete;

	GSingletonT<T> & operator=(GSingletonT<T> const&) = delete;
	GSingletonT<T> & operator=(GSingletonT<T> &&) = delete;

	/***************************************************************************/


	using STYPE = T;

	 /***************************************************************************/
	 /**
	  * If called for the first time, the function creates a std::shared_ptr
	  * of T and returns it to the caller. Subsequent calls to this function
	  * will return the stored copy of the shared_ptr. Other classes can store
	  * the pointer, so that T doesn't get deleted while it is still needed.
	  *
	  * @param mode Determines the mode in which this function is called
	  */
	 static std::shared_ptr<T> Instance(const std::size_t &mode) {
		 static std::shared_ptr<T> p;
		 static std::mutex creation_mutex;

		 switch (mode) {
			 case 0:
				 // Several callers can reach the next line simultaneously. Hence, if
				 // p is empty, we need to ask again if it is empty after we have acquired the lock
				 if (not p) {
					 // Prevent concurrent "first" access
					 std::unique_lock<std::mutex> lk(creation_mutex);
					 if (not p) p = Gem::Common::TFactory_GSingletonT<T>();
				 }

				 return p;
				 break;

			 case 1:
				 p.reset();
				 break;
		 }

		 return std::shared_ptr<T>(); // Make the compiler happy
	 }

	 /***************************************************************************/
};

/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */
