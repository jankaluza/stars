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

#include "Widgets/Window.h"
#include "SDL/SDL.h"
#include <string>

class Button;
class LineEdit;
class Label;

class LineEditHandler {
	public:
		virtual ~LineEditHandler() {}

		virtual void handleLineEditText(const std::string &name, const std::string &text) = 0;
};

class LineEditWindow : public Window {
	public:
		LineEditWindow(const std::string &name, const std::string &info);
		virtual ~LineEditWindow();

		void handleButtonClicked(Button *button);

		const std::string &getText();

		void setHandler(LineEditHandler *handler) {
			m_handler = handler;
		}

	private:
		LineEdit *m_edit;
		Label *m_info;
		LineEditHandler *m_handler;
};
