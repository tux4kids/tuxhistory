#include "players.h"

static int players_alloc();
int init_players(num_players)
{
    num_of_players = num_players;
    if(players_alloc(num_of_players))
        return 1;
    return 0;
}

