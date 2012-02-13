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
#include <string>
#include <vector>

class Connection;
class DataHandler;

class ConnectionManager {
	public:
		virtual ~ConnectionManager();

		Connection *connect(DataHandler *handler, const std::string &hostname, int port);

		void handleConnections();

		static ConnectionManager *instance();

	private:
		ConnectionManager();

		static ConnectionManager *m_instance;
		SDLNet_SocketSet m_set;
		std::vector<Connection *> m_conns;
};
