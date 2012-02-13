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

#include "server.h"
#include <boost/bind.hpp>
#include <iostream>
#include "log.h"
#include <unistd.h>
#include "string.h"
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

DEFINE_LOGGER(logger, "Server");

Server *Server::m_instance = NULL;

Server::Server(const std::string &hostname, int port) {
	m_instance = this;
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(m_sock, F_SETFL, O_NONBLOCK);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);

	while(bind(m_sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		LOG_ERROR(logger, "Main socket creation error: " << strerror(errno));
		usleep(1000000);
	}

	LOG_INFO(logger, "Socket created");

	listen(m_sock, 100);
}

void Server::handleUnifierMessage(unifier::UnifierMessage &message) {
	m_sessionManager.handleUnifierMessage(message);
}

void Server::start() {
	connectToUnifier("localhost", 30000);
	registerService("FE");

	int fd = 0;
	unsigned long ticks = 0;

	LOG_INFO(logger, "Starting the server");
	while (true) {
		waitForTick();
		while((fd = accept(m_sock, NULL, NULL)) > 0) {
			handleAccept(fd);
		}

		m_sessionManager.tick();

		sendTock();
		ticks++;
	}
}

void Server::stop() {
	LOG_INFO(logger, "Stopping the server");
// 	m_ioService.post(boost::bind(&Server::handleStop, this));
}

void Server::handleAccept(int fd) {
	LOG_INFO(logger, "Accepted new client connection " << fd);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	Session::ref session(new Session(fd));
	m_sessionManager.start(session);
}

void Server::handleStop() {
	close(m_sock);
}
