/**
 * Hamlog
 *
 * Copyright (C) 2011, Jan Kaluza <hanzz.k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "sessionmanager.h"
#include <boost/bind.hpp>
#include <iostream>
#include <algorithm>
#include <boost/foreach.hpp>
#include "UnifierProtocol.pb.h"

#include "log.h"

DEFINE_LOGGER(logger, "ServiceManager");

SessionManager::SessionManager() {
	
}

void SessionManager::tick() {
	std::vector<int> to_remove;

	static char buf[65535];
	for (std::map<int, std::string>::iterator it = m_fds.begin(); it != m_fds.end(); it++) {
		int fd = it->first;
		int len;
		while ((len = read(fd, &buf, 65535)) > 0) {
			std::string d(buf, len);
			handleRead(it->first, it->second, d, len);
		}

		if (len == 0) {
			to_remove.push_back(fd);
			return;
		}
	}

	BOOST_FOREACH(int fd, to_remove) {
		m_fds.erase(fd);
	}

}

void SessionManager::start(int fd) {
	m_fds[fd] = std::string();
}

void SessionManager::send(int fd, unifier::UnifierMessage &message) {
	std::string msg;
	message.SerializeToString(&msg);

	char header[4];
	*((int*)(header)) = htonl(msg.size());

	std::string d = std::string(header, 4) + msg;
	write(fd, d.c_str(), d.size());
}

void SessionManager::routeMessage(unifier::UnifierMessage &message) {
	if (message.to().empty()) {
		for (std::map<std::string, int>::iterator it = m_services.begin(); it != m_services.end(); it++) {
			send(it->second, message);
		}
	}
	else {
		std::map<std::string, int>::iterator it = m_services.find(message.to());
		if (it != m_services.end()) {
			send(it->second, message);
		}
		else {
			LOG_ERROR(logger, "Unknown recipient " << message.to());
		}
	}
}

void SessionManager::handleRegister(int fd, unifier::UnifierMessage &message) {
	LOG_INFO(logger, "Service " << message.payload() << " registered");
	m_services[message.payload()] = fd;
}

void SessionManager::handleRead(int fd, std::string &buffer, const std::string &d, std::size_t bytes) {
	buffer.insert(buffer.end(), d.begin(), d.end());

	// Parse data while there are some
	while (buffer.size() != 0) {
		// expected_size of wrapper message
		unsigned int expected_size;

		// if data is >= 4, we have whole header and we can
		// read expected_size.
		if (buffer.size() >= 4) {
			expected_size = *((unsigned int*) &buffer[0]);
			expected_size = ntohl(expected_size);
			// If we don't have whole wrapper message, wait for next
			// handleDataRead call.
			if (buffer.size() - 4 < expected_size)
				return;
		}
		else {
			return;
		}

		// Parse wrapper message and erase it from buffer.
		unifier::UnifierMessage message;
		if (message.ParseFromArray(&buffer[4], expected_size) == false) {
			std::cout << "PARSING ERROR " << expected_size << "\n";
			buffer.erase(buffer.begin(), buffer.begin() + 4 + expected_size);
			continue;
		}
		buffer.erase(buffer.begin(), buffer.begin() + 4 + expected_size);

		// Handle payload in wrapper message
		switch(message.type()) {
			case unifier::UnifierMessage_Type_TYPE_REGISTER:
				handleRegister(fd, message);
				break;
			default:
				routeMessage(message);
				break;
		}
	}
}


