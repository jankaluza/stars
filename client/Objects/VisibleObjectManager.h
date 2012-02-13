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

class VisibleObject;
class Surface;

class VisibleObjectManager {
	public:

		virtual ~VisibleObjectManager();

		void addObject(VisibleObject *object);
		VisibleObject *getObject(unsigned long id);

		virtual void render(Surface *screen);

		virtual void move(int diff_x, int diff_y, Uint32 deltaTicks);

		void removeObjectsFromQuadrant(int qx, int qy) {}

		static VisibleObjectManager *instance();

	private:
		VisibleObjectManager();
		static VisibleObjectManager *m_instance;
		std::map<unsigned long, VisibleObject *> m_objects;
};
