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
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "log.h"
#include <unistd.h>
#include "string.h"
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include "UnifierProtocol.pb.h"

DEFINE_LOGGER(logger, "PersistentDataService");
static int id;

Server::Server() {
}

void Server::handleUnifierMessage(unifier::UnifierMessage &message) {
	switch (message.type()) {
		case unifier::UnifierMessage_Type_TYPE_USER_LOGIN:
			handleUserlogin(message.payload());
			break;
		default:
			break;
	}
}

void Server::handleUserlogin(const std::string &data) {
	unifier::User payload;
	if (payload.ParseFromString(data) == false) {
		LOG_ERROR(logger, "handleUserLogin invalid data");
		return;
	}

	unifier::User user;
	user.set_id(payload.id());
	user.set_str1(payload.str1());
	user.set_str2(boost::lexical_cast<std::string>(++id));
	sendToUnifier("FE", user, unifier::UnifierMessage_Type_TYPE_USER_LOGIN);
}

void Server::start() {
	connectToUnifier("localhost", 30000);
	registerService("PD");

	LOG_INFO(logger, "Starting the server");
	loop();
}

void Server::stop() {
	LOG_INFO(logger, "Stopping the server");
// 	m_ioService.post(boost::bind(&Server::handleStop, this));
}
