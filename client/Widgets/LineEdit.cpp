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

#include "LineEdit.h"
#include "Base/Surface.h"
#include "Widgets/Widget.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

LineEdit::LineEdit(const std::string &name, int x, int y, int width, int height) : Widget(x, y, width, height) {
	m_name = name;
	m_focus = false;
}

LineEdit::~LineEdit() {
}

void LineEdit::render(Surface *screen) {
	screen->box(getX(), getY(), getW(), getH(), 255, 255, 255);
	int w = screen->renderText(getX() + 3, getY() +2, m_string.c_str()) + 1;

	if (hasFocus()) {
		if (m_timer.getTicks() < 500) {
			screen->line(w + getX() + 3, getY() + 2, w + getX() + 3, getY() + getH() - 2, 255, 255, 255);
		}
		else if (m_timer.getTicks() > 700) {
			m_timer.start();
		}
	}
// 	screen->renderText(getX(), getY(), m_text.c_str());
}

void LineEdit::handleEvent(SDL_Event &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		setFocus(not hasFocus());
		if (hasFocus()) {
			m_timer.start();
		}
	}
	else if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_BACKSPACE) {
			if (!m_string.empty()) {
				m_string.erase(m_string.length() - 1);
			}
		}
		else if (isalnum(event.key.keysym.unicode) || (char) event.key.keysym.unicode == ' ') {
			m_string += (char) event.key.keysym.unicode;
		}
	}
}

