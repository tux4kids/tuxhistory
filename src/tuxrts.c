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


    if(!panel_init())
    {
        printf("Error loading GUI.\n");
        return 0;
    }
    return 1;
}

// Returns 1 if the tile is valid to use for a player, and unit
// and 0 if not.
int rts_valid_tile(int player, int unit, th_point coords)
{
    list_node *node;
    th_obj *obj_p;
    if(coords.x < 0 || coords.x > x_tildes)
        return 0;
    if(coords.y < 0 || coords.y > y_tildes)
        return 0;
    if(player < 0 || player > num_of_players)
        return 0;

    //unit_p = list_search(list_nodes, unit);
    
    if(gmaps[player][coords.x][coords.y].terrain == OCEAN)
    {
        return 0;
    }
    else if(gmaps[player][coords.x][coords.y].terrain == HIGHSEA)
    {
        return 0;
    }
    
    return 1; 
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
    list_node *node;
    th_path *path;
    th_point source;
    th_point tmp_point, extra_point;
    int l;
    int action;

    if(!obj)
    {
        printf("rts_goto error: object invalid!\n");
        return 0;
    }
    if(obj->type != UNIT)
    {
        return 0;
    }

    action = GOTO;

    node = list_nodes;
    do{
        if(node->obj.x == point.x && node->obj.y == point.y)
        {
            tmp_point.x = -1;
            tmp_point.y = -1;
            printf("A object is on the goal tile\n");

            extra_point.x = obj->x;
            extra_point.y = obj->y;
            tmp_point = point;
            point = ai_alternative_tile(extra_point, point); 
            if(tmp_point.x == -1 && tmp_point.y == -1)
                return 0;

            printf("Finding a new action!\n");
            if(node->obj.player != human_player)
            {
                if(node->obj.type == BUILDING || node->obj.type == UNIT) 
                {
                    obj->state.target_point = tmp_point;
                    obj->state.target_obj = &(node->obj);
                    action = ATTACK;
                }
                else if(node->obj.type == FOREST ||
                        node->obj.type == GOLD   ||
                        node->obj.type == STONE ) 
                {
                    obj->state.target_point = tmp_point;
                    obj->state.rec_point = tmp_point;
                    obj->state.rec_point_flag = 1;
                    obj->state.target_obj = &(node->obj);
                    action = USE;
                }
            } 
            else
            {
                if(node->obj.type == BUILDING && obj->name_enum == VILLAGER_MILKMAID) 
                {
                    if(node->obj.state.state == CONSTRUCTION)
                    {
                        obj->state.target_point = tmp_point;
                        obj->state.target_obj = &(node->obj);
                        obj->state.rec_point = tmp_point;
                        obj->state.rec_point_flag = 1;    
                        action = BUILD;
                    }
                    else if(node->obj.name_enum == VILLAGE_CENTER &&
                            obj->state.carrying > 0) 
                    {
                        obj->state.target_point = tmp_point;
                        obj->state.target_obj = &(node->obj);
                        action = STORE;
                    }
                    else if(node->obj.name_enum == FARM)
                    {
                        obj->state.target_point = tmp_point;
                        obj->state.target_obj = &(node->obj);
                        obj->state.rec_point = tmp_point;
                        obj->state.rec_point_flag = 1;    
                        action = USE;
                    }
                    else
                    {
                        obj->state.target_point = tmp_point;
                        obj->state.target_obj = &(node->obj);
                        action = REPAIR;
                    }
                }
            }
            break;
        }
        node = node->next;
    }while(node);

    printf("Change %s state: go from (%d,%d) to (%d,%d)\n", 
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
        return 1;
    }
    
    obj->state.path = path;

    ai_modify_state(obj->player, obj, action);

    printf("Path found!\n");

    return 1;
}

int rts_build(th_obj *obj, int type, th_point point)
{
    list_node *node;
    th_path *path;
    th_point source;
    th_point tmp_point;
    th_point extra_point;
    th_obj *obj_template;
    th_obj new_obj;
    char obj_name[50];
    int l;
    int action;

    if(type >= NUM_OBJECTS)
    {
        printf("I can't build this object, Object not in list!\n");
        return 0;
    }
    if(!obj)
    {
        printf("rts_goto error: object invalid!\n");
        return 0;
    }
    if(obj->name_enum != VILLAGER_MILKMAID)
    {
        return 0;
    }



    action = BUILD;

    node = list_nodes;
    do{
        if(node->obj.x == point.x && node->obj.y == point.y)
        {
            printf("A object is on the goal tile\n");
            return 0;
        }
        node = node->next;
    }while(node);

    
    extra_point.x = obj->x;
    extra_point.y = obj->y;
    tmp_point = point;
    point = ai_alternative_tile(extra_point, point); 
           
    if(tmp_point.x == -1 && tmp_point.y == -1)
        return 0;

    // Creating new object: Extract the object tempalte from object hash
    // modify this object and add it to the object list with CONSTRUCTION
    // state.
    printf("Searching for object template %s\n", object_names[type]);
    obj_template = hashtable_lookup(objects_hash, object_names[type]);
            
    if(!obj_template)
        return 0;

    printf("\nObject found, addint object to list.\n");

    new_obj = *obj_template;

    //if(obj->player <= num_of_players)
    //{
        if( player_vars[obj->player].wood - new_obj.cost[REC_WOOD] > 0 &&
            player_vars[obj->player].food - new_obj.cost[REC_FOOD] > 0 &&
            player_vars[obj->player].gold - new_obj.cost[REC_GOLD] > 0 &&
            player_vars[obj->player].stone - new_obj.cost[REC_STONE] > 0 )
        {
            player_vars[obj->player].wood = player_vars[obj->player].wood - new_obj.cost[REC_WOOD]; 
            player_vars[obj->player].food = player_vars[obj->player].food - new_obj.cost[REC_FOOD];
            player_vars[obj->player].gold = player_vars[obj->player].gold - new_obj.cost[REC_GOLD];
            player_vars[obj->player].stone = player_vars[obj->player].stone - new_obj.cost[REC_STONE];
        }
        else
        {
            printf("Not enoght resources! You need wood: %d food: %d gold: %d stone: %d\n",
                    new_obj.cost[REC_WOOD],
                    new_obj.cost[REC_FOOD],
                    new_obj.cost[REC_GOLD],
                    new_obj.cost[REC_STONE]);
            return 0;
        }
    //}

    new_obj.id = object_counter;
    new_obj.player = obj->player;
    new_obj.x = tmp_point.x;
    new_obj.y = tmp_point.y;
    ai_modify_state(obj->player, &new_obj, CONSTRUCTION);
    list_add(&list_nodes, new_obj);

    object_counter++;

    // Find the shortes path to a close tile, and chanche the object 
    // state to build
    
    source.x = obj->x;
    source.y = obj->y;
    if(!(path = ai_shortes_path(obj->player,obj->type, source, point)))
    {
        printf("No shortes path found or a error ocurred!\n");
        return 1;
    }

    node = list_nodes;
    do{
        if(node->obj.x == tmp_point.x && node->obj.y == tmp_point.y)
        {
             obj_template = &(node->obj);
             break;
        }
        node = node->next;
    }while(node);

    obj->state.target_point = point;
    obj->state.target_obj = obj_template;
    obj->state.path = path;

    ai_modify_state(obj->player, obj, action);

    return 1;
}

// Discover a technologie or create a unit.
int rts_generate(th_obj *obj, int type)
{
    th_obj *tmp_obj;

    tmp_obj = hashtable_lookup(objects_hash, object_names[type]);

    if( player_vars[obj->player].wood - tmp_obj->cost[REC_WOOD] > 0 &&
        player_vars[obj->player].food - tmp_obj->cost[REC_FOOD] > 0 &&
        player_vars[obj->player].gold - tmp_obj->cost[REC_GOLD] > 0 &&
        player_vars[obj->player].stone - tmp_obj->cost[REC_STONE] > 0 )
    {
        player_vars[obj->player].wood = player_vars[obj->player].wood - tmp_obj->cost[REC_WOOD]; 
        player_vars[obj->player].food = player_vars[obj->player].food - tmp_obj->cost[REC_FOOD];
        player_vars[obj->player].gold = player_vars[obj->player].gold - tmp_obj->cost[REC_GOLD];
        player_vars[obj->player].stone = player_vars[obj->player].stone - tmp_obj->cost[REC_STONE];
    }
    else
    {
        printf("Not enoght resources! You need wood: %d food: %d gold: %d stone: %d\n",
                    tmp_obj->cost[REC_WOOD],
                    tmp_obj->cost[REC_FOOD],
                    tmp_obj->cost[REC_GOLD],
                    tmp_obj->cost[REC_STONE]);
        return 0;
    }

    if(obj->state.generate_type != type)
    {
        obj->state.generate_type = type;
        obj->state.generate_flag = 0;
        obj->state.generate_compl = tmp_obj->live;
        obj->state.generate_count = 0;
    }
    else if(obj->state.generate_flag == 0)
    {
        obj->state.generate_type = type;
        obj->state.generate_flag = 0;
        obj->state.generate_compl = tmp_obj->live;
        obj->state.generate_count = 0;
    }
    obj->state.generate_flag++;
    ai_modify_state(obj->player, obj, GENERATE);
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

