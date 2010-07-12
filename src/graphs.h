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

typedef struct gelement{
    int anchor_x, anchor_y; //Anchors in main map surface.
    th_obj *object;
    th_map *terrain;
}gelement;


typedef struct gnode{
    struct gnode *nodes[8];
    gelement *data;
}gnode;



#define GRAPHS_H
