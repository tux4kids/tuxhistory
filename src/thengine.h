/* thengine.h
 *
 * Description: In this file you should find the
 *    TuxHistory Data Structures, also the TuxHistory 
 *    API to access all the main game logic. Don't 
 *    handnle the variables manualy, use the adecuate
 *    functions declared in this header. 
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2011
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#include "objects.h"
#include "map.h"
#include "graphs.h"

/************ The game Structure ***************/

// th_map: is the basic unit of a Map, it
// contains the terrain type, and the height
// of the terrain. It also contains a pointer
// to a building, or a resource that is on
// the tile. 

typedef struct th_map{
    int height;  // Options: -1,0,1,2,3
    int terrain;
}th_map;

// th_state: contains all the information needed
// to determine the actual state of a object.

typedef struct th_state{
    int state;
    int old_state;
    int count; // Counter to anime
    int flag; //Has a new state? 
    int action_againts;
    int agains_flag;
    th_path *path;
    int path_count;
    int path_flag; //Need pathfinding?
    int carrying;
    int resource_type;
    struct th_obj *target_obj;
    th_point target_point;
    th_point rec_point;
    int rec_point_flag;
    int generate_flag;
    int generate_type;
    int generate_compl;
    int generate_count;
}th_state;

// th_obj: contains the propieties of each
// object. Under object you can find Units,
// buildings and resources.

typedef struct th_obj{
    int id;
    int x, y; // (x,y) in the gmaps array
    int type; // using the enum NUM_OF_TYPES of map.h
    int live; 
    int actual_live;
    char name[30];
    int name_enum;
    char rname[50];
    char description[200];
    int vision_range;
    int defence;
    int attack;
    int move;
    int cost[NUM_RESOURCES];
    int player;
    struct th_state state;
}th_obj;

// gnode: is a structure that describes 
// tile world

typedef struct gnode{
    int id;
    th_point point;
    th_point rect;
    th_point anchor; //Anchors in main map surface.
    int visible;
    int explored;
    int drawed;
    int usable; 
    struct gnode *nodes[NUM_DIRS];
    int nodes_flag[NUM_DIRS];
    th_obj *object;
    int terrain;
}gnode;

// General game structure
// Use the generated variable to 
// handle the game.

typedef struct th_game{

    // Init variables
    char message1[100];
    char message2[100];
    char message3[100];
    int players;

    // Goal flags
    int goal_conquest;
    int goal_population;
    int goal_food;
    int goal_wood;
    int goal_stone;
    int goal_gold;
   
    // Global game variables
    SDL_Surface* map_image;
    SDL_Surface* mini_map_image;
    th_map **map;
    th_obj* object;
    int object_counter; 

    //gnode gmaps[player][x][y]
    gnode ***gmaps;
 
}th_game;

/************** End of game structures ******************/


