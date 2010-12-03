/*
 * ui_window.h
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

#ifndef __UI_WINDOW_H__
#define __UI_WINDOW_H__

#include <SDL/SDL.h>
#include "ui_proxywidget.h"

typedef struct _UI_Window UI_Window;

void ui_onWindowEvent(UI_Window *win, int x, int y);

UI_Window *ui_CreateWindow(int x, int y, int width, int height);
void ui_DrawWindow(UI_Window *window, SDL_Surface *screen);
int ui_WindowContains(UI_Window *window, int x, int y);
void ui_DestroyWindow(UI_Window *window);

void ui_AddWidget(UI_Window *window, void *widget, WidgetType type);

#endif /* __UI_WINDOW_H__ */

