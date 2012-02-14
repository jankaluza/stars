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

#include "sessionmanager.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "Protocol.pb.h"

#include "log.h"

DEFINE_LOGGER(logger, "SessionManager");

SessionManager *SessionManager::m_instance = NULL;

SessionManager::SessionManager() {
	m_instance = this;
}

SessionManager *SessionManager::instance() {
	return m_instance;
}

void SessionManager::handlePDLoginRegisterResponse(const std::string &data, bool log) {
	unifier::User payload;
	if (payload.ParseFromString(data) == false) {
		LOG_ERROR(logger, "handlePDLoginResponse invalid data");
		return;
	}

	std::map<unsigned long, Session::ref>::iterator it = m_sessions.find(payload.id());
	if (it == m_sessions.end()) {
		return;
	}

	Session::ref s = it->second;

	if (!payload.str1().empty()) {
		it->second->setAuthenticated(true, boost::lexical_cast<unsigned long>(payload.str2()), payload.str1());
		m_sessions[boost::lexical_cast<unsigned long>(payload.str2())] = it->second;
		m_sessions.erase(it->first);
	}
	else {
		it->second->setAuthenticated(false, 0, "");
	}

	stars::Login login;
	login.set_username(payload.str1());
	login.set_password("");

	if (log) {
		s->send(login, stars::WrapperMessage_Type_TYPE_LOGIN);
	}
	else {
		s->send(login, stars::WrapperMessage_Type_TYPE_REGISTER);
	}
}

void SessionManager::handleUnifierMessage(unifier::UnifierMessage &message) {
	switch (message.type()) {
		case unifier::UnifierMessage_Type_TYPE_USER_LOGIN:
			handlePDLoginRegisterResponse(message.payload(), true);
			break;
		case unifier::UnifierMessage_Type_TYPE_USER_REGISTER:
			handlePDLoginRegisterResponse(message.payload(), false);
			break;
		default:
			break;
	}
}

Session::ref SessionManager::getSession(unsigned long id) {
	std::map<unsigned long, Session::ref>::iterator it = m_sessions.find(id);
	if (it == m_sessions.end()) {
		return Session::ref();
	}

	return it->second;
}

void SessionManager::tick() {
	std::vector<Session::ref> moved;
	std::vector<unsigned long> stopped;

	for (std::map<unsigned long, Session::ref>::iterator it = m_sessions.begin(); it != m_sessions.end(); it++) {
		Session::ref session = it->second;
		session->tick();
		if (session->getState() & Session::Stopped) {
			stopped.push_back(it->first);
		}
		else if (session->getState() & Session::Moved) {
			moved.push_back(session);
		}
	}

	BOOST_FOREACH(unsigned long m, stopped) {
		m_sessions.erase(m);
	}

	stars::MoveState pos;
	BOOST_FOREACH(Session::ref m, moved) {
		m->storePosition(pos);
		for (std::map<unsigned long, Session::ref>::iterator it = m_sessions.begin(); it != m_sessions.end(); it++) {
			Session::ref session = it->second;
			if (session == m)
				continue;
			session->send(pos, stars::WrapperMessage_Type_TYPE_MOVE_STATE);
		}
	}
}

void SessionManager::start(Session::ref session) {
	m_sessions[session->getId()] = session;
	session->start();
}

void SessionManager::stop(Session::ref session) {
	session->stop();
}

