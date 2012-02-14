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

#include "session.h"
#include "server.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>
#include "log.h"
#include "Protocol.pb.h"
#include "sessionmanager.h"

DEFINE_LOGGER(logger, "Session");

static unsigned long id = 60000;
	
Session::Session(int socket)
	: m_socket(socket),
	m_authenticated(false),
	m_id(++id),
	parsed_total(0),
	bytes(0),
	m_username("")
	{
		rotating = 0;
		m_angle = 0;
		m_bpsTimer = 0;
		m_bpsIn = 0;
		m_bpsOut = 0;
}

Session::~Session() {
	if (m_username.empty()) {
		LOG_INFO(logger, this << ": Session destroyed");
	}
	else {
		LOG_INFO(logger, m_username << ": Session destroyed");
	}
}

int Session::getSocket() {
	return m_socket;
}

void Session::tick() {
	m_state = None;
	static char buf[65535];
	int len;
	while ((len = read(m_socket, &buf, 65535)) > 0) {
		std::string d(buf, len);
		handleRead(d, len);
	}

	if (len == 0) {
		stop();
		return;
	}

	m_bpsTimer++;
	if (m_bpsTimer == 10) {
		if (m_bpsIn != 0 || m_bpsOut != 0) {
			if (m_username.empty()) {
				LOG_INFO(logger, this << ": in=" << m_bpsIn << " B/s	out=" << m_bpsOut << " B/s");
			}
			else {
				LOG_INFO(logger, m_username << ": in=" << m_bpsIn << " B/s	out=" << m_bpsOut << " B/s");
			}
		}
		m_bpsTimer = 0;
		m_bpsIn = 0;
		m_bpsOut = 0;
	}
}

void Session::start() {

}

void Session::stop() {
	m_state |= Stopped;
	close(m_socket);
}

void Session::handleRead(const std::string &d, std::size_t b) {
	if (b != 0) {
		bytes = b;
	}

	m_bpsIn += bytes;

	m_data.insert(m_data.end(), d.begin(), d.end());

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
				return;
		}
		else {
			return;
		}

		// Parse wrapper message and erase it from buffer.
		stars::WrapperMessage wrapper;
		if (wrapper.ParseFromArray(&m_data[4], expected_size) == false) {
			LOG_ERROR(logger, "PARSING ERROR");
			m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);
			continue;
		}
		m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);

		// Handle payload in wrapper message
		switch(wrapper.type()) {
			case stars::WrapperMessage_Type_TYPE_LOGIN:
				handleLogin(wrapper.payload());
				break;
			case stars::WrapperMessage_Type_TYPE_REGISTER:
				handleRegister(wrapper.payload());
				break;
			case stars::WrapperMessage_Type_TYPE_MOVE_STATE:
				handleMove(wrapper.payload());
				break;
			case stars::WrapperMessage_Type_TYPE_ENTITY_INFO:
				handleEntityInfo(wrapper.payload());
			default:
				break;
		}
	}
}

void Session::storePosition(stars::MoveState &state) {
	state.set_id(m_id);
	state.set_x(m_x);
	state.set_y(m_y);
	state.set_dst_x(m_destX);
	state.set_dst_y(m_destY);
	state.set_angle(m_angle);
}

void Session::handleEntityInfo(const std::string &payload) {
	unsigned long id = boost::lexical_cast<unsigned long>(payload);
	Session::ref s = SessionManager::instance()->getSession(id);
	if (s) {
		stars::EntityInfo p;
		p.set_id(id);
		p.set_name(s->getUsername());
		send(p, stars::WrapperMessage_Type_TYPE_ENTITY_INFO);
	}
}

void Session::handleRegister(const std::string &data) {
	stars::Login payload;
	if (payload.ParseFromString(data) == false) {
		LOG_ERROR(logger, "handleMove invalid data");
		return;
	}

	unifier::User user;
	user.set_id(m_id);
	user.set_str1(payload.username());
	user.set_str2(payload.password());
	Server::instance()->sendToUnifier("PD", user, unifier::UnifierMessage_Type_TYPE_USER_REGISTER);
}

void Session::handleLogin(const std::string &data) {
	stars::Login payload;
	if (payload.ParseFromString(data) == false) {
		LOG_ERROR(logger, "handleMove invalid data");
		return;
	}

	unifier::User user;
	user.set_id(m_id);
	user.set_str1(payload.username());
	user.set_str2(payload.password());
	Server::instance()->sendToUnifier("PD", user, unifier::UnifierMessage_Type_TYPE_USER_LOGIN);
}

void Session::handleMove(const std::string &data) {
	m_state |= Moved;
	stars::MoveState payload;
	if (payload.ParseFromString(data) == false) {
		LOG_ERROR(logger, "handleMove invalid data");
		return;
	}

	m_x = payload.x();
	m_y = payload.y();
	m_destX = payload.dst_x();
	m_destY = payload.dst_y();

}

#define WRAP(MESSAGE, TYPE) 	stars::WrapperMessage wrap; \
	wrap.set_type(TYPE); \
	wrap.set_payload(MESSAGE); \
	wrap.SerializeToString(&MESSAGE);

void Session::send(::google::protobuf::Message &payload, stars::WrapperMessage_Type type) {
	std::string p;
	payload.SerializeToString(&p);

	std::string message;
	WRAP(p, type);
	wrap.SerializeToString(&message);

	char header[4];
	*((int*)(header)) = htonl(message.size());

	std::string d = std::string(header, 4) + message;
	write(m_socket, d.c_str(), d.size());

	m_bpsOut += d.size();
}

void Session::send(const std::string &payload, stars::WrapperMessage_Type type) {
	std::string message;
	stars::WrapperMessage wrap;
	wrap.set_type(type);
	wrap.set_payload(payload);
	wrap.SerializeToString(&message);

	char header[4];
	*((int*)(header)) = htonl(message.size());

	std::string d = std::string(header, 4) + message;
	write(m_socket, d.c_str(), d.size());

	m_bpsOut += d.size();
}
