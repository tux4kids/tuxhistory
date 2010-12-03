/*
 * ui_layouts.c
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

#include "ui_layouts.h"
#include <SDL/SDL.h>

static int Fx=0, Fy=0;

void layout_FSetPos(int x, int y) {
	Fx=x; Fy=y;
}

SDL_Rect layout_FGetPos() {
	SDL_Rect pos;
	pos.x = Fx;
	pos.y = Fy;
	return pos;
}
