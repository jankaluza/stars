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

#include "Timer.h"

#include "SDL/SDL.h"
#include <string>

Timer::Timer() : m_startTicks(0), m_started(false) {
}

void Timer::start() {
	m_started = true;
	m_startTicks = SDL_GetTicks();
}

void Timer::stop() {
	m_started = false;
}

int Timer::getTicks() {
	if (m_started) {
		return SDL_GetTicks() - m_startTicks;
	}

	return 0;
}

bool Timer::isStarted() {
    return m_started;
}
