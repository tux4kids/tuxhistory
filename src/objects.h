#ifndef OBJECTS_H
#define OBJECTS_H

#include "globals.h"
#include "hashtable.h"

#define NUM_OBJECT_HASH 1000

enum{
    REC_NONE,
    REC_GOLD,
    REC_STONE,
    REC_WOOD,
    REC_FOOD,
    NUM_RESOURCES
};

enum{
    INACTIVE,
    BUILD,
    REPAIR,
    ATTACK,
    CREATE,
    GOTO,
    USE,
    DIE,
    STORE,
    CONSTRUCTION,
    GENERATE,
    NUM_OF_STATES
};

enum{
    FOREST,
    GOLD,
    STONE,
    BUILDING,
    UNIT,
    NUM_OF_TYPES
};

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

struct hashtable *obj_table_hash; //Strings to enums
struct hashtable *objects_hash; //Names to objects
char object_names[NUM_OBJECT_HASH][30];

th_obj *object;
int object_counter;

int objects_xml(FILE *fp);

#endif
