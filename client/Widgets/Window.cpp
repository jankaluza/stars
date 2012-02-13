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

#include "Window.h"
#include "Base/Surface.h"
#include "Widgets/Widget.h"
#include "Widgets/Button.h"
#include "Widgets/WindowManager.h"
#include "Widgets/Label.h"
#include "Widgets/LineEdit.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "math.h"

#include "tinyxml.h"

Window::Window(const std::string &caption, int x, int y, int width, int height) : Widget(x, y, width, height) {
	m_caption = caption;
	m_dragging = false;
	m_dragX = 0;
	m_dragY = 0;
}

Window::~Window() {
	for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
		Widget *widget = it->second;
		delete widget;
	}
}

void Window::createFromFile(Window *win, const std::string &f) {
	TiXmlDocument d(f);
	d.LoadFile();

	int ival;
	std::string sval;
	d.RootElement()->QueryValueAttribute("caption", &sval);
	win->setCaption(sval);
	d.RootElement()->QueryValueAttribute("w", &ival);
	win->setW(ival);
	d.RootElement()->QueryValueAttribute("h", &ival);
	win->setH(ival);

	Widget *widget = NULL;
	for (TiXmlNode *child = d.RootElement()->FirstChild(); child; child = child->NextSibling()) {
		TiXmlElement *el = child->ToElement();
		if (!el) {
			continue;
		}

		if (std::string(el->Value()) == "label") {
			el->QueryValueAttribute("text", &sval);
			widget = new Label(sval);
		}
		else if (std::string(el->Value()) == "lineedit") {
			el->QueryValueAttribute("name", &sval);
			widget = new LineEdit(sval);
		}
		else if (std::string(el->Value()) == "button") {
			el->QueryValueAttribute("text", &sval);
			widget = new Button(sval);
		}
		else {
			continue;
		}

		el->QueryValueAttribute("name", &sval);
		widget->setName(sval);

		el->QueryValueAttribute("x", &ival);
		widget->setX(ival);
		el->QueryValueAttribute("y", &ival);
		widget->setY(ival);

		el->QueryValueAttribute("w", &ival);
		widget->setW(ival);
		el->QueryValueAttribute("h", &ival);
		widget->setH(ival);

		win->addWidget(widget);
	}
}

Window *Window::createFromFile(const std::string &f) {
	Window *win = new Window("", 100, 100);
	createFromFile(win, f);
	return win;
}

void Window::addWidget(Widget *widget) {
	widget->setX(getX() + widget->getX());
	widget->setY(getY() + widget->getY());
	m_widgets[widget->getName()] = widget;
}

Widget *Window::getWidget(const std::string &name) {
	std::map<std::string, Widget *>::iterator it = m_widgets.find(name);
	if (it == m_widgets.end()) {
		return NULL;
	}
	return it->second;
}

void Window::clearFocus(Widget *except) {
		for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
			Widget *widget = it->second;
			if (widget != except) {
				widget->setFocus(false);
			}
		}
}

void Window::destroy() {
	WindowManager::instance()->removeWindow(this);
	delete this;
}

void Window::render(Surface *screen) {
	screen->fill(getX(), getY(), getW(), getH(), 92, 92, 103);
	renderFrame(screen);

	for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
		Widget *widget = it->second;
		widget->render(screen);
	}
}

void Window::renderFrame(Surface *screen) {
	screen->fill(getX(), getY(), getW(), 12, 151, 174, 196);
	screen->line(getX(), getY(), getX() + getW(), getY(), 255, 255, 255);
	screen->line(getX(), getY(), getX(), getY() + getH(), 255, 255, 255);
	screen->line(getX() + getW(), getY(), getX() + getW(), getY() + getH(), 255, 255, 255);
	screen->line(getX(), getY() + getH(), getX() + getW(), getY() + getH(), 255, 255, 255);

	screen->renderCenteredText(getX() + getW()/2, getY(), m_caption.c_str());
	screen->line(getX(), getY() + 12, getX() + getW(), getY() + 12, 255, 255, 255);
}

void Window::handleEvent(SDL_Event &event) {
	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION) {
		int x = event.motion.x;
		int y = event.motion.y;

		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)) {
			setFocus(true);
			if (m_dragging) {
				int x = event.motion.x;
				int y = event.motion.y;

				setX(getX() - (m_dragX - x));
				setY(getY() - (m_dragY - y));

				for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
					Widget *widget = it->second;
					widget->setX(widget->getX() - (m_dragX - x));
					widget->setY(widget->getY() - (m_dragY - y));
				}

				m_dragX = x;
				m_dragY = y;
			}

			if (x > getX() && x < getX() + getW() && y > getY() && y < getY() + 12) {
				m_dragging = true;
				m_dragX = x;
				m_dragY = y;
			}
		}
		else {
			if (m_dragging) {
				m_dragging = false;
			}
		}

		for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
			Widget *win = it->second;
			if (x > win->getX() && x < win->getX() + win->getW() && y > win->getY() && y < win->getY() + win->getH()) {
				win->handleEvent(event);
				if (win->hasFocus()) {
					clearFocus(win);
				}

				Button *button = dynamic_cast<Button *>(win);
				if (button) {
					if (button->isClicked()) {
						handleButtonClicked(button);
						button->setClicked(false);
					}
				}

				return;
			}
		}
	}
	else if (event.type == SDL_KEYDOWN) {
		setFocus(true);
		for (std::map<std::string, Widget *>::iterator it = m_widgets.begin(); it != m_widgets.end(); it++) {
			Widget *widget = it->second;
			if (widget->hasFocus()) {
				widget->handleEvent(event);
			}
		}
	}

}

