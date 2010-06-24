/*
 *  This file contains the map data structure 
 *  called th_map. th_map is a two dimensional 
 *  data structure, made bye with pointers.
 *  This data structure shoultn't be accesed
 *  directly.
 */

#define MAX_X_TILDES
#define MAX_Y_TILDES

// List of objects that can be used
// in create_object. 
// TODO: (maybe this should be in game.c
//          with int create_object(int type, int x, int y)
//          prototype...)
//       

enum{
    TREE,
    NUM_OF_OBJECTS
};

// th_map is the main data strucutre
// th_map_tilde specifies the terrain
// of the isometric map

typedef struct th_map{
    int height; //Height of this tilde
    int terrain;
    th_obj *obj;// Pointer to object
}th_map;

// th_obj represent each object in the field, in 
// each tilde there can be only one object.

typedef struct th_obj{
    int x, y; // (x,y) in the th_map array
    int type;
    int live; // 100 to 0 
    char *name;
    int defence;
    int attack;
    int move; 
}th_obj;

//The map array.
th_map map[MAX_X_TILDES][MAX_Y_TILDES];
    
void th_draw_map(void);

//Generates the th_map from a XML string.
//th_map** th_make_map(char *);

char* th_serialize_map(th_map **);

