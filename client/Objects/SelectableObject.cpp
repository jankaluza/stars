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

#include "SelectableObject.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

SelectableObject::SelectableObject(unsigned long id) : VisibleObject(id, 0, 0, 0, 0) {
	m_selected = false;
}

SelectableObject::~SelectableObject() {
}

bool SelectableObject::trySelect(int x, int y) {
	if (x > getX() - getW()/2 && x < getX() + getW()/2 && y > getY() - getH()/2 && y < getY() + getH()/2) {
		m_selected = true;
		return true;
	}
	m_selected = false;
	return false;
}

void SelectableObject::render(Surface *screen) {
	if (!m_selected) {
		return;
	}

	Surface *square = Surface::create(getW() + 10, getH() + 10, 32);
	square->fill(255, 255, 255);
	SDL_Rect rect = {1, 1, getW() + 8, getH() + 8};
	square->fill(&rect, 255, 0, 255);

	square->setColorKey(255, 0, 255);
	
	square->blit(getX() - square->getW()/2, getY() - square->getH()/2, screen);
	delete square;
}

