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

#include "ConnectionManager.h"
#include "Connection.h"
#include "DataHandler.h"
#include <string>
#include <sstream>

ConnectionManager *ConnectionManager::m_instance = NULL;

ConnectionManager::ConnectionManager() {
	m_set = SDLNet_AllocSocketSet(32);
}

ConnectionManager *ConnectionManager::instance() {
	if (m_instance == NULL) {
		m_instance = new ConnectionManager();
	}

	return m_instance;
}

ConnectionManager::~ConnectionManager() {
	for (std::vector<Connection *>::const_iterator it = m_conns.begin(); it != m_conns.end(); it++) {
		Connection *conn = *it;
		delete conn;
	}
	SDLNet_FreeSocketSet(m_set);
}

void ConnectionManager::handleConnections() {
	int numready = SDLNet_CheckSockets(m_set, 0);
	if (numready == 0 || numready == -1) {
		return;
	}

	for (std::vector<Connection *>::const_iterator it = m_conns.begin(); it != m_conns.end(); it++) {
		Connection *conn = *it;
		if (SDLNet_SocketReady(conn->m_sock)) {
			conn->readData();
		}
	}
}

Connection *ConnectionManager::connect(DataHandler *handler, const std::string &hostname, int port) {
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, hostname.c_str(), port) == -1) {
		return NULL;
	}

	Connection *conn = new Connection(handler, ip, port);
	conn->start();

	SDLNet_TCP_AddSocket(m_set, conn->m_sock);
	m_conns.push_back(conn);

	return conn;
}
