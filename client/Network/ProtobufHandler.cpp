/**
 * Stars client
 *
 * Copyright (C) 2012, Jan Kaluza <hanzz.k@gmail.com>
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

#include "ProtobufHandler.h"
#include "Connection.h"
#include <string>
#include <sstream>
#include <iostream>
#include <arpa/inet.h>
#include "Objects/VisibleObjectManager.h"
#include "Objects/Ship.h"
#include "Widgets/WindowManager.h"
#include "Windows/LoginWindow.h"
#include "Base/MapArea.h"

ProtobufHandler::ProtobufHandler() {
}

ProtobufHandler::~ProtobufHandler() {
}

void ProtobufHandler::setConnection(Connection *conn) {
	m_conn = conn;
}

void ProtobufHandler::send(const std::string &message) {
	char header[4];
	*((int*)(header)) = htonl(message.size());

	std::string d = std::string(header, 4) + message;
	m_conn->sendData(d.c_str(), d.size());
}

void ProtobufHandler::sendPong() {
	std::string message;
	stars::WrapperMessage wrap;
	wrap.set_type(stars::WrapperMessage_Type_TYPE_PONG);
	wrap.SerializeToString(&message);

	send(message);
	std::cout << "> PONG\n";
}

#define WRAP(MESSAGE, TYPE) 	stars::WrapperMessage wrap; \
	wrap.set_type(TYPE); \
	wrap.set_payload(MESSAGE); \
	wrap.SerializeToString(&MESSAGE);

void ProtobufHandler::send(Connection *conn, ::google::protobuf::Message &payload, stars::WrapperMessage_Type type) {
	std::string p;
	payload.SerializeToString(&p);

	std::string message;
	WRAP(p, type);
	wrap.SerializeToString(&message);

	char header[4];
	*((int*)(header)) = htonl(message.size());

	std::string d = std::string(header, 4) + message;
	conn->sendData(d.c_str(), d.size());
}

void ProtobufHandler::send(Connection *conn, const std::string &data, stars::WrapperMessage_Type type) {
	stars::WrapperMessage wrap;
	wrap.set_type(type);
	wrap.set_payload(data);
	std::string message;
	wrap.SerializeToString(&message);

	char header[4];
	*((int*)(header)) = htonl(message.size());

	std::string d = std::string(header, 4) + message;
	conn->sendData(d.c_str(), d.size());
}

void ProtobufHandler::handleEntityInfo(const std::string &data) {
	stars::EntityInfo payload;
	if (payload.ParseFromString(data) == false) {
		return;
	}

	unsigned long id = payload.id();
	VisibleObject *obj = VisibleObjectManager::instance()->getObject(id);
	Ship *ship = dynamic_cast<Ship *>(obj);
	if (ship) {
		ship->setName(payload.name());
	}
}

void ProtobufHandler::handleMoveState(const std::string &data) {
	stars::MoveState payload;
	if (payload.ParseFromString(data) == false) {
		return;
	}

	unsigned long id = payload.id();
	VisibleObject *obj = VisibleObjectManager::instance()->getObject(id);
	if (!obj) {
		obj = new Ship("", id);
		VisibleObjectManager::instance()->addObject(obj);

		std::stringstream ss;
		ss << id;
		ProtobufHandler::send(m_conn, ss.str(), stars::WrapperMessage_Type_TYPE_ENTITY_INFO);
	}

	obj->setGlobalX(payload.x());
	obj->setGlobalY(payload.y());
	obj->setGlobalDestX(payload.dst_x());
	obj->setGlobalDestY(payload.dst_y());
// 	std::cout << obj->getNewAngle() << " " << payload
// 	obj->setNewAngle(payload.angle());
// 	obj->setAngle(payload.angle());

	obj->interpolate();
}

void ProtobufHandler::handleLogin(const std::string &data) {
	stars::Login payload;
	if (payload.ParseFromString(data) == false) {
		return;
	}

	if (payload.username().empty()) {
		LoginWindow *window = new LoginWindow();
		window->setInfoText("Bad username or password.");
		WindowManager::instance()->addWindow(window);
	}
	else {
		MapArea::instance()->setConnection(m_conn);
	}
}

void ProtobufHandler::handleData(char *data, unsigned long len) {
	std::string d(data, len);
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
			std::cout << "PARSING ERROR " << expected_size << "\n";
			m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);
			continue;
		}
		m_data.erase(m_data.begin(), m_data.begin() + 4 + expected_size);

		// Handle payload in wrapper message
		switch(wrapper.type()) {
			case stars::WrapperMessage_Type_TYPE_MOVE_STATE:
				handleMoveState(wrapper.payload());
				break;
			case stars::WrapperMessage_Type_TYPE_LOGIN:
				handleLogin(wrapper.payload());
				break;
			case stars::WrapperMessage_Type_TYPE_ENTITY_INFO:
				handleEntityInfo(wrapper.payload());
				break;
			default:
				break;
		}
	}
}
