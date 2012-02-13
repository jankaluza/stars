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

#include "Ship.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Base/MapArea.h"
#include "Particle.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_gfxPrimitives.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

Ship::Ship(const std::string &name, unsigned long playerID) : KillableObject(playerID, 100, 10) {
	m_state = Done;
	m_ship = Surface::createFromFile("ship.png");
	m_rotated = Surface::createFromFile("ship.png");
	m_angle = 0;
	m_playerID = playerID;
	m_playerName = name;
	m_target = NULL;
	m_fireTimer = 0;
	m_interpolating = false;

	setX(800/2);
	setY(600/2);
	for( int p = 0; p < 20; p++ ) {
		m_particles[ p ] = new Particle(0, 0);
	}
}

Ship::~Ship() {
	delete m_ship;
	delete m_rotated;
}

void Ship::fire(VisibleObject *object) {
	if (!m_target) {
		m_target = object;
	}
}

void Ship::setAngle(double angle) {
	m_angle = angle;
	delete m_rotated;
	m_rotated = Surface::createRotated(m_ship, - 90 - angle * 180 / 3.1415, 1);
}

void Ship::setAngleAnimate(double angle) {
	m_state = Rotating;
	m_newAngle = angle;

	if (abs(m_newAngle + 2*3.14 - (m_angle + 2*3.14)) > 3.14/2) {
		m_state = RotatingSlow;
	}
}

void Ship::move(int diff_x, int diff_y, Uint32 deltaTicks) {
	if (m_interpolating) {
// 		m_interpolating -= deltaTicks;
		setX(getX() + diff_x);
		setY(getY() + diff_y);
		m_destX += diff_x;
		m_destY += diff_y;
// 		interpolate();
	}

// 	std::cout << m_newAngle << " " << m_angle << "\n";
	if (m_state != Done) {
		if (m_newAngle > 0 && m_angle < 0 && 3.14 - m_newAngle + 3.14 + m_angle < m_newAngle - m_angle) {
			m_angle -= 0.060;
			if (m_newAngle < m_angle) {
				m_angle = m_newAngle;
				m_state = Done;
			}
		}
		else if (m_newAngle < 0 && m_angle > 0 && 3.14 + m_newAngle + 3.14 - m_angle < m_angle - m_newAngle) {
			m_angle += 0.060;
			if (m_newAngle > m_angle) {
				m_angle = m_newAngle;
				m_state = Done;
			}
		}
		else if (m_newAngle < m_angle) {
			m_angle -= 0.060;
			if (m_newAngle > m_angle) {
				m_angle = m_newAngle;
				m_state = Done;
			}
		}
		else if (m_newAngle > m_angle) {
			m_angle += 0.060;
			if (m_newAngle < m_angle) {
				m_angle = m_newAngle;
				m_state = Done;
			}
		}
		if (m_angle > 3.14) {
			m_angle = -3.14;
		}
		else if (m_angle < -3.14) {
			m_angle = 3.14;
		}
		setAngle(m_angle);
	}

	if ((m_interpolating) && (m_xVel != 0 || m_yVel !=0)) {
		double xVel = m_xVel;
		double yVel = m_yVel;
		if (getState() == Ship::RotatingSlow) {
			xVel *= 0.3;
			yVel *= 0.3;
		}
		if (getState() != Ship::Done) {
			xVel *= 0.8;
			yVel *= 0.8;
		}

		if (getGlobalX() != getGlobalDestX()) {
			int dx = xVel * ( deltaTicks / 1000.f );
			if (dx == 0 && xVel < 0) { dx = -1; }
			else if (dx == 0 && xVel > 0) { dx = 1; }
			if (getGlobalX() > getGlobalDestX()) {
				setX(getX() + dx);
				if (getGlobalX() < getGlobalDestX()) {
					setGlobalX(m_destX);
				}
			}
			else if (getGlobalX() < getGlobalDestX()) {
				setX(getX() + dx);
				if (getGlobalX() > getGlobalDestX()) {
					setGlobalX(m_destX);
				}
			}
			else {
				setX(getX() + dx);
			}
		}

		if (getGlobalY() != getGlobalDestY()) {
			int dy = yVel * ( deltaTicks / 1000.f );
			if (dy == 0 && yVel < 0) { dy = -1; }
			else if (dy == 0 && yVel > 0) { dy = 1; }
			if (getGlobalY() > getGlobalDestY()) {
				setY(getY() + dy);
				if (getGlobalY() < getGlobalDestY()) {
					setGlobalY(m_destY);
				}
			}
			else if (getGlobalY() < getGlobalDestY()) {
				setY(getY() + dy);
				if (getGlobalY() > getGlobalDestY()) {
					setGlobalY(m_destY);
				}
			}
			else {
				setY(getY() + dy);
			}
		}

		if (getGlobalX() == getGlobalDestX() && getGlobalY() == getGlobalDestY()) {
			m_xVel = 0;
			m_yVel = 0;
		}
	}


    for( int p = 0; p < 20; p++ )
    {
        //Delete and replace dead particles
            m_particles[ p ]->move(diff_x, diff_y, deltaTicks);
     
    }

	KillableObject::move(diff_x, diff_y, deltaTicks);

	if (m_target) {
		m_fireTimer += deltaTicks;
		if (m_fireTimer > 500) {
			m_target = NULL;
			m_fireTimer = 0;
		}
	}
}

void Ship::interpolate() {
	m_interpolating = true;
	double shipangle = atan2(getDestY() - getY(), getDestX() - getX());
	setXVel(200 * cos(shipangle));
	setYVel(200 * sin(shipangle));

	setAngleAnimate(shipangle);
}

void Ship::render(Surface *screen) {
	//Go through particles
	for( int p = 0; p < 20; p++ )
	{
		//Delete and replace dead particles
		if( m_particles[ p ]->isDead() == true )
		{
			delete m_particles[ p ];

			m_particles[ p ] = new Particle(getX() - 3 + rand() % 15 , getY() - 3 + rand() % 15);
		}
	}


	//Show particles
	for( int p = 0; p < 20; p++ )
	{
		m_particles[ p ]->render(screen);
	}

	if (m_target) {
		screen->line(getX(), getY(), m_target->getX(), m_target->getY(), 0, 255, 0);
		screen->line(getX()-1, getY()-1, m_target->getX()-1, m_target->getY()-1, 0, 128, 0);
		screen->line(getX()-1, getY(), m_target->getX()-1, m_target->getY(), 0, 128, 0);
		screen->line(getX(), getY()-1, m_target->getX(), m_target->getY()-1, 0, 128, 0);
	}

	m_rotated->blit(getX() - m_rotated->getW()/2, getY() - m_rotated->getH()/2, screen);
	setW(m_rotated->getW());
	setH(m_rotated->getH());

	screen->renderCenteredText(getX(), getY() - getH()/2 - 10, m_playerName);

	KillableObject::render(screen);
}

