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

class Surface;

class VisibleObject {
	public:
		VisibleObject(unsigned long id, int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~VisibleObject();

		virtual void render(Surface *screen) = 0;

		virtual void move(int diff_x, int diff_y, Uint32 deltaTicks) = 0;

		virtual void interpolate() {}

		unsigned long getID() { return m_id; }
		void setID(unsigned long id) { m_id = id; }

		void setGlobalX(int x);
		int getGlobalX();
		void setGlobalY(int y);
		int getGlobalY();
		void setGlobalDestX(int x);
		int getGlobalDestX();
		void setGlobalDestY(int y);
		int getGlobalDestY();

		int getX() { return m_x; }
		void setX(int x) { m_x = x; }
		int getY() { return m_y; }
		void setY(int y) { m_y = y; }
		int getW() { return m_width; }
		void setW(int w) { m_width = w; }
		int getH() { return m_height; }
		void setH(int h) { m_height = h; }
		int getDestX() { return m_destX; }
		void setDestX(int x) { m_destX = x; }
		int getDestY() { return m_destY; }
		void setDestY(int y) { m_destY = y; }
		int getXVel() { return m_xVel; }
		void setXVel(int x) { m_xVel = x; }
		int getYVel() { return m_yVel; }
		void setYVel(int y) { m_yVel = y; }

		void setAngle(double angle) { m_angle = angle; }
		void setNewAngle(double angle) { m_newAngle = angle; }
		double getAngle() {
			return m_angle;
		}

		double getNewAngle() {
			return m_newAngle;
		}

	private:
		unsigned long m_id;
		int m_x;
		int m_y;
		int m_width;
		int m_height;

	protected:
		int m_destX;
		int m_destY;
		int m_xVel;
		int m_yVel;
		double m_angle;
		double m_newAngle;
};
