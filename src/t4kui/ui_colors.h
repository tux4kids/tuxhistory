/*
 * ui_colors.h
 * This file is part of Tux4Kids
 *
 * Copyright (C) 2010 - Aviral Dasgupta
 *
 * Tux4Kids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tux4Kids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tux4Kids; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __UI_COLORS_H__
#define __UI_COLORS_H__

#include <SDL/SDL.h>

typedef enum _UI_Color {
	C_WHITE = 0xffffff,
	C_BLACK = 0x000000,
	C_WND = 0xd3d7cf,
	C_BTN = 0x555753
} UI_Color;

SDL_Color SDLC_WHITE;
SDL_Color SDLC_BLACK;
SDL_Color SDLC_BTN_BG;

#endif /* __UI_COLORS_H__ */

