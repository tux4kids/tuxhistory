#include "tuxrts.h"
#include "tuxhistory.h"
#include "loaders.h"
#include "maps.h"
#include "players.h"
#include "graphs.h"
#include "llist.h"
/*
int tuxrts_init(char *map_name, th_players *players)
{

    object_counter = 0;

    fp = LoadMap("map");
    if(fp == NULL)
    {
        DEBUGMSG(debug_game, "File not found!");
        return 1;
    }

    
    if(map_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the map file.\n");
        return 1;
    }

    if(create_gmaps(2))
    {
        return 1;
    }
    generate_map();
    return 0;
}
*/
// Returns 1 if the tile is valid to use for a player, and unit
// and 0 if not.
static int rts_valid_tile(int player, int unit, th_point coords)
{
    list_node *unit_p;
    if(coords.x < 0 || coords.x > x_tildes)
        return 0;
    if(coords.y < 0 || coords.y > y_tildes)
        return 0;
    if(player < 0 || player > num_of_players)
        return 0;

    unit_p = list_search(list_nodes, unit);
    
    if(gmaps[player][coords.x][coords.y].terrain == OCEAN)
    {
        return 0;
    }
    else if(gmaps[player][coords.x][coords.y].terrain == HIGHSEA)
    {
        return 0;
    }
    else if(gmaps[player][coords.x][coords.y].object != NULL)
    {
        if( gmaps[player][coords.x][coords.y].object->type == FOREST   ||
            gmaps[player][coords.x][coords.y].object->type == GOLD     ||
            gmaps[player][coords.x][coords.y].object->type == STONE ) 
        {
            return 0;
            // From to condition... Ships may use wather, 
            // pawns may une FOREST, GOLD, AND STONE
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

