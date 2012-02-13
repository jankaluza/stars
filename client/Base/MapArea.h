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
#include <vector>

class Surface;
class Ship;
class VisibleObject;
class SelectableObject;
class Connection;

#define MAP_POINT_SIZE 5

class MapArea {
	public:
		~MapArea();

		void setConnection(Connection *conn) {
			m_conn = conn;
		}

		void handleInput(SDL_Event &event);

		void move(Uint32 deltaTicks);

		void render(Surface *screen);

		int getGlobalX() {
			return m_camera.x / MAP_POINT_SIZE;
		}

		int getGlobalY() {
			return m_camera.y / MAP_POINT_SIZE;
		}

		static MapArea *instance();

	private:
		MapArea();

		static MapArea *m_instance;
		SelectableObject *m_selected;
		Ship *m_ship;
		Surface *m_area;
		SDL_Rect m_camera;
		float m_xVel;
		float m_yVel;
		float m_speed;
		int mouse_x;
		int mouse_y;
		int moving_x;
		int moving_y;
		Connection *m_conn;
};
