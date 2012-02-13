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

#pragma once

#include "SDL/SDL_net.h"
#include <vector>
#include "DataHandler.h"
#include <string>
#include "Protocol.pb.h"

class Connection;

class ProtobufHandler : public DataHandler {
	public:
		ProtobufHandler();
		virtual ~ProtobufHandler();

		void setConnection(Connection *);

		void handleData(char *data, unsigned long len);

		void sendPong();

		void send(const std::string &data);

		static void send(Connection *conn, ::google::protobuf::Message &msg, stars::WrapperMessage_Type type);
		static void send(Connection *conn, const std::string &data, stars::WrapperMessage_Type type);

	private:
		void handleMoveState(const std::string &data);
		void handleLogin(const std::string &data);
		void handleEntityInfo(const std::string &data);

		Connection *m_conn;
		std::string m_data;
};
