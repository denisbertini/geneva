/**
 * @file GAsioHelperFunctions.cpp
 */

/*
 * Copyright (C) Gemfony scientific UG (haftungsbeschraenkt)
 *
 * See the AUTHORS file in the top-level directory for a list of authors.
 *
 * Contact: contact [at] gemfony (dot) eu
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
 * http://www.gemfony.eu .
 */

#include "courtier/GCourtierHelperFunctions.hpp"

namespace Gem {
namespace Courtier {

/******************************************************************************/
/**
 * Assembles a query string from a given command, emitting a string of a given size.
 * This function is used in conjunction with Boost::Asio .
 *
 * @param str The string from which the size should be extracted
 * @param sz Resulting size of the query string
 * @return The query string
 */
std::string assembleQueryString(const std::string &query, const std::size_t &sz) {
	std::ostringstream query_stream;
	query_stream << std::setw(boost::numeric_cast<int>(sz)) << query;
	return query_stream.str();
}

/******************************************************************************/
/**
 * Extracts the size of ASIO's data section from a C string.
 * Used in conjunction with Boost::Asio. See e.g. GAsioTCPClient.
 *
 * @param ds The data string holding the data size
 * @param sz The size of the data string
 * @return The size of the data
 */
std::size_t extractDataSize(const char *ds, const std::size_t &sz) {
	std::istringstream is(std::string(ds, sz));
	std::size_t inboundDataSize = 0;
	if (!(is >> std::hex >> inboundDataSize)) {
		glogger
		<< "In extractDataSize: Got invalid header!" << std::endl
		<< GEXCEPTION;
	}

	return inboundDataSize;
}

/******************************************************************************/
/**
 * Cleanly shuts down a socket
 *
 * @param socket The socket on which the shutdown should be performed
 */
void disconnect(boost::asio::ip::tcp::socket &socket) {
	boost::system::error_code ignore;
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignore);
	socket.close();
}

/******************************************************************************/
/**
 * Create a boolean mask
 */
std::vector<bool> getBooleanMask(
	std::size_t vecSize
	, std::size_t start
	, std::size_t end
) {
	std::vector<bool> workItemPos(vecSize, Gem::Courtier::GBC_PROCESSED);
	for(auto p_it = workItemPos.begin() + start; p_it != workItemPos.begin() + end; ++p_it) {
		*p_it = Gem::Courtier::GBC_UNPROCESSED;
	}
	return workItemPos;
}

/******************************************************************************/

} /* namespace Courtier */
} /* namespace Gem */