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
#include "VisibleObject.h"

class Surface;
class Particle;

class KillableObject : public VisibleObject {
	public:
		KillableObject(unsigned long id, int hp, int hpps);
		virtual ~KillableObject();

		void changeHP(int change);

		virtual void render(Surface *screen);

		virtual void move(int diff_x, int diff_y, Uint32 deltaTicks);

		int getHP() {
			return m_hp;
		}

		bool isKilled() {
			return m_killed;
		}

	private:
		int m_hpTimer;
		int m_maxhp;
		int m_hp;
		int m_hpps;
		bool m_killed;
};
