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

#pragma once

#include "Widget.h"
#include "SDL/SDL.h"
#include <string>
#include <vector>
#include <map>

class Surface;
class Widget;
class Button;

class Window : public Widget {
	public:
		Window(const std::string &caption = "", int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Window();

		static Window *createFromFile(const std::string &f);
		static void createFromFile(Window *win, const std::string &f);
		void saveToFile(const std::string &f);

		void addWidget(Widget *widget);
		Widget *getWidget(const std::string &name);

		const std::map<std::string, Widget *> &getWidgets() {
			return m_widgets;
		}

		void clearFocus(Widget *except = NULL);

		void destroy();

		void setCaption(const std::string &caption) {
			m_caption = caption;
		}

		virtual void setFocus(bool focus);

		virtual void render(Surface *screen);

		virtual void handleEvent(SDL_Event &event);

		virtual void handleButtonClicked(Button *button) {}

	private:
		void renderFrame(Surface *screen);

		std::string m_caption;
		bool m_dragging;
		int m_dragX;
		int m_dragY;
		std::map<std::string, Widget *> m_widgets;
};
