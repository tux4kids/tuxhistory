#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "players.h"
#include "graph.h"

static int players_alloc(int players);

static int players_alloc(int players)
{
    player = malloc(players * sizeof(int));
    if(player == NULL)
    {
        printf("players_alloc: Couldn't allocate memory for palyer\n");
        return 1;
    }
    return 0;
}

int init_players(int players)
{
    num_of_players = players;
    if(players_alloc(num_of_players))
    {
        return 1;
    }
    last_player = -1;
    return 0;
}
int add_player(char *name, int civ, int max_pop, int stone, 
                int wood, int food, int gold, th_point pos)
{
    last_payer++;
    if(last_player > num_of_players)
    {
        printf("add_player(): player out of limit\n");
        return 1;
    }
    if(strlen(name) > NAME_LEN)
    {
        printf("add_player(): Players name too large, plese choose another");
        return 1;
    }
    (void) strcpy(player[last_player].name, name);
    player[last_player].civ = civ;
    player[last_player].max_pop = max_pop;
    player[last_player].stone = stone;
    player[last_player].wood = wood;
    player[last_player].food = food;
    player[last_player].gold = gold;
    player[last_player].pop = 0;
    player[last_player].player_num = last_player;
    if(gmap == NULL)
    {
        printf("add_player(): gmap isn't allocated, cant giva a position in map to player!\n");
        return 1;
    }
    player[last_player].pos = &gmap[0][pos.x][pos.y];

    return 0;
}   


void clean_players(void)
{
    FREE(player);
}
