/*
 * ui_events.c
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

#include "ui_events.h"
#include "ui_window.h"
#include "util.h"
#include <SDL/SDL.h>

static LinkedList *reg_windows;

void uie_Register() {
	SDL_SetEventFilter(uie_EventManager);
	reg_windows = list_Create();
}

void uie_RegisterWindow(UI_Window *win) {
	list_AddNode(reg_windows, win);
}

static UI_Window *_uie_FindWindow(int x, int y) {
	Node *node = list_GetFirst(reg_windows);
	while(node!=NULL) {
		if(ui_WindowContains((UI_Window *)list_GetNodeValue(node), x, y))
			return (UI_Window *)list_GetNodeValue(node);
		node = list_Traverse(node);
	}
	return NULL;
}

int uie_EventManager(const SDL_Event *event) {
	switch(event->type) {
		case SDL_MOUSEBUTTONDOWN:
			if(event->button.button==SDL_BUTTON_LEFT) {
				UI_Window *win=_uie_FindWindow(event->button.x, event->button.y);
				if(win!=NULL)
					ui_onWindowEvent(win, event->button.x, event->button.y);
			}
			break;
	}
	return 1;
}
