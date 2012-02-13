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

#include "Star.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

Star::Star(unsigned long id) : SelectableObject(id) {
	m_star = Surface::createFromFile("star.png");
	setW(m_star->getW() + 30);
	setH(m_star->getH() + 30);

	for( int p = 0; p < 200; p++ ) {
		m_particles.push_back(new Particle(0, 0));
	}
}

Star::~Star() {
	delete m_star;
}

void Star::move(int diff_x, int diff_y, Uint32 deltaTicks) {
	setX(getX() + diff_x);
	setY(getY() + diff_y);

    for( int p = 0; p < m_particles.size(); p++ )
    {
        //Delete and replace dead particles
            m_particles[ p ]->move(diff_x, diff_y, deltaTicks);
     
    }
}

void Star::render(Surface *screen) {
	//Go through particles
	for( int p = 0; p < m_particles.size(); p++ )
	{
		//Delete and replace dead particles
		if( m_particles[ p ]->isDead() == true )
		{
			delete m_particles[ p ];
			
			double angle = atan2(rand() * 50, rand()*50);
			int x = 61 * cos(angle) + rand() % 15;
			int y = 61 * sin(angle) + rand() % 15;
			m_particles[p] = new Particle(getX() + x - 7, getY() + y - 7);
		}
	}


	//Show particles
	for( int p = 0; p < m_particles.size(); p++ )
	{
		m_particles[ p ]->render(screen);
	}

// std::cout << getX() - m_star->getW()/2 << " " << getY() - m_star->getH()/2 << "\n";
	m_star->blit(getX() - m_star->getW()/2, getY() - m_star->getH()/2, screen);
	SelectableObject::render(screen);
}

