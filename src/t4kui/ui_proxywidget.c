/*
 * ui_proxywidget.c
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

#include "ui_proxywidget.h"
#include "ui_button.h"
#include <stdlib.h>

struct _UI_ProxyWidget {
	WidgetType type; /* contains type information about widget */
	void *widget; /* pointer to actual widget */
	int x, y;
};

int ui_WidgetContains(UI_ProxyWidget *widget, int x, int y) {
	//fprintf(stdout, "X:%d Y:%d W:%d H:%d / px: %d py: %d\n", widget->x, widget->y, widget->x + ui_GetWidgetWidth(widget), widget->y + ui_GetWidgetHeight(widget), x, y);
	return ((x > widget->x) &&
			(y > widget->y) &&
			(x < widget->x + ui_GetWidgetWidth(widget)) &&
			 (y < widget->y + ui_GetWidgetHeight(widget)))?1:0;
}

UI_ProxyWidget *ui_CreateProxyWidget(void *proxied, WidgetType type) {
	UI_ProxyWidget *widget = malloc(sizeof(UI_ProxyWidget));
	widget->widget = proxied;
	widget->type = type;
	widget->x=0; widget->y=0;
	return widget;
}

void *UI_DestroyProxyWidget(UI_ProxyWidget *widget) {
	void *proxied = widget->widget;
	free(widget);
	return proxied;
}

void ui_onWidgetEvent(UI_ProxyWidget *widget, int x, int y) {
	switch(widget->type) {
		case WT_BUTTON:
			ui_onButtonEvent(widget->widget);
	}
}

void ui_SetWidgetPosition(UI_ProxyWidget *widget, int x, int y) {
	widget->x = x;
	widget->y = y;
}

void ui_PaintWidget(UI_ProxyWidget *widget, SDL_Surface *canvas) {
	switch(widget->type) {
		case WT_BUTTON:
			ui_PaintButton(widget->widget, canvas, widget->x, widget->y);
			break;
	}
}

WidgetType ui_GetWidgetType(UI_ProxyWidget *widget) {
	return widget->type;
}

void *ui_GetWidget(UI_ProxyWidget *widget) {
	return widget->widget;
}

int ui_GetWidgetHeight(UI_ProxyWidget *widget) {
	switch(widget->type) {
		case WT_BUTTON:
			return ui_GetButtonHeight(widget->widget);
			break;
	}
}

int ui_GetWidgetWidth(UI_ProxyWidget *widget) {
	switch(widget->type) {
		case WT_BUTTON:
			return ui_GetButtonWidth(widget->widget);
			break;
	}
}

char *ui_GetWidgetLabel(UI_ProxyWidget *widget) {
	switch(widget->type) {
		case WT_BUTTON:
			return ui_GetButtonLabel(widget->widget);
			break;
	}
}

void ui_SetWidgetLabel(UI_ProxyWidget *widget, char *label) {
	switch(widget->type) {
		case WT_BUTTON:
			ui_SetButtonLabel(widget->widget, label);
			break;
	}
}

