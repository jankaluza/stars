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

#include "WindowManager.h"
#include "Window.h"
#include "Base/Surface.h"
#include "Base/Timer.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "math.h"

WindowManager *WindowManager::m_instance = NULL;

WindowManager *WindowManager::instance() {
	if (WindowManager::m_instance == NULL) {
		WindowManager::m_instance = new WindowManager();
	}

	return WindowManager::m_instance;
}

WindowManager::WindowManager() {
}

WindowManager::~WindowManager() {
}

void WindowManager::addWindow(Window *window) {
	m_windows.push_back(window);
}

void WindowManager::removeWindow(Window *object) {
	std::vector<Window *>::iterator i = std::remove(m_windows.begin(), m_windows.end(), object);
	m_windows.erase(i);
}

void WindowManager::render(Surface *screen) {
	for (std::vector<Window *>::iterator it = m_windows.begin(); it != m_windows.end(); it++) {
		Window *win = *it;
		win->render(screen);
	}
}

bool WindowManager::handleEvent(SDL_Event &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION) {
		int x = event.motion.x;
		int y = event.motion.y;

		for (std::vector<Window *>::iterator it = m_windows.begin(); it != m_windows.end(); it++) {
			Window *win = *it;
			if (x > win->getX() && x < win->getX() + win->getW() && y > win->getY() && y < win->getY() + win->getH()) {
				win->handleEvent(event);
				return true;
			}
		}
	}
	else if (event.type == SDL_KEYDOWN) {
		for (std::vector<Window *>::iterator it = m_windows.begin(); it != m_windows.end(); it++) {
			Window *win = *it;
			if (win->hasFocus()) {
				win->handleEvent(event);
				return true;
			}
		}
	}
	return false;
}

