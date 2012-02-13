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

#include "Connection.h"
#include "DataHandler.h"
#include <string>
#include <sstream>
#include <iostream>

#define MAX_LEN 65535

Connection::Connection(DataHandler *handler, IPaddress ip, int port) {
	m_ip = ip;
	m_port = port;
	m_handler = handler;
	m_handler->setConnection(this);
}

Connection::~Connection() {
	stop();
}

void Connection::readData() {
	char buf[MAX_LEN];
	Uint32 result;

	result = SDLNet_TCP_Recv(m_sock, buf, MAX_LEN);

	m_handler->handleData(buf, result);
}

void Connection::sendData(const char *data, unsigned long len) {
	SDLNet_TCP_Send(m_sock, data, len);
}

void Connection::stop() {
	SDLNet_TCP_Close(m_sock);
}

void Connection::start() {
	m_sock = SDLNet_TCP_Open(&m_ip);
}

