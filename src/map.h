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

//#include "hashtable.h"

// List of objects that can be used
// in create_object. 
// TODO: (maybe this should be in game.c
//          with int create_object(int type, int x, int y)
//          prototype...)
//       

enum{
    FOREST,
    GOLD,
    STONE,
    BUILDING,
    UNI,
    NUM_OF_TYPES
};



/*Global tuxhistory vars*/
typedef struct {
    int id;
    int x, y; // (x,y) in the th_map array
    int type; // using the enum NUM_OF_TYPES of map.h
    int live; // 100 to 0 
    char name[30];
    int defence;
    int attack;
    int move;
    int player;
}th_obj;
// th_map is the main data strucutre
// th_map_tilde specifies the terrain
// of the isometric map

typedef struct {
    int height; //Height of this tilde
    int terrain;
}th_map;

struct hashtable *map_table_hash; //Values of Terrains and objects
list_node *list_nodes;
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

//Generates the th_map from a XML string.
//th_map** th_make_map(char *);

//char *th_serialize_map(th_map **);

#endif
