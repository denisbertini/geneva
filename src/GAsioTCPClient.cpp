/**
 * @file GAsioTCPClient.cpp
 */

/* Copyright (C) 2004-2008 Dr. Ruediger Berlich
 * Copyright (C) 2007-2008 Forschungszentrum Karlsruhe GmbH
 *
 * This file is part of the Geneva library.
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
 */

#include "GAsioTCPClient.hpp"

namespace Gem {
namespace GenEvA {

/***********************************************************************/
/**
 * The standard constructor for this class.
 *
 * @param server Identifies the server
 * @param port Identifies the port on the server
 */
GAsioTCPClient::GAsioTCPClient(std::string server, std::string port) :
	GBaseClient(), maxStalls_(ASIOMAXSTALLS), maxConnectionAttempts_(
			ASIOMAXCONNECTIONATTEMPTS), stalls_(0), command_length_(
			CLIENTCOMMANDLENGTH), io_service_(), socket_(io_service_),
			resolver_(io_service_), query_(server, port), endpoint_iterator0_(
					resolver_.resolve(query_)), end_() { /* nothing */
}

/***********************************************************************/
/**
 * A standard destructor. As we have no local, dynamically allocated
 * data, it is empty.
 */
GAsioTCPClient::~GAsioTCPClient() { /* nothing */
}

/**********************************************************************/
/**
 * Sets the maximum number of stalled connection attempts
 *
 * @param maxStalls The maximum number of stalled connection attempts
 */
void GAsioTCPClient::setMaxStalls(boost::uint32_t maxStalls) throw() {
	maxStalls_ = maxStalls;
}

/**********************************************************************/
/**
 * Retrieves the maximum allowed number of stalled attempts
 *
 * @return The value of the maxStalls_ variable
 */
boost::uint32_t GAsioTCPClient::getMaxStalls() const throw() {
	return maxStalls_;
}

/**********************************************************************/
/**
 * Sets the maximum number of failed connection attempts before termination
 *
 * @param maxConnectionAttempts The maximum number of allowed failed connection attempts
 */
void GAsioTCPClient::setMaxConnectionAttempts(
		boost::uint32_t maxConnectionAttempts) throw() {
	maxConnectionAttempts_ = maxConnectionAttempts;
}

/**********************************************************************/
/**
 * Retrieves the maximum allowed number of failed connection attempts
 *
 * @return The value of the maxConnectionAttempts_ variable
 */
boost::uint32_t GAsioTCPClient::getMaxConnectionAttempts() const throw() {
	return maxConnectionAttempts_;
}

/**********************************************************************/
/**
 * Retrieve work items from the server.
 *
 * @param item Holds the string representation of the work item, if successful
 * @return true if operation should be continued, otherwise false
 */
bool GAsioTCPClient::retrieve(std::string& item) {
	item = "empty"; // Indicates that no item could be retrieved

	try {
		// Try to make a connection, at max maxConnectionAttempts_ times
		boost::uint32_t connectionAttempt = 0;

	    // Restore the start of the iteration
	    boost::asio::ip::tcp::resolver::iterator endpoint_iterator=endpoint_iterator0_;
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (maxConnectionAttempts_ ? (connectionAttempt++ < maxConnectionAttempts_) : true) {
			while (error && endpoint_iterator != end_) {
				// Make sure we try not to re-open an already open socket
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				socket_.close();
				// Make the connection attempt
				socket_.connect(*endpoint_iterator++, error);
			}

			// We were successful
			if (!error)	break;

			// Unsuccessful. Sleep for a second, then try again
			usleep(999999);
		}

		// Still error ? Return, terminate
		if (error) return shutdown(false);

		// Let the server know we want work
		boost::asio::write(socket_, boost::asio::buffer(assembleQueryString("ready", command_length_)));

		// Read answer. First we care for the command sent by the server
		char inboundCommand[command_length_];
		boost::asio::read(socket_, boost::asio::buffer(inboundCommand));

		// Remove all leading or trailing white spaces from the command
		std::string inboundCommandString = boost::algorithm::trim_copy(
				std::string(inboundCommand, command_length_));

		// Act on the command
		if (inboundCommandString == "compute") {
			// We have likely received data. Let's find out how big it is
			char inboundHeader_[command_length_];
			boost::asio::read(socket_, boost::asio::buffer(inboundHeader_));
			std::string inboundHeader = boost::algorithm::trim_copy(
					std::string(inboundHeader_, command_length_));
			std::size_t dataSize = boost::lexical_cast<std::size_t>(
					inboundHeader);

			// Create appropriately sized buffer
			std::vector<char> inboundData(dataSize);

			// Read the real data section from the stream
			boost::asio::read(socket_, boost::asio::buffer(inboundData));

			// And transfer the data into a string
			std::ostringstream oss;
			std::vector<char>::iterator it;
			for (it = inboundData.begin(); it != inboundData.end(); ++it)
				oss << *it;

			// Make the data known to the outside world
			item = oss.str();

			// We have successfully retrieved an item, so we need
			// to reset the stall-counter
			stalls_ = 0;

			// Close sockets, return true
			return shutdown(true);
		}
		else { // Received no work. Try again a number of times
			// We will usually only allow a given number of timeouts / stalls
			if (maxStalls_ && (stalls_++ > maxStalls_)) {
				std::ostringstream error;
				error << "In GAsioTCPClient::retrieve():" << std::endl
						<< "Maximum number of consecutive stalls reached,"
						<< std::endl << "with last command = "
						<< inboundCommand << std::endl
						<< "Cannot cope. Leaving now." << std::endl;

				return shutdown(false);
			}

			// We can continue. But let's wait approximately 1 second first.
			usleep(999999);

			// Indicate that we want to continue
			return shutdown(true);
		}
	}
	// Any system error (except for those where a connection attempt failed) is considered
	// fatal and leads to the termination, by returning false.
	catch (boost::system::system_error&) {
		std::ostringstream error;
		error << "In GAsioTCPClient::retrieve:" << std::endl
				<< "Caught boost::system::system_error exception."
				<< std::endl << "Leaving now." << std::endl;

		LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);

		try {
			return shutdown(false);
		} catch (...) {
			std::ostringstream error;
			error << "In GAsioTCPClient::retrieve:" << std::endl
				  << "Cannot shutdown gracefully as shutdown command" << std::endl
				  << "threw inside of catch statement." << std::endl;

			LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);

			std::terminate();
		}
	}

	// This part of the function should never be reached. Let the audience know, then terminate.
	std::ostringstream error;
	error << "In GAsioTCPClient::retrieve() :" << std::endl
		  << "In a part of the function that should never have been reached!" << std::endl;
	LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);
	std::terminate();

	return false; // Make the compiler happy
}

/**********************************************************************/
/**
 * Submit processed items to the server.
 *
 * @param item String to be submitted to the server
 * @param fitness The current fitness of the individual to be submitted
 * @param isDirty Specifies whether the dirty flag was set on the individual
 * @return true if operation was successful, otherwise false
 */
bool GAsioTCPClient::submit(const std::string& item, const double& fitness,	const bool& isDirty) {
	// Let's assemble an appropriate buffer
	std::vector<boost::asio::const_buffer> buffers;
	std::string result = assembleQueryString("result", command_length_);
	buffers.push_back(boost::asio::buffer(result));

	// Assemble a buffer for the fitness
	std::string fitnessString = assembleQueryString(boost::lexical_cast<std::string>(fitness),command_length_);
	buffers.push_back(boost::asio::buffer(fitnessString));

	// Assemble a buffer for the dirty flag
	std::string dirtyString = assembleQueryString(boost::lexical_cast<std::string>(isDirty),command_length_);
	buffers.push_back(boost::asio::buffer(dirtyString));

	// Assemble the size header
	std::string sizeHeader = assembleQueryString(boost::lexical_cast<std::string> (item.size()), command_length_);
	buffers.push_back(boost::asio::buffer(sizeHeader));

	// Finally take care of the data section.
	buffers.push_back(boost::asio::buffer(item));

	try{
		// Try to make a connection, at max maxConnectionAttempts_ times
		boost::uint32_t connectionAttempt = 0;

	    // Restore the start of the iteration
	    boost::asio::ip::tcp::resolver::iterator endpoint_iterator=endpoint_iterator0_;
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (maxConnectionAttempts_ ? (connectionAttempt++ < maxConnectionAttempts_) : true) {
			while (error && endpoint_iterator != end_) {
				// Make sure we try not to re-open an already open socket
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				socket_.close();
				// Make the connection attempt
				socket_.connect(*endpoint_iterator++, error);
			}

			// We were successful
			if (!error)	break;

			// Unsuccessful. Sleep for a second, then try again
			usleep(999999);
		}

		// Still error ? Return, terminate
		if (error) return shutdown(false);

		// And write the serialized data to the socket. We use
		// "gather-write" to send different buffers in a single
		// write operation.
		boost::asio::write(socket_, buffers);

		// Make sure we don't leave any open sockets lying around.
		return shutdown(true);
	}
	// Any system error (except for those where a connection attempt failed) is considered
	// fatal and leads to the termination, by returning false.
	catch (boost::system::system_error&) {
		std::ostringstream error;
		error << "In GAsioTCPClient::retrieve:" << std::endl
				<< "Caught boost::system::system_error exception."
				<< std::endl << "Leaving now." << std::endl;

		LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);

		try {
			return shutdown(false);
		} catch (...) {
			std::ostringstream error;
			error << "In GAsioTCPClient::retrieve:" << std::endl
				  << "Cannot shutdown gracefully as shutdown command" << std::endl
				  << "threw inside of catch statement." << std::endl;

			LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);

			std::terminate();
		}
	}

	// This part of the function should never be reached. Let the audience know, then terminate.
	std::ostringstream error;
	error << "In GAsioTCPClient::submit() :" << std::endl
		  << "In a part of the function that should never have been reached!" << std::endl;
	LOGGER.log(error.str(), Gem::GLogFramework::CRITICAL);
	std::terminate();

	return false; // Make the compiler happy
}

/**********************************************************************/
/**
 * A small helper function that closes the socket and emits a return code
 *
 * @param returnCode The return code to be emitted
 * @return The return code
 */
bool GAsioTCPClient::shutdown(const bool& returnCode){
	// Make sure we don't leave any open sockets lying around.
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close();

	return returnCode;
}

/**********************************************************************/

} /* namespace GenEvA */
} /* namespace Gem */
