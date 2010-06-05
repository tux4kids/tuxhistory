/*
 *  This file contains the map data structure 
 *  called th_map. th_map is a two dimensional 
 *  data structure, made bye with pointers.
 *  This data structure shoultn't be accesed
 *  directly.
 */

enum{
    SEA,
    DEEP_SEA,
    DESERT,
    SNOW,
    GRASS,
    GRASS2,
    BEACH,
    ROCKS,
    MOUNTAIN
};

// th_map is the main data strucutre
// th_map_tilde specifies the terrain
// of the isometric map

typedef struct th_map{
    int height; //Height of this tilde
    int terrain;
}th_map;

//The map array.
th_map **map;
    
void th_draw_map(void);

//loads the XML map file.
void th_load(void);

