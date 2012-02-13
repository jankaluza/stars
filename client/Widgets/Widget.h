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

#include "SDL/SDL.h"
#include <string>

class Surface;

class Widget {
	public:
		Widget(int x = 0, int y = 0, int w = 0, int h = 0);
		virtual ~Widget();

		virtual void render(Surface *screen) = 0;

		virtual void handleEvent(SDL_Event &event) = 0;

		int getX() { return m_x; }
		void setX(int x) { m_x = x; }
		int getY() { return m_y; }
		void setY(int y) { m_y = y; }
		int getW() { return m_width; }
		void setW(int w) { m_width = w; }
		int getH() { return m_height; }
		void setH(int h) { m_height = h; }
		void setName(const std::string &name) { m_name = name; }
		std::string &getName() { return m_name; }

		void setFocus(bool focus) { m_focus = focus; }
		bool hasFocus() { return m_focus; }

	private:
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		bool m_focus;
		std::string m_name;
};
