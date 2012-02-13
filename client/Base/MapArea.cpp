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

#include "MapArea.h"
#include "Surface.h"
#include "Timer.h"
#include "Objects/Ship.h"
#include "Objects/VisibleObject.h"
#include "Objects/VisibleObjectManager.h"
#include "Objects/Star.h"
#include "Network/Connection.h"
#include "Network/ProtobufHandler.h"
#include "Network/Protocol.pb.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

MapArea *MapArea::m_instance = NULL;

MapArea *MapArea::instance() {
	if (MapArea::m_instance == NULL) {
		MapArea::m_instance = new MapArea();
	}

	return MapArea::m_instance;
}

MapArea::MapArea() {
	m_xVel = 0;
	m_yVel = 0;
	m_speed = 200;
	m_camera.x = 0;
	m_camera.y = 0;
	m_camera.w = 800;
	m_camera.h = 600;
	m_area = Surface::createFromFile("space.jpg");
	m_ship = new Ship("HanzZ", 1);
	m_selected = NULL;
	m_conn = NULL;
}

MapArea::~MapArea() {
	delete m_ship;
	if (m_area) {
		delete m_area;
	}
}

void MapArea::handleInput(SDL_Event &event) {
	if (!m_conn) {
		return;
	}
	if (event.type == SDL_MOUSEMOTION) {
		int x = event.motion.x;
		int y = event.motion.y;

		mouse_x = (m_camera.x + x) / MAP_POINT_SIZE;
		mouse_y = (m_camera.y + y) / MAP_POINT_SIZE;
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			int x = event.motion.x;
			int y = event.motion.y;

// 			SelectableObject *selected = NULL;
// 			for (int x_ = 0; x_ < m_objects.size(); x_++) {
// 				for (int y_ = 0; y_ < m_objects[x_].size(); y_++) {
// 					if (m_objects[x_][y_]) {
// 						SelectableObject *obj = dynamic_cast<SelectableObject *>(m_objects[x_][y_]);
// 						if (obj) {
// 							if (obj->trySelect(x, y)) {
// 								selected = obj;
// 								break;
// 							}
// 							
// 						}
// 					}
// 				}
// 			}
// 			if (selected) {
// 				if (m_selected)
// 					m_selected->setSelected(false);
// 				m_selected = selected;
// 				return;
// 			}
// 			if (m_selected) {
// 				m_selected->setSelected(true);
// 			}

			mouse_x = (m_camera.x + x) / MAP_POINT_SIZE;
			mouse_y = (m_camera.y + y) / MAP_POINT_SIZE;

			moving_x = mouse_x;
			moving_y = mouse_y;

			int center_x = (m_camera.w/2);
			int center_y = (m_camera.h/2);

			double shipangle = atan2(y - center_y, x - center_x);
			m_xVel = m_speed * cos(shipangle);
			m_yVel = m_speed * sin(shipangle);

			m_ship->setAngleAnimate(shipangle);

			if (m_conn) {
				stars::MoveState payload;
				payload.set_id(0);
				payload.set_x((m_camera.x + center_x) / MAP_POINT_SIZE);
				payload.set_y((m_camera.y + center_y) / MAP_POINT_SIZE);
				payload.set_dst_x(mouse_x);
				payload.set_dst_y(mouse_y);
				payload.set_angle(shipangle);

				ProtobufHandler::send(m_conn, payload, stars::WrapperMessage_Type_TYPE_MOVE_STATE);
			}

		}
	}
	else if (event.type == SDL_KEYDOWN) {
		
		if( event.key.keysym.sym == SDLK_1) {
			if (m_selected) {
				m_ship->fire(m_selected);
			}
		}
	}
}

void MapArea::move(Uint32 deltaTicks) {
	int diff_x = 0;
	int diff_y = 0;

	if (m_xVel != 0 || m_yVel !=0) {
		double xVel = m_xVel;
		double yVel = m_yVel;
		if (m_ship->getState() == Ship::RotatingSlow) {
			xVel *= 0.3;
			yVel *= 0.3;
		}
		if (m_ship->getState() != Ship::Done) {
			xVel *= 0.8;
			yVel *= 0.8;
		}
		int center_x = (m_camera.x + m_camera.w/2) / MAP_POINT_SIZE;
// 		center_x = x/m + w/m
// 		center_x - w/m = x/m
		
		int center_y = (m_camera.y + m_camera.h/2) / MAP_POINT_SIZE;

		std::cout << center_x << " " << center_y << " " << moving_x << " " << moving_y << "\n";

		if (center_x != moving_x) {
			diff_x = xVel * ( deltaTicks / 1000.f );
			if (diff_x == 0 && xVel < 0) { diff_x = -1; }
			else if (diff_x == 0 && xVel > 0) { diff_x = 1; }
			if (center_x > moving_x) {
				m_camera.x += diff_x;
				center_x = (m_camera.x + m_camera.w/2) / MAP_POINT_SIZE;
				if (center_x < moving_x) {
					m_camera.x = (moving_x - m_camera.w/2/MAP_POINT_SIZE) * MAP_POINT_SIZE;
				}
			}
			else if (center_x < moving_x) {
				m_camera.x += diff_x;
				center_x = (m_camera.x + m_camera.w/2) / MAP_POINT_SIZE;
				if (center_x > moving_x) {
					m_camera.x = (moving_x - m_camera.w/2/MAP_POINT_SIZE) * MAP_POINT_SIZE;
				}
			}
			else {
				m_camera.x += diff_x;
			}
		}

		if (center_y != moving_y) {
			diff_y = yVel * ( deltaTicks / 1000.f );
			if (diff_y == 0 && yVel < 0) { diff_y = -1; }
			else if (diff_y == 0 && yVel > 0) { diff_y = 1; }
			if (center_y > moving_y) {
				m_camera.y += diff_y;
				center_y = (m_camera.y + m_camera.h/2) / MAP_POINT_SIZE;
				if (center_y < moving_y) {
					m_camera.y = (moving_y - m_camera.h/2/MAP_POINT_SIZE) * MAP_POINT_SIZE;
				}
			}
			else if (center_y < moving_y) {
				m_camera.y += diff_y;
				center_y = (m_camera.y + m_camera.h/2) / MAP_POINT_SIZE;
				if (center_y > moving_y) {
					m_camera.y = (moving_y - m_camera.h/2/MAP_POINT_SIZE) * MAP_POINT_SIZE;
				}
			}
			else {
				m_camera.y += diff_y;
			}
		}

		if (center_x == moving_x && center_y == moving_y) {
// 			m_camera.x = (moving_x - MapArea::instance()->center_x) * 32;
// 			m_camera.y = (m_camera.y/32 - MapArea::instance()->getGlobalY()) * 32;
			m_xVel = 0;
			m_yVel = 0;
		}
	}

	VisibleObjectManager::instance()->move(-diff_x, -diff_y, deltaTicks);

	m_ship->move(-diff_x, -diff_y, deltaTicks);
}

void MapArea::render(Surface *screen) {
	m_area->blit(0, 0, screen, &m_camera);

	VisibleObjectManager::instance()->render(screen);

	m_ship->render(screen);

	std::stringstream caption;
	caption << "Mouse: [" << mouse_x << "," << mouse_y << "]";
	screen->renderText(0, 10, caption.str());

	caption.str("");
	caption << "Center: [" << ((m_camera.x + m_camera.w/2) / MAP_POINT_SIZE) << "," << ((m_camera.y + m_camera.h/2) / MAP_POINT_SIZE) << "]";
	screen->renderText(0, 20, caption.str());

	caption.str("");
	caption << "Moving: [" << moving_x << "," << moving_y << "]";
	screen->renderText(0, 30, caption.str());

	caption.str("");
	caption << "Angle: " << m_ship->getAngle();
	screen->renderText(0, 40, caption.str());

	caption.str("");
	caption << "New angle: " << m_ship->getNewAngle();
	screen->renderText(0, 50, caption.str());
}

