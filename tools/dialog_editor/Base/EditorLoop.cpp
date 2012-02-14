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
#include "Widgets/Button.h"
#include "Widgets/Label.h"
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

void EditorLoop::handleLineEditText(const std::string &name, const std::string &text) {
	if (name == "name") {
		currentWidget->setName(text);
	}
	else if (name == "button") {
		std::cout << "button\n";
		Button *b = new Button("", 20, 20, 70, 15);
		b->setName(text);
		mainWindow->addWidget(b);
	}
	else if (name == "text") {
		if (dynamic_cast<Button *>(currentWidget)) {
			dynamic_cast<Button *>(currentWidget)->setText(text);
		}
		else if (dynamic_cast<Label *>(currentWidget)) {
			dynamic_cast<Label *>(currentWidget)->setText(text);
		}
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

	mainWindow = Window::createFromFile(f);
	WindowManager::instance()->addWindow(mainWindow);

	currentWidget = NULL;
	int currentWidgetCounter = 0;
	std::string currentWidgetName;

	while (quit == false) {
		fps2.start();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			if (event.type == SDL_KEYDOWN) {
				int diff = 1;
				if (event.key.keysym.mod & KMOD_CAPS) {
					diff = 10;
				}
				if (event.key.keysym.sym == SDLK_TAB) {
					currentWidgetCounter = (currentWidgetCounter + 1) % (mainWindow->getWidgets().size() + 1);
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					if (event.key.keysym.mod & KMOD_SHIFT) {
						currentWidget->setW(currentWidget->getW() + diff);
					}
					else {
						currentWidget->setX(currentWidget->getX() + diff);
					}
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					if (event.key.keysym.mod & KMOD_SHIFT) {
						currentWidget->setW(currentWidget->getW() - diff);
					}
					else {
						currentWidget->setX(currentWidget->getX() - diff);
					}
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (event.key.keysym.mod & KMOD_SHIFT) {
						currentWidget->setH(currentWidget->getH() + diff);
					}
					else {
						currentWidget->setY(currentWidget->getY() + diff);
					}
				}
				else if (event.key.keysym.sym == SDLK_UP) {
					if (event.key.keysym.mod & KMOD_SHIFT) {
						currentWidget->setH(currentWidget->getH() - diff);
					}
					else {
						currentWidget->setY(currentWidget->getY() - diff);
					}
				}
				else if (event.key.keysym.sym == SDLK_F2) {
					LineEditWindow *win = new LineEditWindow("name", "Widget Name:");
					win->setHandler(this);
					WindowManager::instance()->addWindow(win);
				}
				else if (event.key.keysym.sym == SDLK_F3) {
					LineEditWindow *win = new LineEditWindow("text", "Text:");
					win->setHandler(this);
					WindowManager::instance()->addWindow(win);
				}
				else if (event.key.keysym.sym == SDLK_b && event.key.keysym.mod & KMOD_CTRL) {
					LineEditWindow *win = new LineEditWindow("button", "Widget Name:");
					win->setHandler(this);
					WindowManager::instance()->addWindow(win);
				}
			}

			if (WindowManager::instance()->handleEvent(event)) {
				continue;
			}
		}

		int i = 0;
		currentWidget = NULL;
		for (std::map<std::string, Widget *>::const_iterator it = mainWindow->getWidgets().begin(); it != mainWindow->getWidgets().end(); it++) {
			if (i++ == currentWidgetCounter) {
				currentWidget = it->second;
				currentWidgetName = it->first;
			}
		}

		if (currentWidget == NULL) {
			currentWidget = mainWindow;
			currentWidgetName = "MainWindow";
		}

		// Restart delta timer.
		delta.start();

		m_screen->fill(0, 0, 0);

		WindowManager::instance()->render(m_screen);

		m_screen->box(currentWidget->getX(), currentWidget->getY(), currentWidget->getW(), currentWidget->getH(), 255, 0, 0);

		frame++;

		// Show FPS
		std::stringstream caption;
		caption << "Widget: " << currentWidgetName;
		m_screen->renderText(0, 0, caption.str());

		update.start();

		m_screen->flip();

		if (loader.getTicks() > 1000) {
// 			mainWindow->destroy();
// 			mainWindow = Window::createFromFile(f);
// 			WindowManager::instance()->addWindow(mainWindow);
			mainWindow->saveToFile(f);
			loader.start();
		}

		if(( fps2.getTicks() < 1000 / FRAMES_PER_SECOND ) ) {
			SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps2.getTicks() );
		}
    }

}

