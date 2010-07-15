#ifndef TUXRTS_H
#define TUXRTS_H

#include "map.h"

/* tuxrts_mapinit(): Inizialize all map vars. This is
 * the fisrt function to call when we begin the game.*/

int tuxrts_init(char *map_name, th_player *players);

void tuxrts_cleanup(void);

#endif
