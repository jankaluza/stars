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

#include "Widget.h"
#include "SDL/SDL.h"
#include <string>
#include <vector>
#include "Base/Timer.h"

class Surface;

class Button : public Widget {
	public:
		Button(const std::string &name = "", int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Button();

		bool isClicked() {
			return m_clicked;
		}

		void setClicked(bool clicked) {
			m_clicked = clicked;
		}

		const std::string &getText() {
			return m_name;
		}

		void setText(const std::string &text) {
			m_name = text;
		}

		virtual void render(Surface *screen);

		virtual void handleEvent(SDL_Event &event);

	private:
		std::string m_name;
		bool m_clicked;
};
