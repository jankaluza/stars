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

#include "VisibleObjectManager.h"
#include "VisibleObject.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

VisibleObjectManager *VisibleObjectManager::m_instance = NULL;

VisibleObjectManager *VisibleObjectManager::instance() {
	if (VisibleObjectManager::m_instance == NULL) {
		VisibleObjectManager::m_instance = new VisibleObjectManager();
	}

	return VisibleObjectManager::m_instance;
}

VisibleObjectManager::VisibleObjectManager() {
}

VisibleObjectManager::~VisibleObjectManager() {
}

void VisibleObjectManager::addObject(VisibleObject *object) {
	m_objects[object->getID()] = object;
}

VisibleObject *VisibleObjectManager::getObject(unsigned long id) {
	std::map<unsigned long, VisibleObject *>::iterator it = m_objects.find(id);
	if (it == m_objects.end()) {
		return NULL;
	}
	return it->second;
}

void VisibleObjectManager::render(Surface *screen) {
	for (std::map<unsigned long, VisibleObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
		it->second->render(screen);
	}
}

void VisibleObjectManager::move(int diff_x, int diff_y, Uint32 deltaTicks) {
	for (std::map<unsigned long, VisibleObject *>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
		it->second->move(diff_x, diff_y, deltaTicks);
	}
}
