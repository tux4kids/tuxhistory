#ifndef TUXRTS_H
#define TUXRTS_H

#include "tuxhistory.h"
#include "globals.h"
#include "map.h"
#include "players.h"
#include "llist.h"
#include "objects.h"

/* tuxrts_mapinit(): Inizialize all map vars. This is
 * the fisrt function to call when we begin the game.
 * char *: Object file name, without .xml
 * char *: Map file name, without .xml
 * int   : Number of players
 * */

int tuxrts_init(char *, char *, int);

/* rts_valid_tile(): Evaluates a tile coordiate
 * to determine if it is valid for a player.
 * return 1 if it is valid and 0 if not.
 * int      : Player number 
 * th_point : Coords to evaluate 
 */

int rts_valid_tile(int, int, th_point);

/* rts_get_object(): gets a objetc that is 
 * on a spesific coordinate. Returns a pointer to 
 * that object if it exists on the coords and NULL
 * if no object is on the coords.
 * int      : Player
 * th_point : The coords
 */
th_obj *rts_get_object(int, th_point); 

void tuxrts_cleanup(void);

list_node *selected_node;

#endif
