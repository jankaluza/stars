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

#include "LineEditWindow.h"
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

LineEditWindow::LineEditWindow(const std::string &name, const std::string &info) : Window(info, 100, 100, 200, 300) {
	setName(name);
	setX(GameLoop::getW()/2 - getW()/2);
	setY(GameLoop::getH()/2 - getH()/2);
	Window::createFromFile(this, "Resources/Windows/LineEditWindow.xml");

	m_edit = dynamic_cast<LineEdit *>(getWidget("edit"));
	m_info = dynamic_cast<Label *>(getWidget("info"));
	m_info->setText(info);

	m_handler = NULL;
}

LineEditWindow::~LineEditWindow() {
}

const std::string &LineEditWindow::getText() {
	return m_edit->getText();
}

void LineEditWindow::handleButtonClicked(Button *button) {
	if (button->getName() == "ok") {
		if (m_handler) {
			m_handler->handleLineEditText(getName(), getText());
		}
	}
	destroy();
}
