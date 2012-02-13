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

#include "GameObject.h"
#include "Surface.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

GameObject::GameObject(Surface *surface, float speed) :
	m_surface(surface), m_x(0), m_y(0), m_speed(speed) {
}
/*
void Dot::handle_input()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= DOT_VEL; break;
            case SDLK_DOWN: yVel += DOT_VEL; break;
            case SDLK_LEFT: xVel -= DOT_VEL; break;
            case SDLK_RIGHT: xVel += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += DOT_VEL; break;
            case SDLK_DOWN: yVel -= DOT_VEL; break;
            case SDLK_LEFT: xVel += DOT_VEL; break;
            case SDLK_RIGHT: xVel -= DOT_VEL; break;
        }
    }
}

void Dot::move( Uint32 deltaTicks )
{
    //Move the dot left or right
    x += xVel * ( deltaTicks / 1000.f );

    //If the dot went too far to the left
    if( x < 0 )
    {
        //Move back
        x = 0;
    }
    //or the right
    else if( x + DOT_WIDTH > SCREEN_WIDTH )
    {
        //Move back
        x = SCREEN_WIDTH - DOT_WIDTH;
    }

    //Move the dot up or down
    y += yVel * ( deltaTicks / 1000.f );

    //If the dot went too far up
    if( y < 0 )
    {
        //Move back
        y = 0;
    }
    //or down
    else if( y + DOT_HEIGHT > SCREEN_HEIGHT )
    {
        //Move back
        y = SCREEN_HEIGHT - DOT_HEIGHT;
    }
}*/

void GameObject::show(Surface *screen) {
	m_surface->blit(m_x, m_y, screen);
}
