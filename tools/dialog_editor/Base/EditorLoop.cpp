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

#include "EditorLoop.h"
#include "Base/Surface.h"
#include "Base/Timer.h"
#include "Base/FontManager.h"
#include "Widgets/WindowManager.h"
#include "Windows/LoginWindow.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_net.h"
#include <string>
#include <sstream>
#include <iostream>

#define FRAMES_PER_SECOND 30

EditorLoop::EditorLoop() {
	m_initialized = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		m_initialized = false;
	}

	SDLNet_Init();

	TTF_Init();

	SDL_EnableUNICODE( SDL_ENABLE );

	m_screen = Surface::createScreen(640, 480, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("Stars", NULL);
}

EditorLoop::~EditorLoop() {
	if (m_screen) {
		delete m_screen;
	}

	SDLNet_Quit();

	TTF_Quit();

	if (m_initialized) {
		SDL_Quit();
	}
}

void EditorLoop::start(const std::string &f) {
	if (!m_initialized || !m_screen) {
		return;
	}

	bool quit = false;
	Timer delta;
	Timer update;
	Timer fps;
	Timer fps2;
	Timer loader;
	int frame = 0;

	delta.start();
	update.start();
	fps.start();
	loader.start();

	Window *mainWindow = Window::createFromFile(f);
	WindowManager::instance()->addWindow(mainWindow);

	while (quit == false) {
		fps2.start();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}

			if (WindowManager::instance()->handleEvent(event)) {
				continue;
			}
		}

		// Restart delta timer.
		delta.start();

		m_screen->fill(0, 0, 0);

		WindowManager::instance()->render(m_screen);

		frame++;

		// Show FPS
		std::stringstream caption;
		caption << "FPS: " << frame / ( fps.getTicks() / 1000.f );
		m_screen->renderText(0, 0, caption.str());

		update.start();

		m_screen->flip();

		if (loader.getTicks() > 1000) {
			mainWindow->destroy();
			mainWindow = Window::createFromFile(f);
			WindowManager::instance()->addWindow(mainWindow);
			loader.start();
		}

		if(( fps2.getTicks() < 1000 / FRAMES_PER_SECOND ) ) {
			SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps2.getTicks() );
		}
    }

}

