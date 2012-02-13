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

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include <string>

class Surface {
	public:
		Surface();
		Surface(SDL_Surface *surface);
		~Surface();

		static Surface *createFromFile(const std::string &file);
		static Surface *createScreen(int width, int height, int bpp, Uint32 flags);
		static Surface *createFromText(TTF_Font *font, Uint32 r, Uint32 g, Uint32 b, const std::string &text);
		static Surface *createFromText(Uint32 r, Uint32 g, Uint32 b, const std::string &text);
		static Surface *createRotated(Surface *surface, double angle, int smooth = 1);
		static Surface *create(int width, int height, int bpp);

		int renderText(int x, int y, const std::string &text);
		void renderCenteredText(int x, int y, const std::string &text);

		bool flip();

		void blit(int x, int y, Surface *destination, SDL_Rect *clip = NULL);

		void fill(SDL_Rect *dstrect, Uint32 color);
		void fill(Uint32 color);
		void fill(Uint32 r, Uint32 g, Uint32 b);
		void fill(SDL_Rect *dstrect, Uint32 r, Uint32 g, Uint32 b);
		void fill(int x, int y, int w, int h, Uint32 r, Uint32 g, Uint32 b);

		void box(int x, int y, int w, int h, Uint32 r, Uint32 g, Uint32 b);

		void line(int x1, int y1, int x2, int y2, Uint32 r, Uint32 g, Uint32 b);

		void setColorKey(Uint32 r, Uint32 g, Uint32 b);

		int getW() {
			return m_surface->w;
		}

		int getH() {
			return m_surface->h;
		}

	private:
		SDL_Surface *m_surface;
};
