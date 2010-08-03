#include "SDL.h"
#include "SDL_rotozoom.h"
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
    float zoom;
    FILE *fp;
    SDL_Surface *tmp_surf;

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

    zoom = (float)screen->w/(float)images[IMG_GUIBG_BYZANTINE]->w;

    //rotozoomSurface (SDL_Surface *src, double angle, double zoom, int smooth);
    tmp_surf = rotozoomSurface(images[IMG_GUIBG_BYZANTINE], 0, zoom, 1);

    if (tmp_surf == NULL)
    {
      fprintf(stderr,
              "\nError: Zoom of GUI Backgrund not possible\n");
      return 0;
    }

    SDL_FreeSurface(images[IMG_GUIBG_BYZANTINE]);
    images[IMG_GUIBG_BYZANTINE] = tmp_surf;


    return 0;
}

// Returns 1 if the tile is valid to use for a player, and unit
// and 0 if not.
int rts_valid_tile(int player, int unit, th_point coords)
{
    list_node *unit_p;
    th_obj *obj_p;
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

        obj_p = rts_get_object(player, coords);
        if(obj_p != NULL)
        {
            if( obj_p->type == FOREST   ||
                obj_p->type == GOLD     ||
                obj_p->type == STONE)
                return 0;
            else
                return 1;
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

th_obj *rts_get_object(int player, th_point coords)
{
    list_node *obj_node;

    if(gmaps[player][coords.x][coords.y].visible != 0)
        return NULL;

    obj_node = list_nodes;
    if(obj_node != NULL)
    {
        do{
            if( obj_node->obj.x == coords.x &&
                obj_node->obj.y == coords.y   )
                return &obj_node->obj;
            else
                obj_node = obj_node->next;
        }while(obj_node != NULL);
        return NULL;
    }

    return 0;
}

