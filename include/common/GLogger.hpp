/**
 * @file GLogger.hpp
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

// Standard header files go here

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <vector>

// Includes check for correct Boost version(s)
#include "common/GGlobalDefines.hpp"

// Boost header files go here
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#ifndef GEXCEPTIONSTREAMER_HPP_
#define GEXCEPTIONSTREAMER_HPP_

// For Microsoft-compatible compilers
#if defined(_MSC_VER)  &&  (_MSC_VER >= 1020)
#pragma once
#endif


// Geneva header files go here
#include "common/GSingletonT.hpp"
#include "common/GCommonEnums.hpp"
#include "common/GExceptions.hpp"

namespace Gem {
namespace Common {

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class defines the interface of log targets, i.e. targets for the logging of
 * messages through the GLogStreamer class. Essentially all that is needed is
 * the log function. Pointers to this class are stored in the GLogStreamer. They
 * point to objects of the GConsoleLogger or GFileLogger classes, or other log targets
 * defined by the user.
 */
class GBaseLogTarget {
public:
   /** @brief The default constructor */
    GBaseLogTarget(void);
    /** @brief The standard destructor */
    virtual ~GBaseLogTarget();

    /** @brief The logging interface */
    virtual void log(const std::string&) const = 0;
    /** @brief Adds an extension to the output */
    virtual void logWithSource(const std::string&, const std::string&) const = 0;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The console logger writes log messages to the console.
 */
class GConsoleLogger :public GBaseLogTarget
{
public:
   /** @brief A standard constructor */
   GConsoleLogger(void);
   /** @brief The standard destructor */
   virtual ~GConsoleLogger();

   /** @brief Implements the logging to the console */
   virtual void log(const std::string&) const;
   /** @brief Adds a specifier to the output */
   virtual void logWithSource(
         const std::string&
         , const std::string&
   ) const;
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * The file logger writes log messages to a file.
 */
class GFileLogger :public GBaseLogTarget
{
public:
/** @brief A standard constructor */
    GFileLogger(void);
    /** @brief This constructor accepts a boost path to a file name as argument */
    explicit GFileLogger(const boost::filesystem::path&);
    /** @brief The standard destructor */
    virtual ~GFileLogger();

    /** @brief Implements logging to a file on disk */
    virtual void log(const std::string& msg) const;
    /** @brief Adds an extension to the output file */
    virtual void logWithSource(
          const std::string&
          , const std::string&
    ) const;

private:
    std::string fname_; ///< The name of the log file
    mutable bool first_; ///< Indicates whether any logging has already been done
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * This class serves as the front end of the logging infrastructure. An object of
 * this type is accessible through a singleton to all entities in the program.
 * Upon invocation of the streaming operator it produces an object which is supposed
 * to handle the rest of the work, either using the log targets stored in the
 * GLogger object or letting manipulators output the work.
 */
template <class S> // "S" means "streamer"
class GLogger
   :boost::noncopyable
{
public:
  /***************************************************************************/
  /** @brief The default constructor - needed for the singleton */
  GLogger(void)
  { /* nothing */ }

  /***************************************************************************/
  /**
   * The destructor
   */
  virtual ~GLogger()
  { /* nothing */ }

  /***************************************************************************/
  /**
   * This function will forward all arguments to a newly created object
   * of type S. Note that the function returns the S object by value. It
   * will not the survive beyond the end of the stream-chain.
   */
  template <typename T>
  S operator<<(const T& t) {
    S s;
    s << t;
    return s;
  }

  /***************************************************************************/
  /**
   * This function instructs the logger architecture to emit additional
   * specifications for the data being logged. When writing to the console,
   * a corresponding text will be emitted. When writing to a file, the
   * modifier will be appended with an underscore to the filename.
   */
  S operator()(const std::string& extension) {
     S s(extension);
     return s;
  }

  /***************************************************************************/
  /**
   * This function instructs the logger architecture to emit data to the file
   * specified by the boost::path object
   */
  S operator()(boost::filesystem::path p) {
     S s(p);
     return s;
  }

  /***************************************************************************/
  /**
   * Adds a log target, such as console or file
   */
  void addLogTarget(boost::shared_ptr<GBaseLogTarget> gblt) {
     if(gblt) {
        logVector_.push_back(gblt);
     }
  }

  /***************************************************************************/
  /**
   * Checks whether any log targets are present
   */
  bool hasLogTargets(void) const {
     return !logVector_.empty();
  }

  /***************************************************************************/
  /**
   * Clears local log-targets
   */
  void resetLogTargets() {
     logVector_.clear();
  }

  /***************************************************************************/
  /**
   * Allows S-objects to submit strings to the log targets. Note that this
   * function is thread-safe and thus may be called from different threads.
   * Note that this function throws if no logging targets have been registered.
   */
  void log(const std::string& message) const {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

#ifdef DEBUG
     if(logVector_.empty()) {
        raiseException(
              "In GLogger<S>::log(): Error!" << std::endl
              << "No logging targets have been registered." << std::endl
        );
     }
#endif /* DEBUG */

     // Do the actual logging
     std::vector<boost::shared_ptr<GBaseLogTarget> >::const_iterator cit;
     for(cit=logVector_.begin(); cit!=logVector_.end(); ++cit) {
        (*cit)->log(message);
     }
  }

  /***************************************************************************/
  /**
   * Allows S-objects to submit strings to the log targets. Note that this
   * function is thread-safe and thus may be called from different threads.
   * Note that this function throws if no logging targets have been registered.
   */
  void logWithSource(const std::string& message, const std::string& extension) const {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

#ifdef DEBUG
     if(logVector_.empty()) {
        raiseException(
              "In GLogger<S>::logWithSource(): Error!" << std::endl
              << "No logging targets have been registered." << std::endl
        );
     }
#endif /* DEBUG */

     // Do the actual logging
     std::vector<boost::shared_ptr<GBaseLogTarget> >::const_iterator cit;
     for(cit=logVector_.begin(); cit!=logVector_.end(); ++cit) {
        (*cit)->logWithSource(message, extension);
     }
  }

  /***************************************************************************/
  /**
   * Throws an exception from a global position. This prevents exceptions thrown
   * from within threads from getting lost.
   */
  void throwException(const std::string& error) {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

     throw(Gem::Common::gemfony_error_condition(error));
  }

  /***************************************************************************/
  /**
   * Initiates the termination sequence
   */
  void terminateApplication(const std::string& error) {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

     std::cerr << error;
     std::terminate();
  }

  /***************************************************************************/
  /**
   * Output to stdout
   */
  void toStdOut(const std::string& message) {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

     std::cout << message;
  }

  /***************************************************************************/
  /**
   * Output to stderr
   */
  void toStdErr(const std::string& message) {
     // Make sure only one entity outputs data
     boost::mutex::scoped_lock lk(logger_mutex_);

     std::cerr << message;
  }

private:
  /***************************************************************************/

  std::vector<boost::shared_ptr<GBaseLogTarget> > logVector_; ///< Contains the log targets
  mutable boost::mutex logger_mutex_; ///< Needed for concurrent access to the log targets
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
 * Objects of this class need to be added as the last element of a logging or
 * exception chain, possibly wrapped into a macro giving it information about
 * the file and lines from which it has been called.
 */
class GManipulator
   : boost::noncopyable
{
public:
   /** @brief A constructor that stores both accompanying information and the logging type */
   GManipulator(
         const std::string& accompInfo
         , const logType& lt
   );
   /** @brief A constructor that stores the logging type only */
   explicit GManipulator(const logType& lt);

   /** @brief Retrieves the stored logging type */
   logType getLogType() const;
   /** @brief Retrieves stored accompanying information (if any) */
   std::string getAccompInfo() const;
   /** @brief Checks whether any accompanying information is available */
   bool hasAccompInfo() const;

private:
   GManipulator(); ///< Intentionally private and undefined

   std::string accompInfo_; ///< Holds accompanying information
   logType logType_; ///< Holds the type of logging event used for instantiating the manipulator
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/**
* Every entity in Geneva should be able to throw exceptions, regardless of whether
* this happens from within a thread a in the context of serial execution. The output
* should go to different log targets defined by the user, such as stdout or a file
* (or possibly both). Emitting as much information as possible should be encouraged.
* Hence adding information the the exception handler should be as easy as adding
* data to a stream.
*/
class GLogStreamer
{
public:
  /** @brief The default constructor */
  GLogStreamer(void);
  /** @brief The copy constructor */
  GLogStreamer(const GLogStreamer&);
  /** @brief A constructor that adds an extension string to the output */
  explicit GLogStreamer(const std::string&);
  /** @brief A constructor that logs data to a file specified by a boost::filesystem::path object */
  explicit GLogStreamer(boost::filesystem::path);
  /** @brief A standard destructor */
  virtual ~GLogStreamer();

  /** @brief Needed for std::ostringstream */
  GLogStreamer& operator<< (std::ostream& ( *val )(std::ostream&));
  /** @brief Needed for std::ostringstream */
  GLogStreamer& operator<< (std::ios& ( *val )(std::ios&));
  /** @brief Needed for std::ostringstream */
  GLogStreamer& operator<< (std::ios_base& ( *val )(std::ios_base&));

  /** @brief A GManipulator object triggers the actual logging procedure */
  void operator<<(const GManipulator& gm);

  /** @brief Returns the content of the stream */
  std::string content() const;
  /** @brief Resets the stream content */
  void reset();

  /** @brief Checks whether an extension string has been registered */
  bool hasExtension() const;
  /** @brief The content of the extension_ string */
  std::string getExtension() const;
  /** @brief Checks whether a log file name has been registered */
  bool hasOneTimeLogFile() const;
  /** @brief The name of the manually specified file */
  boost::filesystem::path getOneTimeLogFile() const;

  /****************************************************************************/
  /**
   * Output of all standard values and types with a predefined operator<< .
   */
  template <typename T>
  GLogStreamer& operator<<(const T& val) {
    oss_ << val;
    return *this;
  }

  /****************************************************************************/

private:
  std::ostringstream oss_; ///< Holds the actual streamed data
  std::string extension_; ///< Additional information about the logging source
  boost::filesystem::path logFile_; ///< The name of a manually specified log file
};

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

} /* namespace Common */
} /* namespace Gem */

/******************************************************************************/
/**
* We currently require the global GLogStreamer object to be a singleton
*/
typedef Gem::Common::GSingletonT<Gem::Common::GLogger<Gem::Common::GLogStreamer> > log_singleton;
#define glogger_ptr log_singleton::Instance(0)
#define glogger (*(log_singleton::Instance(0)))

/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/
// Some related defines

#define GEXCEPTION   Gem::Common::GManipulator(std::string("in file ") + boost::lexical_cast<std::string>(__FILE__) + std::string(" near line ") + boost::lexical_cast<std::string>(__LINE__), Gem::Common::EXCEPTION)
#define GTERMINATION Gem::Common::GManipulator(std::string("in file ") + boost::lexical_cast<std::string>(__FILE__) + std::string(" near line ") + boost::lexical_cast<std::string>(__LINE__), Gem::Common::TERMINATION)
#define GWARNING     Gem::Common::GManipulator(std::string("in file ") + boost::lexical_cast<std::string>(__FILE__) + std::string(" near line ") + boost::lexical_cast<std::string>(__LINE__), Gem::Common::WARNING)
#define GLOGGING     Gem::Common::GManipulator(Gem::Common::LOGGING)
#define GFILE        Gem::Common::GManipulator(Gem::Common::FILE)
#define GSTDOUT      Gem::Common::GManipulator(Gem::Common::STDOUT)
#define GSTDERR      Gem::Common::GManipulator(std::string("in file ") + boost::lexical_cast<std::string>(__FILE__) + std::string(" near line ") + boost::lexical_cast<std::string>(__LINE__), Gem::Common::STDERR)

/******************************************************************************/

#endif /* GEXCEPTIONSTREAMER_HPP_ */