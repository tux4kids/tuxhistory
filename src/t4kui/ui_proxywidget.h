/*
 * ui_proxywidget.h
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

#ifndef __UI_PROXYWIDGET_H__
#define __UI_PROXYWIDGET_H__

#include <SDL/SDL.h>

typedef struct _UI_ProxyWidget UI_ProxyWidget;

typedef enum _WidgetType {
	WT_BUTTON
} WidgetType;

UI_ProxyWidget *ui_CreateProxyWidget(void *widget, WidgetType type);
void *ui_DestroyProxyWidget(UI_ProxyWidget *widget);

void ui_onWidgetEvent(UI_ProxyWidget *widget, int x, int y);
int ui_WidgetContains(UI_ProxyWidget *widget, int x, int y);

void ui_SetWidgetPosition(UI_ProxyWidget *widget, int x, int y);

void ui_PaintWidget(UI_ProxyWidget *widget, SDL_Surface *canvas);

int ui_GetWidgetHeight(UI_ProxyWidget *widget);
int ui_GetWidgetWidth(UI_ProxyWidget *widget);

WidgetType ui_GetWidgetType(UI_ProxyWidget *widget);
void *ui_GetWidget(UI_ProxyWidget *widget);

char *ui_GetWidgetLabel(UI_ProxyWidget *widget);
void ui_SetWidgetLabel(UI_ProxyWidget *widget, char *label);

#endif /* __UI_PROXYWIDGET_H__ */

