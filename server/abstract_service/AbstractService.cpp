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

#include "AbstractService.h"
#include <boost/bind.hpp>
#include <iostream>
#include "log.h"
#include <unistd.h>
#include "string.h"
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include "poll.h"

DEFINE_LOGGER(logger, "AbstractService");

AbstractService::AbstractService() {
	m_sock = 0;
}

AbstractService::~AbstractService() {
	close(m_sock);
}

void AbstractService::sendToUnifier(unifier::UnifierMessage &message) {
	std::string msg;
	message.SerializeToString(&msg);

	char header[4];
	*((int*)(header)) = htonl(msg.size());

	std::string d = std::string(header, 4) + msg;
	write(m_sock, d.c_str(), d.size());
}

void AbstractService::sendToUnifier(const std::string &to, ::google::protobuf::Message &msg, unifier::UnifierMessage_Type type) {
	unifier::UnifierMessage w;
	w.set_to(to);
	w.set_type(type);

	std::string payload;
	msg.SerializeToString(&payload);
	w.set_payload(payload);

	sendToUnifier(w);
}

void AbstractService::sendTock() {
	unifier::UnifierMessage m;
	m.set_type(unifier::UnifierMessage_Type_TYPE_TOCK);
	m.set_to("TS");
	sendToUnifier(m);
}

void AbstractService::registerService(const std::string &name) {
	unifier::UnifierMessage u;
	u.set_type(unifier::UnifierMessage_Type_TYPE_REGISTER);
	u.set_to("");
	u.set_payload(name);

	sendToUnifier(u);
}

void AbstractService::connectToUnifier(const std::string &host, int port) {
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;
	struct hostent *hp;

	bzero((char *) &server, sizeof(server));
	server.sin_family = AF_INET;
	while ((hp = gethostbyname(host.c_str())) == NULL) {
		LOG_ERROR(logger, "Can't connect the Unifier: " << strerror(errno));
		usleep(1000000);
	}

	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons((u_short) port);

	while(connect(m_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		LOG_ERROR(logger, "Can't connect the Unifier: " << strerror(errno));
		usleep(1000000);
	}
}

int AbstractService::handleRead(const std::string &d, std::size_t bytes) {
	m_data.insert(m_data.end(), d.begin(), d.end());

	int tick = 0;
	// Parse data while there are some
	while (m_data.size() != 0) {
		// expected_size of wrapper message
		unsigned int expected_size;

		// if data is >= 4, we have whole header and we can
		// read expected_size.
		if (m_data.size() >= 4) {
			expected_size = *((unsigned int*) &m_data[0]);
			expected_size = ntohl(expected_size);
			// If we don't have whole wrapper message, wait for next
			// handleDataRead call.
			if (m_data.size() - 4 < expected_size)
				return tick;
		}
		else {
			return tick;
		}

		// Parse wrapper message and erase it from m_data.
		unifier::UnifierMessage message;
		if (message.ParseFromArray(&m_data[4], expected_size) == false) {
			std::cout << "PARSING ERROR " << expected_size << "\n";
			m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);
			continue;
		}
		m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);

		// Handle payload in wrapper message
		switch(message.type()) {
			case unifier::UnifierMessage_Type_TYPE_TICK:
				tick = 1;
				break;
			case unifier::UnifierMessage_Type_TYPE_TOCK:
				tick = -1;
				break;
			default:
				handleUnifierMessage(message);
				break;
		}
	}
	return tick;
}

void AbstractService::waitForTick() {
	static char buf[65535];
	struct pollfd fds[1];
	fds[0].fd = m_sock;
	fds[0].events = POLLIN;

	while (1) {
		int ret = poll(fds, 1, -1);
		if (ret == 1) {
			int len = read(m_sock, &buf, 65535);
			if (len > 0) {
				std::string d(buf, len);
				if (handleRead(d, len) == 1) {
					return;
				}
			}
		}
	}
}

void AbstractService::waitForTock() {
	static char buf[65535];
	struct pollfd fds[1];
	fds[0].fd = m_sock;
	fds[0].events = POLLIN;

	while (1) {
		int ret = poll(fds, 1, -1);
		if (ret == 1) {
			int len = read(m_sock, &buf, 65535);
			if (len > 0) {
				std::string d(buf, len);
				if (handleRead(d, len) == -1) {
					return;
				}
			}
		}
	}
}

void AbstractService::loop() {
	static char buf[65535];
	struct pollfd fds[1];
	fds[0].fd = m_sock;
	fds[0].events = POLLIN;

	while (1) {
		int ret = poll(fds, 1, -1);
		if (ret == 1) {
			int len = read(m_sock, &buf, 65535);
			if (len > 0) {
				std::string d(buf, len);
				handleRead(d, len);
			}
		}
	}
}
