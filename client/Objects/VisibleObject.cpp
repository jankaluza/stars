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

#include "VisibleObject.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Base/MapArea.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

VisibleObject::VisibleObject(unsigned long id, int x, int y, int width, int height) {
	m_id = id;
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_destX = 0;
	m_destY = 0;
	m_xVel = 0;
	m_yVel = 0;
}

VisibleObject::~VisibleObject() {
}

void VisibleObject::setGlobalX(int x) {
	int cameraX = MapArea::instance()->getGlobalX();
	m_x = (x - cameraX) * MAP_POINT_SIZE;
}

int VisibleObject::getGlobalX() {
	int cameraX = MapArea::instance()->getGlobalX();
	return cameraX + m_x / MAP_POINT_SIZE;
}

void VisibleObject::setGlobalY(int y) {
	int cameraY = MapArea::instance()->getGlobalY();
	m_y = (y - cameraY) * MAP_POINT_SIZE;
}

int VisibleObject::getGlobalY() {
	int cameraY = MapArea::instance()->getGlobalY();
	return cameraY + m_y / MAP_POINT_SIZE;
}

void VisibleObject::setGlobalDestX(int x) {
	int cameraX = MapArea::instance()->getGlobalX();
	m_destX = (x - cameraX) * MAP_POINT_SIZE;
}

int VisibleObject::getGlobalDestX() {
	int cameraX = MapArea::instance()->getGlobalX();
	return cameraX + m_destX / MAP_POINT_SIZE;
}

void VisibleObject::setGlobalDestY(int y) {
	int cameraY = MapArea::instance()->getGlobalY();
	m_destY = (y - cameraY) * MAP_POINT_SIZE;
}

int VisibleObject::getGlobalDestY() {
	int cameraY = MapArea::instance()->getGlobalY();
	return cameraY + m_destY / MAP_POINT_SIZE;
}
