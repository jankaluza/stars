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

#include <boost/asio.hpp>
#include <string>
#include "UnifierProtocol.pb.h"

class AbstractService {
	public:
		AbstractService();
		virtual ~AbstractService();

		virtual void handleUnifierMessage(unifier::UnifierMessage &message) = 0;

		void sendToUnifier(unifier::UnifierMessage &message);
		void sendToUnifier(const std::string &to, ::google::protobuf::Message &msg, unifier::UnifierMessage_Type type);

		void connectToUnifier(const std::string &host, int port);

		void registerService(const std::string &name);

		void sendTock();

		void waitForTick();

		void waitForTock();

		void loop();

	private:
		int handleRead(const std::string &d, std::size_t bytes);
		std::string m_data;
		int m_sock;
};
