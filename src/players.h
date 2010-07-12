#ifndef PLAYERS_H
#define PLAYERS_H

#include "graphs.h"

typedef struct players{
    char name[50];
    int player_num;
    int civ;
    int max_pop;
    gnode *pos;
}players;

int num_of_players;

players *player;

#endif
