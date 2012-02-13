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

#include <map>
#include <vector>

#include "SDL/SDL.h"

class Window;
class Surface;

class WindowManager {
	public:

		virtual ~WindowManager();

		void addWindow(Window *object);
		void removeWindow(Window *object);

		void render(Surface *screen);

		bool handleEvent(SDL_Event &event);

		static WindowManager *instance();

	private:
		WindowManager();
		static WindowManager *m_instance;
		std::vector<Window *> m_windows;
};
