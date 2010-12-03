/*
 * ui_button.h
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

#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include <SDL/SDL.h>
#include "ui_callback.h"

typedef struct _UI_Button UI_Button;

UI_Button *ui_CreateButton(char *label);

void ui_SetButtonCallback(UI_Button *button, UI_Callback callback);
void ui_onButtonEvent(UI_Button *button);

void ui_PaintButton(UI_Button *button, SDL_Surface *canvas, int x, int y);

int ui_GetButtonWidth(UI_Button *);
int ui_GetButtonHeight(UI_Button *);
char *ui_GetButtonLabel(UI_Button *);
void ui_SetButtonLabel(UI_Button *, char *label);

#endif /* __UI_BUTTON_H__ */

