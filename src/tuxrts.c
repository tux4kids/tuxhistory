#include "tuxrts.h"
#include "loaders.h"
#include "graphs.h"

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

