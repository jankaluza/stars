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
#include "UnifierProtocol.pb.h"

DEFINE_LOGGER(logger, "TickingService");

Server::Server() {
}

void Server::handleUnifierMessage(unifier::UnifierMessage &message) {
	
}

void Server::start() {
	connectToUnifier("localhost", 30000);
	registerService("TS");

	unsigned long ticks;

	LOG_INFO(logger, "Starting the server");
	while (true) {
		unifier::UnifierMessage tick;
		tick.set_type(unifier::UnifierMessage_Type_TYPE_TICK);
		tick.set_to("");

// 		LOG_INFO(logger, "tick");
		sendToUnifier(tick);
// 		int ticking_services = 1;
// 		while (ticking_services != 0) {
// 			waitForTock();
// 			ticking_services--;
// 		}

		ticks++;
		usleep(100000);
	}
}

void Server::stop() {
	LOG_INFO(logger, "Stopping the server");
// 	m_ioService.post(boost::bind(&Server::handleStop, this));
}
