#ifndef OBJECTS_H
#define OBJECTS_H

#include "hashtable.h"

enum{
    FOREST,
    GOLD,
    STONE,
    BUILDING,
    UNIT,
    NUM_OF_TYPES
};


typedef struct th_obj{
    int id;
    int x, y; // (x,y) in the th_map array
    int type; // using the enum NUM_OF_TYPES of map.h
    int live; // 100 to 0 
    char name[30];
    int name_enum;
    char rname[50];
    char description[200];
    int defence;
    int attack;
    int move;
    int player;
}th_obj;

int object_counter;
struct hashtable *obj_table_hash; //Strings to enums
struct hashtable *objects_hash; //Names to objects

int objects_xml(FILE *fp);

#endif
