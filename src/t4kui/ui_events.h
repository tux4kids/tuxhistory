/*
 * ui_events.h
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

#ifndef __UI_EVENTS_H__
#define __UI_EVENTS_H__

#include <SDL/SDL.h>
#include "ui_window.h"

typedef enum _UI_Event {
	UIE_CLICK
} UI_Click;

void uie_Register();
void uie_RegisterWindow(UI_Window *win);
int uie_EventManager(const SDL_Event *event);


#endif /* __UI_EVENTS_H__ */

