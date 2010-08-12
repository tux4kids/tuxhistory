/*
 * panel.h
 *
 * Description: GUI panel for TuxHistory
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#ifndef PANEL_H
#define PANEL_H

#include "SDL.h"
#include "objects.h"

typedef struct th_panel{
    SDL_Rect panel_minimap;
    SDL_Rect panel_description;
    SDL_Rect panel_option[5];
    int panel_actions[5];
    int panel_actions_obj[5];
    SDL_Rect panel_dest;
    SDL_Rect panel_origin;
    SDL_Rect panel_header_dest;
    SDL_Rect panel_header_origin;
    SDL_Rect panel_game;
}th_panel;

th_panel panel;

int panel_init(void);
void panel_draw(th_obj *, int);
int panel_click(th_point *point, th_obj *obj);
int panel_rclick(th_point *point);

#endif
