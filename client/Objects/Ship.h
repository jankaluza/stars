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
#include "KillableObject.h"
#include <string>

class Surface;
class Particle;

class Ship : public KillableObject {
	public:
		typedef enum { Done, Rotating, RotatingSlow } State;
		Ship(const std::string &name, unsigned long playerID = 0);
		virtual ~Ship();

		virtual void render(Surface *screen);

		virtual void move(int diff_x, int diff_y, Uint32 deltaTicks);

		virtual void interpolate();

		void fire(VisibleObject *object);

		void setAngle(double angle);
		void setAngleAnimate(double angle);

		void setName(const std::string &name) {
			m_playerName = name;
		}

		State &getState() {
			return m_state;
		}

	private:
		Surface *m_ship;
		Surface *m_rotated;
		State m_state;
		Particle *m_particles[20];
		unsigned long m_playerID;
		std::string m_playerName;
		VisibleObject *m_target;
		int m_fireTimer;
		bool m_interpolating;
};
