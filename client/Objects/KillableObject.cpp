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

#include "KillableObject.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

KillableObject::KillableObject(unsigned long id, int hp, int hpps) : VisibleObject(id, 0, 0, 0, 0) {
	m_killed = false;
	m_maxhp = hp;
	m_hp = hp - 50;
	m_hpps = hpps;
	m_hpTimer = 0;
}

KillableObject::~KillableObject() {
}

void KillableObject::move(int diff_x, int diff_y, Uint32 deltaTicks) {
	if (m_killed) {
		return;
	}

	m_hpTimer += deltaTicks;
	if (m_hpTimer > 1000) {
		m_hpTimer = 0;
		changeHP(m_hpps);
	}
}

void KillableObject::changeHP(int hp) {
	m_hp += hp;
	if (m_hp > m_maxhp) {
		m_hp = m_maxhp;
	}
	if (m_hp < 0) {
		m_killed = true;
	}
}

void KillableObject::render(Surface *screen) {
	int green_w = (m_maxhp / 100) * m_hp;
	Surface *green = Surface::create(green_w, 10, 32);
	green->fill(0,255,0);
	green->blit(getX() - 50, getY() - getH()/2 - 20, screen);
	delete green;

	if (green_w != 100) {
		Surface *red = Surface::create(100 - green_w, 10, 32);
		red->fill(255,0,0);
		red->blit(getX() - 50 + green_w, getY() - getH()/2 - 20, screen);
		delete red;
	}
}

