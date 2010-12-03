/*
 * ui_button.c
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

#include "ui_button.h"
#include "ui_colors.h"
#include "ui_system.h"
#include "ui_callback.h"
#include <SDL/SDL.h>

#include "../SDL_extras.h"

struct _UI_Button {
	char *label;
	SDL_Surface *prerendered;
	UI_Callback callback;
};

struct _UI_Button *ui_CreateButton(char *label) {
	struct _UI_Button *btn = malloc(sizeof(struct _UI_Button));
	ui_SetButtonLabel(btn, label);
	return btn;
}

void ui_PaintButton(struct _UI_Button *button, SDL_Surface *canvas, int x, int y) {
	SDL_Rect pos;
	pos.x = x; pos.y = y;
	SDL_BlitSurface(button->prerendered, NULL, canvas, &pos);
}

int ui_GetButtonWidth(struct _UI_Button *button) {
	return button->prerendered->w;
}

int ui_GetButtonHeight(struct _UI_Button *button) {
	return button->prerendered->h;
}

char *ui_GetButtonLabel(struct _UI_Button *button) {
	return button->label;
}

void ui_SetButtonLabel(struct _UI_Button *button, char *label) {
    SDL_Surface *tmpsurf;
	button->label = label;
    tmpsurf = SimpleText(label, FONT_SIZE, SDLC_WHITE);
    SDL_BlitSurface(tmpsurf, NULL, button->prerendered, NULL);
	//button->prerendered = TTF_RenderText_Shaded(ui_GetFont(), label, SDLC_WHITE, SDLC_BTN_BG);
}

void ui_SetButtonCallback(struct _UI_Button *button, UI_Callback callback) {
	button->callback = callback;
}

void ui_onButtonEvent(struct _UI_Button *button) {
	button->callback(button);
}


