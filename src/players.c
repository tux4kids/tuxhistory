#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "players.h"
#include "graphs.h"

static int players_alloc(int players);

static int players_alloc(int players)
{
    player_vars = malloc((1 + players) * sizeof(th_players));
    if(player_vars == NULL)
    {
        printf("players_alloc: Couldn't allocate memory for palyer\n");
        return 1;
    }
    return 0;
}

int init_players(int players, int human)
{
    th_point pos;
    num_of_players = players;
    if(players_alloc(num_of_players))
    {
        return 0;
    }
    last_player = 0;

    //TODO: This information should be readen from a map
    
    pos.x = 0;
    pos.y = 0;
    add_player("Human", CIV_DEFAULT, 50, 200, 
                200, 200, 200, pos);
    add_player("Computer", CIV_DEFAULT, 50, 200, 
                200, 200, 200, pos);
    human_player = human;
    return 1;
}
int add_player(char *name, int civ, int max_pop, int stone, 
                int wood, int food, int gold, th_point pos)
{
    last_player++;
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
    (void) strcpy(player_vars[last_player].name, name);
    player_vars[last_player].civ = civ;
    player_vars[last_player].max_pop = max_pop;
    player_vars[last_player].stone = stone;
    player_vars[last_player].wood = wood;
    player_vars[last_player].food = food;
    player_vars[last_player].gold = gold;
    player_vars[last_player].pop = 0;
    player_vars[last_player].limit_pop = 0;
    player_vars[last_player].player_num = last_player;
    /*if(gmaps == NULL)
    {
        printf("add_player(): gmaps isn't allocated, cant giva a position in map to player!\n");
        return 1;
    }
    player[last_player].pos = &gmaps[0][pos.x][pos.y];
    */
    return 0;
}   


void clean_players(void)
{
    FREE(player_vars);
}
