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

#include "Surface.h"
#include "FontManager.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "SDL/SDL_gfxPrimitives.h"
#include <string>

Surface::Surface() : m_surface(NULL) {
}

Surface::Surface(SDL_Surface *surface) : m_surface(surface) {
}

Surface::~Surface() {
	if (m_surface) {
		SDL_FreeSurface(m_surface);
	}
}

Surface *Surface::createFromFile(const std::string &file) {
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	loadedImage = IMG_Load(file.c_str());

	if (loadedImage) {
		optimizedImage = SDL_DisplayFormat(loadedImage);
		SDL_FreeSurface(loadedImage);
		if(optimizedImage != NULL) {
			SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB(optimizedImage->format, 255, 0, 255));
		}
		else {
			return NULL;
		}
	}
	
	Surface *s = new Surface(optimizedImage);
	return s;
}

Surface *Surface::createScreen(int width, int height, int bpp, Uint32 flags) {
	SDL_Surface *screen = SDL_SetVideoMode(width, height, bpp, flags);

	if (!screen) {
		return NULL;
	}

	Surface *s = new Surface(screen);
	return s;
}

Surface *Surface::createFromText(TTF_Font *font, Uint32 r, Uint32 g, Uint32 b, const std::string &text) {
	SDL_Color textColor = {r, g, b};
	return new Surface(TTF_RenderText_Solid(font, text.c_str(), textColor));
}

Surface *Surface::createFromText(Uint32 r, Uint32 g, Uint32 b, const std::string &text) {
	return createFromText(FontManager::instance()->getDefaultFont(), r, g, b, text);
}

Surface *Surface::createRotated(Surface *surface, double angle, int smooth) {
	SDL_Surface *res = rotozoomSurface(surface->m_surface, angle, 1, smooth);
	SDL_SetColorKey(res, SDL_SRCCOLORKEY, SDL_MapRGB(res->format, 0, 0, 0));
	return new Surface(res);
}

Surface *Surface::create(int w, int h, int bpp) {
	SDL_Surface *image = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, w, h, bpp, 0xff000000, 0x00ff0000, 0x0000ff00, 0);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, SDL_MapRGB(image->format, 255, 0, 255));
	return new Surface(image);
}

void Surface::line(int x1, int y1, int x2, int y2, Uint32 r, Uint32 g, Uint32 b) {
	lineRGBA(m_surface, x1, y1, x2, y2, r, g, b, 255);
}

bool Surface::flip() {
	return SDL_Flip(m_surface) != -1;
}

int Surface::renderText(int x, int y, const std::string &text) {
	if (text.empty()) {
		return 0;
	}

	Surface *frame = Surface::createFromText(255, 255, 255, text);
	frame->blit(x, y, this);
	int w = frame->getW();
	delete frame;
	return w;
}

void Surface::renderCenteredText(int x, int y, const std::string &text) {
	if (text.empty()) {
		return;
	}
	Surface *frame = Surface::createFromText(255, 255, 255, text);
	frame->blit(x - frame->getW()/2, y, this);
	delete frame;
}

void Surface::blit(int x, int y, Surface* destination, SDL_Rect* clip) {
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(m_surface, clip, destination->m_surface, &offset );
}

void Surface::fill(SDL_Rect *dstrect, Uint32 color) {
	SDL_FillRect(m_surface, dstrect, color);
}

void Surface::fill(Uint32 color) {
	fill(&m_surface->clip_rect, color);
}

void Surface::fill(Uint32 r, Uint32 g, Uint32 b) {
	fill(SDL_MapRGB(m_surface->format, r, g, b));
}

void Surface::fill(SDL_Rect *dstrect, Uint32 r, Uint32 g, Uint32 b) {
	fill(dstrect, SDL_MapRGB(m_surface->format, r, g, b));
}

void Surface::fill(int x, int y, int w, int h, Uint32 r, Uint32 g, Uint32 b) {
	SDL_Rect rc;
	rc.x = x;
	rc.y = y;
	rc.w = w;
	rc.h = h;
	fill(&rc, SDL_MapRGB(m_surface->format, r, g, b));
}

void Surface::box(int x, int y, int w, int h, Uint32 r, Uint32 g, Uint32 b) {
	line(x, y, x + w, y, r, g, b);
	line(x, y, x, y + h, r, g, b);
	line(x + w, y, x + w, y + h, r, g, b);
	line(x, y + h, x + w, y + h, r, g, b);
}

void Surface::setColorKey(Uint32 r, Uint32 g, Uint32 b) {
	SDL_SetColorKey(m_surface, SDL_SRCCOLORKEY, SDL_MapRGB(m_surface->format, r, g, b));
}


