/**
 * @file GThread.hpp
 */

/*
 * This file is part of the Geneva library collection.
 *
 * Note: this class was adapted from examples provided by Anthony
 * Williams together with his book "C++ Concurrency in Action". At the
 * time of writing, his code was available from
 * https://www.manning.com/books/c-plus-plus-concurrency-in-action
 * under the terms of the Boost software license version 1.0 .
 * Gemfony recommends Anthony's book as a comprehensive pool of
 * knowledge around C++11 threads.
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

// Global checks, defines and includes needed for all of Geneva
#include "common/GGlobalDefines.hpp"

// Standard headers go here
#include <thread>
#include <future>
#include <atomic>
#include <utility>
#include <type_traits>
#include <functional>

// Boost headers go here

#ifndef GTHREAD_HPP_
#define GTHREAD_HPP_

// Geneva headers go here
#include "common/GExceptions.hpp"
#include "common/GLogger.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Indicates whether the thread has been interrupted
 */
class interrupt_flag {
public:
	 G_API_COMMON interrupt_flag();
	 G_API_COMMON void set();
	 G_API_COMMON bool is_set() const;

private:
	 std::atomic<bool> m_interrupted;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * An exception to be thrown if the thread should be interrupted
 */
class thread_interrupted
	: public gemfony_error_condition
{
public:
	 // Use gemfony_error_condition's constructor
	 using gemfony_error_condition::gemfony_error_condition;
	 // But add a default constructor, so we do not need to pass messages
	 thread_interrupted() : gemfony_error_condition("")
	 { /* nothing */ }
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * A wrapper for std::thread that allows to indicate whether the thread
 * has been interrupted.
 */
class thread
{
public:
	 /********************************************************************/
	 /**
	  * The default constructor
	  */
	 G_API_COMMON thread()
	 { /* nothing */ }

	 /********************************************************************/
	 /**
	  * Construction with the function to be executed by the thread
	  */
	 template <typename FunctionType, typename... paramtypes>
	 thread(FunctionType&& f, paramtypes&&... params)
	 {
		 std::promise<interrupt_flag*> p;
		 m_internal_thread=std::move(std::thread(
			 Gem::Common::thread::wrapper<typename std::decay<FunctionType>::type, paramtypes...>
			 , std::ref(p)
			 , std::forward<FunctionType>(f)
			 , std::forward<paramtypes>(params)...
		 ));
		 m_flag=p.get_future().get();
	 }

	 /********************************************************************/
	 /**
	  * Move-constructor
	  */
	 G_API_COMMON thread(thread&& other)
		 : m_internal_thread(std::move(other.m_internal_thread))
		 , m_flag(other.m_flag)
	 {
		 other.m_flag = nullptr;
	 }

	 /********************************************************************/
	 /**
	  * The destructor
	  */
	 G_API_COMMON ~thread() {
		 m_flag = nullptr;
	 }

	 /********************************************************************/
	 /**
	  * Move-assignment
	  */
	 G_API_COMMON thread& operator=(thread&& other) {
		 m_flag = other.m_flag;
		 other.m_flag = nullptr;
		 m_internal_thread = std::move(other.m_internal_thread);
	 }

	 /********************************************************************/
	 /**
	  * Checks whether the internal thread is joinable
	  */
	 G_API_COMMON bool joinable() const {
		 return m_internal_thread.joinable();
	 }

	 /********************************************************************/
	 /**
	  * Retrieval of the thread id
	  */
	 G_API_COMMON std::thread::id get_id() const {
		 return m_internal_thread.get_id();
	 }

	 /********************************************************************/
	 /**
	  * Retrieval of a native_handle to the
	  * underlying thread-implementation
	  */
	 G_API_COMMON std::thread::native_handle_type native_handle() {
		 return m_internal_thread.native_handle();
	 }

	 /********************************************************************/
	 /**
	  * Retrieves information about the number of hardware threads
	  */
	 static unsigned hardware_concurrency() {
		 return std::thread::hardware_concurrency();
	 }

	 /********************************************************************/
	 /**
	  * Waits for the thread to terminate
	  */
	 G_API_COMMON void join() {
		 m_internal_thread.join();
		 m_flag = nullptr;
	 }

	 /********************************************************************/
	 /**
	  * Sends the internal thread to the background
	  */
	 G_API_COMMON void detach() {
		 m_flag = nullptr;
		 m_internal_thread.detach();
	 }

	 /********************************************************************/
	 /**
	  * Signal the thread function that it should terminate itself
	  */
	 G_API_COMMON void interrupt() {
		 if(m_flag) {
			 m_flag->set();
		 }
	 }

	 /********************************************************************/
	 /**
	  * Swaps this objects content with another
	  */
	 G_API_COMMON void swap(thread& other) {
		 this->m_internal_thread.swap(other.m_internal_thread);

		 interrupt_flag *tmp_flag = other.m_flag;
		 other.m_flag = m_flag;
		 m_flag = tmp_flag;
	 }

	 /********************************************************************/
	 /**
	  * Checks whether the thread was interrupted
	  */
	 static bool interrupted() {
		 return m_this_thread_interrupt_flag.is_set();
	 }

	 /********************************************************************/
	 /**
 	  * This function may be called to detect whether an interrupt was triggered
 	  */
	 static void interruption_point() {
		 if(m_this_thread_interrupt_flag.is_set()) {
			 throw thread_interrupted();
		 }
	 }

private:
	 /********************************************************************/
	 /**
	  * A wrapper around function execution with the main goal of
	  * catching interruption requests as well as exceptions
	  */
	 template <typename FunctionType, typename... paramtypes>
	 static void wrapper(
		 std::promise<interrupt_flag*>& p
		 , FunctionType&& f
		 , paramtypes&&... parm
	 ) {
		 p.set_value(&m_this_thread_interrupt_flag);

		 try {
			 f(std::forward<paramtypes>(parm)...);
		 } catch(Gem::Common::thread_interrupted&) { // Nothing, we want to terminate execution
			 return;
		 } catch(Gem::Common::gemfony_error_condition& e) {
			 glogger
				 << "In GThreadWrapper::operator(): Caught Gem::Common::gemfony_error_condition with message" << std::endl
				 << e.what() << std::endl
				 << GTERMINATION;
		 } catch(boost::exception& e){
			 glogger
				 << "In GThreadWrapper::operator(): Caught boost::exception with message" << std::endl
				 << GTERMINATION;
		 } catch(std::exception& e){
			 glogger
				 << "In GThreadWrapper::operator(): Caught std::exception with message" << std::endl
				 << e.what() << std::endl
				 << GTERMINATION;
		 } catch(...){
			 glogger
				 << "GThreadWrapper::operator(): Caught unknown exception" << std::endl
				 << GTERMINATION;
		 }
	 }

	 /********************************************************************/

	 thread(const thread&) = delete; ///< Threads are not copyable
	 thread& operator=(const thread&) = delete; ///< Threads are not copyable

	 std::thread m_internal_thread; ///< The actual thread to be started
	 interrupt_flag* m_flag = nullptr; ///< Holds termination information

	 thread_local static interrupt_flag m_this_thread_interrupt_flag;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

namespace std {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * @brief Swaps the data of two thread objects
 */
void swap(Gem::Common::thread&, Gem::Common::thread&);

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace std */

#endif /* GTHREAD_HPP_ */