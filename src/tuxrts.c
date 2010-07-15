#include "tuxrts.h"
#include "loaders.h"
#include "graphs.h"

int tuxrts_mapinit(char *map_name, th_players *players)
{
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

    generate_map();
    return 0;
}

