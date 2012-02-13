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

#pragma once

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <string>
#include "Protocol.pb.h"

class RequestHandler;
class RequestParser;

class Session : public boost::enable_shared_from_this<Session> {
	public:
		typedef boost::shared_ptr<Session> ref;

		enum State {None = 0, Moved = 2, Stopped = 4};

		Session(int socket);
		~Session();

		int getSocket();

		void tick();

		void start();

		void stop();

		int getState() {
			return m_state;
		}

		void setAuthenticated(bool authenticated, unsigned long id, const std::string &username) {
			m_authenticated = authenticated;
			m_id = id;
			m_username = username;
		}

		unsigned long getId() {
			return m_id;
		}

		bool isAuthenticated() {
			return m_authenticated;
		}

		const std::string &getUsername() {
			return m_username;
		}

		void storePosition(stars::MoveState &state);
		void send(::google::protobuf::Message &payload, stars::WrapperMessage_Type type);
		void send(const std::string &payload, stars::WrapperMessage_Type type);

	private:
		void handleRead(const std::string &data, std::size_t bytes);

		void handleLogin(const std::string &payload);
		void handleMove(const std::string &payload);
		void handleEntityInfo(const std::string &payload);

		int m_socket;
		bool m_authenticated;
		std::string m_data;
		unsigned long m_id;
		size_t parsed_total;
		size_t bytes;
		std::string m_username;
		int m_state;
		int rotating;

		int m_x;
		int m_y;
		int m_xVel;
		int m_yVel;
		int m_destX;
		int m_destY;
		double m_newAngle;
		double m_angle;
		int m_bpsTimer;
		int m_bpsIn;
		int m_bpsOut;
};
