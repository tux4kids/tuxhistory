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
#include "ai.h"


int tuxrts_init(char *object_name, char *map_name, int players)
{
    float zoom;
    FILE *fp;
    SDL_Surface *tmp_surf;

    object_counter = 0;

    if(!init_players(players, 1))
    {
       printf("No players created!\n");
       DEBUGMSG(debug_game, "No players created!");
       return 0;
    }

    fp = LoadObj("objects");
    if(fp == NULL)
    {
        printf("File not found!\n");
        DEBUGMSG(debug_game, "File not found!");
        return 0;
    }
    printf("Object files in memory!\n");
    if(objects_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the objects description file.\n");
        return 0;
    }
    printf("Object file parsed.\n");
   
    fp = LoadMap(map_name);
    if(fp == NULL)
    {
        DEBUGMSG(debug_game, "File not found!");
        return 0;
    }
    printf("Map file in memory.\n");

    if(map_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the map file.\n");
        return 0;
    }
    printf("Map file parsed!\n");
    if(create_gmaps(players))
    {
        printf("Couldn't generate grpah mesh!\n");
        return 0;
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


    return 1;
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

    printf("Enter to rts_get_object\n");

    if(gmaps[player][coords.x][coords.y].visible != 1)
    {
        printf("No visible in (%d,%d).\n", coords.x, coords.y);
        return NULL;
    }

    obj_node = list_nodes;
    if(obj_node != NULL)
    {
        do{
            printf("Object: (%d,%d) = (%d,%d) and player %d = %d ?\n", obj_node->obj.x, 
                    obj_node->obj.y, coords.x, coords.y, obj_node->obj.player, player);
            if( obj_node->obj.x == coords.x &&
                obj_node->obj.y == coords.y &&
                obj_node->obj.player == player)
                return &obj_node->obj;
            else
                obj_node = obj_node->next;
        }while(obj_node != NULL);
        return NULL;
    }

    return 0;
}

static void rts_set_visible(int player, th_point point, int deph, int count)
{
    int l;
    if(count == deph)
        return;

    gmaps[player][point.x][point.y].explored = 1;
    gmaps[player][point.x][point.y].visible = 1;


    for(l = 0; l < NUM_DIRS; l++)
    {
        if(gmaps[player][point.x][point.y].nodes[l])
            rts_set_visible(player, gmaps[player][point.x][point.y].nodes[l]->point, deph, count+1);
    }
    return;
}

int rts_update_game(void)
{
    int i, j, player;
    list_node *obj_node;
    th_point point;

    // Update gmaps...
    for(player = 0; player <= num_of_players; player++)
    {
        for(i = 0; i <= x_tildes; i++)
        {
            for(j = 0; j <= y_tildes; j++)
            {
                gmaps[player][i][j].visible = 0;
                gmaps[player][i][j].object = NULL;
                gmaps[player][i][j].drawed = 0;
            }
        }
    }
    obj_node = list_nodes;
    if(obj_node != NULL)
    {
        do{
            point.x = obj_node->obj.x;
            point.y = obj_node->obj.y;
            rts_set_visible(obj_node->obj.player,
                            point,
                            obj_node->obj.vision_range,
                            0);
            gmaps[obj_node->obj.player][point.y][point.y].object = &(obj_node->obj);
            obj_node = obj_node->next;
        }while(obj_node != NULL);
    }
    return 0;
}

/*************** Change state functions ****************/


int rts_goto(th_obj *obj, th_point point)
{
    th_path *path;
    th_point source;
    if(!obj)
    {
        printf("rts_goto error: object invalid!\n");
        return 0;
    }
    printf("Chanche %s state: go from (%d,%d) to (%d,%d)\n", 
                obj->rname,
                obj->x,
                obj->y,
                point.x,
                point.y);
    
    source.x = obj->x;
    source.y = obj->y;

    if(!(path = ai_shortes_path(obj->player,obj->type,source, point)))
    {
        printf("No shortes path found or a error ocurred!\n");
        return 0;
    }
    
    obj->state.path = path;

    ai_modify_state(obj->player, obj, GOTO);

    //printf("Path found!\n");

    return 0;
}

int rts_build(th_obj *obj, int type, th_point point)
{
    return 1;
}

int rts_die(th_obj *obj)
{
    return 1;
}
int rts_create(th_obj *obj, int type)
{
    return 1;
}
// For the folowing functions: target can be NULL or point 
// can be -1,-1, but one must be valid. If all to are valid,
// it will give preference to target.

int rts_attack(th_obj *obj, th_obj *target, th_point point)
{
    return 1;
}
int rts_repair(th_obj *obj, th_obj *target, th_point point)
{
    return 1;
}
int rts_use(th_obj *obj, th_obj *target, th_point point)
{
    return 1;
}

