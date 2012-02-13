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

#include "Particle.h"
#include "Base/Surface.h"
#include "Base/Timer.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

Particle::Particle(int x, int y, int r) {
    //Set offsets
	m_x = x - 2;
	m_y = y - 2;

    //Initialize animation
    m_frame = rand() % 5;
	m_particle = Surface::createFromFile("shimmer.bmp");
}

Particle::~Particle() {
	delete m_particle;
}

void Particle::move(int diff_x, int diff_y, Uint32 deltaTicks) {
	m_frame++;
	m_x += diff_x;
	m_y += diff_y;
}

void Particle::render(Surface *screen) {
	m_particle->blit(m_x, m_y, screen);
}

