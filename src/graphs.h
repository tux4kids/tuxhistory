/*
 * graphs.h
 *
 * Description: Graph functions 
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#ifndef GRAPHS_H
#define GRAPHS_H

#include "map.h"

{
    ISO_NW,
    ISO_N,
    ISO_NE,
    ISO_W,
    ISO_E,
    ISO_SW,
    ISO_S,
    ISO_SE
}

typedef struct gnode{
    int anchor_x, anchor_y; //Anchors in main map surface.
    int visible;
    struct gnode *nodes[8];
    th_obj *object;
    int terrain;
}gelement;

gnode ***gmaps;

int gmaps_alloc(void);

#define GRAPHS_H
