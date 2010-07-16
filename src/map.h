/*
 * map.h
 *
 * Description: The main game loop for TuxHistory
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) (C) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

/*
 *  This file contains the map data structure 
 *  called th_map. th_map is a two dimensional 
 *  data structure, made bye with pointers.
 *  This data structure shoultn't be accesed
 *  directly.
 */
#ifndef MAP_H
#define MAP_H

#include "hashtable.h"
#include "globals.h"
#include "objects.h"


/*Global tuxhistory vars*/

// th_map is the main data strucutre
// th_map_tilde specifies the terrain
// of the isometric map

typedef struct th_map{
    int height; //Height of this tilde
    int terrain;
    //int obj_id;
}th_map;

struct hashtable *map_table_hash; //Values of Terrains and objects
int flag_map; // Map flag: is a map surface allocated? 
int x_tildes;
int y_tildes;

int map_xml(FILE *fp);

// generate_map() generate a map from the basic
// xml input to a visual atractive one and store 
// the image in SDL_Surface *map_image.
// NOTE: map_xml() need to be called before!
// Use free_map() at the end of the program.

int generate_map(void);
void free_map(void);

// th_obj represent each object in the field, in 
// each tilde there can be only one object.

void th_draw_map(void);


th_vector get_vector(th_point point, int iso_dir);

#endif
