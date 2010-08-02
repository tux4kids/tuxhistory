#ifndef TUXRTS_H
#define TUXRTS_H

#include "tuxhistory.h"
#include "globals.h"
#include "map.h"
#include "players.h"
#include "llist.h"

/* tuxrts_mapinit(): Inizialize all map vars. This is
 * the fisrt function to call when we begin the game.
 * char *: Object file name, without .xml
 * char *: Map file name, without .xml
 * int   : Number of players*/

int tuxrts_init(char *, char *, int);
int rts_valid_tile(int, int, th_point);

void tuxrts_cleanup(void);

list_node *selected_node;

#endif
