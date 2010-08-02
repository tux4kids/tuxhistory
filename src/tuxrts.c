#include "tuxrts.h"
#include "globals.h"
#include "tuxhistory.h"
#include "fileops.h"
#include "loaders.h"
#include "map.h"
#include "players.h"
#include "graphs.h"
#include "llist.h"

int tuxrts_init(char *object_name, char *map_name, int players)
{
    FILE *fp;

    object_counter = 0;

    fp = LoadObj("objects");
    if(fp == NULL)
    {
        printf("File not found!\n");
        DEBUGMSG(debug_game, "File not found!");
        return 1;
    }
    printf("Object files in memory!\n");
    if(objects_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the objects description file.\n");
        return 1;
    }
    printf("Object file parsed.\n");
   
    fp = LoadMap(map_name);
    if(fp == NULL)
    {
        DEBUGMSG(debug_game, "File not found!");
        return 1;
    }
    printf("Map file in memory.\n");

    if(map_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the map file.\n");
        return 1;
    }
    printf("Map file parsed!\n");
    if(create_gmaps(players))
    {
        printf("Couldn't generate grpah mesh!\n");
        return 1;
    }
    generate_map();

    return 0;
}

// Returns 1 if the tile is valid to use for a player, and unit
// and 0 if not.
int rts_valid_tile(int player, int unit, th_point coords)
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
    /*else if(gmaps[player][coords.x][coords.y].object != NULL)
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
    }*/
    else
    {
        return 1;
    }
}

