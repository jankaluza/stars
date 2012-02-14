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

#include "LoginWindow.h"
#include "Widgets/Label.h"
#include "Widgets/LineEdit.h"
#include "Widgets/Button.h"

#include "Base/MapArea.h"
#include "Base/GameLoop.h"
#include "Network/ConnectionManager.h"
#include "Network/Connection.h"
#include "Network/ProtobufHandler.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

LoginWindow::LoginWindow() : Window("Login", 100, 100, 200, 300) {
	setX(GameLoop::getW()/2 - getW()/2);
	setY(GameLoop::getH()/2 - getH()/2);
	Window::createFromFile(this, "Resources/Windows/LoginWindow.xml");

	m_username = dynamic_cast<LineEdit *>(getWidget("username"));
	m_password = dynamic_cast<LineEdit *>(getWidget("password"));
	m_info = dynamic_cast<Label *>(getWidget("info"));
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::setInfoText(const std::string &text) {
	m_info->setText(text);
}

void LoginWindow::handleButtonClicked(Button *button) {
	m_info->setText("Connecting...");
	Connection *conn = ConnectionManager::instance()->connect(new ProtobufHandler(), "localhost", 30001);
	if (conn != NULL && conn->isConnected()) {
		stars::Login payload;
		payload.set_username(m_username->getText());
		payload.set_password(m_password->getText());

		if (button->getName() == "register") {
			ProtobufHandler::send(conn, payload, stars::WrapperMessage_Type_TYPE_REGISTER);
		}
		else {
			ProtobufHandler::send(conn, payload, stars::WrapperMessage_Type_TYPE_LOGIN);
		}
		destroy();
		return;
	}

	m_info->setText("Unable to connect the server.");

}
