/* ai.c
 *
 * Description: AI mainly path finding functions for the game lives here
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */
#include<stdio.h>

// Lua headers
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

//Tuxhistory headers
#include "tuxhistory.h"
#include "globals.h"
#include "graphs.h"
#include "ai.h"
#include "tuxrts.h"
#include "bheap.h"
#include "hashtable.h"
#include "objects.h"
#include "fileops.h"
#include "loaders.h"
#include "map.h"
#include "players.h"
#include "llist.h"

// Hueristic distance between to points
#define HDIST(x1, y1, x2, y2) (((x1<x2)?(x2-x1):(x1-x2) + ((y1<y2)?(y2-y1):(y1-y2)))*10)

int ai_init(int players)
{
    L = lua_open();
    if(!L)
    {
        printf("Error starting LUA!\n");
        return 0;
    }
    return 1;
}

void ai_free(void)
{
    lua_close(L);
}
// Returns 1 if the tile is valid to use for a player, and unit
// and 0 if not.
int ai_valid_tile(int player, int unit, th_point coords)
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
    
    //Is a object there?
    node = list_nodes;
    do{
        if(node->obj.x == coords.x && node->obj.y == coords.y)
        {
            return 0;
        }
        node = node->next;
    }while(node);
    return 1; 
}

th_path *ai_shortes_path(int player, int unit, th_point source, th_point goal)
{
    int i, a;
    int count;

    th_vector vector;
    th_point pt;
    th_point *solution;
    th_path *path;

    bheap *open;
    struct hashtable *closed;
    bheap_node *e;
    bheap_node *p;
    bheap_node *n;
    
    // Are the source and goal point valid?
    if( source.x >= 0 && source.x <= x_tildes   &&
        source.y >= 0 && source.y <= y_tildes   &&
        goal.x >= 0 && goal.x <= x_tildes       &&
        goal.y >= 0 && goal.y <= y_tildes       )
    {
        
        // TODO: Actual way to store binary heap nodes is a bad idea. We need make 
        //       te code to use the dinamic strucures in bheap_add.
        e = (bheap_node *)malloc(1000 *  x_tildes * y_tildes * sizeof(bheap_node));
        if(e == NULL)
            return NULL;

        count = 0;
        i = 0;

        // Creating open and closed lists
        open = bheap_init(x_tildes * y_tildes);
        closed = make_hashtable(hashtable_default_hash, x_tildes * y_tildes);

        // Defining the initial node 
        sprintf(e[count].id, "%03d%03d", source.x, source.y);
        //printf("====================== A* STARTING... =====================\n");
        //printf("Element id to store: %s\n", e[count].id);
        e[count].deph = 0;
        e[count].point = source;
        e[count].h = HDIST(e[count].point.x, e[count].point.y, goal.x, goal.y);
        e[count].g = 0;
        e[count].val = e[count].g + e[count].h;
        e[count].index = count;
        e[count].parent = NULL;
        
        // Insert the initial node to the open list
        if(!bheap_add(open, &e[count]))
        {
            printf("Coudn't add element to the open list!\n");
            return NULL;
        }
        //bheap_print(open);

        while(open->count >= 0)
        {
            //printf("********** New Loop Cycle\n");
            // Remove the lowest element in open list
            // and add it to the closed list
            n = bheap_del(open);
            if(n == NULL)
            {
                printf("Error deleting the priority element from open list!\n");
                return NULL;
            }
            //printf("Removed id: %s\n", n->id);
            //bheap_print(open);
            
            //printf("Element id to store in loop: %s, index: %d\n", n->id, n->index);

            if(!hashtable_add(closed, e[n->index].id, &e[n->index]))
            {
                printf("Error adding to hashtable!\n");
                return NULL;
            }
            //hashtable_iter(closed, hashtable_default_hash);
            

            //Is this element the goal?
            if(n->point.x == goal.x && n->point.y == goal.y)
            {
                printf("Solution deph is %d\n", n->deph);
                solution = (th_point *)malloc(n->deph * sizeof(th_point));
                if(!solution)
                    return NULL;
                path = (th_path *)malloc(sizeof(th_path));
                if(!path)
                    return NULL;

                i=0;

                while(n->parent)
                {
                    printf("(%d,%d)\n",n->point.x, n->point.y);
                    solution[i] = n->point;
                    n = n->parent;
                    i++;
                } 
                
                path->path = solution;
                path->size = i - 1;

                free_hashtable(closed);
                bheap_free(open);
                FREE(e);
                return path;
            }

            //printf("This element is not the goal!.. Trying...\n");

            //For each valid move for n
            for(a = 0; a < NUM_DIRS; a++)
            {
                vector = get_vector(n->point, a);
                if(vector.x != -2 && vector.y != -2)
                {
                    //printf("Vector is valid... \n");
                    //printf("For %d direction tile in (%d,%d) is valid?\n", a, n->point.x, n->point.y);

                    pt.x = vector.x + n->point.x;
                    pt.y = vector.y + n->point.y;
                    if(ai_valid_tile(player, unit, pt))
                    {

                        //printf("Adding direction %d to open list!\n", a);

                        //New valid element
                        count++;
                        e[count].deph = n->deph + 1;
                        e[count].point = pt;
                        memset(e[count].id, 0, 7);
                        sprintf(e[count].id, "%03d%03d", pt.x, pt.y);
                        e[count].index = count;
                        e[count].h = HDIST(e[count].point.x, e[count].point.y, goal.x, goal.y);
                        if( a == ISO_N || 
                            a == ISO_S ||
                            a == ISO_W ||
                            a == ISO_E)
                            e[count].g = n->g + 10; 
                        else
                            e[count].g = n->g + 14;
                        e[count].val = e[count].g + e[count].h; // F = G + H
                        e[count].parent = n;
                        //printf("Actual id: %s, H: %d G:%d F:%d Deph:%d\n", e[count].id, e[count].h,
                        //       e[count].g, e[count].val, e[count].deph);

                        //Is this element in closed list?
                        if((p = hashtable_lookup(closed, e[count].id)) != NULL)
                        {
                            //printf("P exists in cloded list!\n");
                            if(p->val > e[count].val)
                            {
                                if(!hashtable_remove(closed, p->id))
                                {
                                    printf("Error ocurred while trying to remove key in hashtable!\n");
                                    hashtable_iter(closed, hashtable_default_hash);
                                    return NULL;
                                }
                                //else
                                //{
                                    //printf("Removes OK, let's check integrity!\n");
                                    //hashtable_iter(closed, hashtable_default_hash);
                                //}
                                if(!bheap_add(open, p))
                                {
                                    printf("Error ocurred while adding a element to open list\n");
                                    return NULL;
                                }
                                //printf("Succesfully removed from closed list and added to open list\n");
                            }
                        }   
                        else
                        {
                            //printf("P doesn't exist in closed list!\n");
                            if(!bheap_add(open, &e[count]))
                            {
                                printf("Error ocurred while adding a new element to open list\n");
                                return NULL;
                            }
                        }
                        //bheap_print(open);
                    }
                }
            }
        }
        free_hashtable(closed);
        bheap_free(open);
        FREE(e);
    }
    else
    {
        printf("Bad point references : Origin(%d, %d) Dest(%d, %d)\n", source.x, source.y, goal.x, goal.y);
    }
}

void ai_free_path(th_path *path)
{
    FREE(path->path);
    FREE(path);
}

int ai_modify_state(int player, th_obj *object, int state)
{
    if(!object)
    {
        printf("Incorrect object!\n");
        return 0;
    }
    if(object->player == player || object->player == 0)
    {
        object->state.old_state = object->state.state;
        object->state.state = state;
        object->state.flag = 1;
        printf("State Modified from %d to %d!\n",object->state.old_state,object->state.state );
        return 1;
    }
    printf("Not a valid player to modify objects state!");
    return 0;
}
static int ai_kill_object(list_node *node)
{
    list_node *iter_node;
    iter_node = list_nodes;
    do{

        if(&(iter_node->obj) == node->obj.state.target_obj)
        {
            node->obj.state.target_obj = NULL;
            node->obj.state.target_point.x = 0;
            node->obj.state.target_point.y = 0;
        }
        iter_node = iter_node->next;
    }while(iter_node);

    list_remove(&node);
    printf("Exito removiendo\n");
    return 1;
}

static int ai_is_tile_free(th_point point)
{
    return 0;
}

static int ai_close_obj(th_obj *obj1, th_obj *obj2)
{
    int l;
    th_point point;
    th_vector vector;
    for(l = 0; l < NUM_DIRS; l++)
    {
        vector = get_iso_vector(l);
        point.x = vector.x;
        point.y = vector.y;
        if(obj1->x + point.x == obj2->x &&
           obj1->y + point.y == obj2->y)
        {
            return 1;
        }
    }
    return 0;
}

static th_point ai_find_closest_center(th_point point)
{
    th_point lowest;
    list_node *node;

    lowest.x = -1;
    lowest.y = -1;

    node = list_nodes;
    do{
        if(node->obj.name_enum == VILLAGE_CENTER)
        {
            if(lowest.x == -1 && lowest.y ==-1)
            {
                lowest.x = node->obj.x;
                lowest.y = node->obj.y;
            }
            else if( HDIST(point.x, point.y, node->obj.x, node->obj.y) < 
                HDIST(point.x, point.y, lowest.x, lowest.y))
            {
                lowest.x =  node->obj.x;
                lowest.y =  node->obj.y;
            }
        }
        node = node->next;
    }while(node);
    return lowest;
}
    
th_point ai_alternative_tile(th_point p1, th_point p2)
{
    int prev_hdist;
    int l;
    th_point tmp_point;

    prev_hdist = -1;
    for(l=0; l<NUM_DIRS; l++)
    {
        if(gmaps[human_player][p2.x][p2.y].nodes[l])
        {
            if(ai_valid_tile(human_player, 0, gmaps[human_player][p2.x][p2.y].nodes[l]->point))
            {
                if(prev_hdist == -1)
                {
                    tmp_point = gmaps[human_player][p2.x][p2.y].nodes[l]->point;
                    prev_hdist = HDIST(p1.x, p1.y, gmaps[human_player][p2.x][p2.y].nodes[l]->point.x,
                           gmaps[human_player][p2.x][p2.y].nodes[l]->point.y);
                }
                else if(HDIST(p1.x, p1.y, gmaps[human_player][p2.x][p2.y].nodes[l]->point.x,
                           gmaps[human_player][p2.x][p2.y].nodes[l]->point.y) < prev_hdist)
                {
                    tmp_point = gmaps[human_player][p2.x][p2.y].nodes[l]->point;
                    prev_hdist = HDIST(p1.x, p1.y, gmaps[human_player][p2.x][p2.y].nodes[l]->point.x,
                           gmaps[human_player][p2.x][p2.y].nodes[l]->point.y);
                }
            }
        }
    }
/*    if(prev_hdist == -1)
    {
       for(l=0; l<NUM_DIRS; l++)
       {
            tmp_point = ai_alternative_tile(gmaps[human_player][p2.x][p2.y].nodes[l]->point, p2);
            break;
       }
    }*/
    printf("Alternative point: (%d, %d)\n", tmp_point.x, tmp_point.y);
    return tmp_point;
}
// ai_state_update modifies the values of all objects if they change their
// state.

int ai_state_update(list_node *node)
{
    list_node *node_tmp;
    int tmp;
    int i;
    th_point tmp_point, point;
    if(!node)
        return 0;
    
    i = 0;
    
    do{
        i++;
        if(node->obj.state.flag)
        {
            printf("Enter to process state\n");
            if(node->obj.state.state == GOTO)
            {
                node->obj.state.path_count = node->obj.state.path->size;
                node->obj.state.count = 0;
                node->obj.state.flag = 0;
                node->obj.state.agains_flag = 0;
                node->obj.state.action_againts = 0;
                node->obj.state.path_flag = 1;
            }
            if(node->obj.state.state == ATTACK)
            {
                node->obj.state.path_count = node->obj.state.path->size;
                node->obj.state.count = 0;
                node->obj.state.flag = 0;
                node->obj.state.agains_flag = 0;
                node->obj.state.action_againts = 0;
                node->obj.state.path_flag = 1;
            }
            if(node->obj.state.state == USE)
            {
                node->obj.state.path_count = node->obj.state.path->size;
                node->obj.state.count = 0;
                node->obj.state.flag = 0;
                node->obj.state.agains_flag = 0;
                node->obj.state.action_againts = 0;
                node->obj.state.path_flag = 1;
            }
            if(node->obj.state.state == REPAIR)
            {
                node->obj.state.path_count = node->obj.state.path->size;
                node->obj.state.count = 0;
                node->obj.state.flag = 0;
                node->obj.state.agains_flag = 0;
                node->obj.state.action_againts = 0;
                node->obj.state.path_flag = 1;
            } 
            if(node->obj.state.state == STORE)
            {
                printf("Go to store resources!\n");
                node->obj.state.path_count = node->obj.state.path->size;
                node->obj.state.count = 0;
                node->obj.state.flag = 0;
                node->obj.state.agains_flag = 0;
                node->obj.state.action_againts = 0;
                node->obj.state.path_flag = 1;
            } 
            node->obj.state.flag = 0;
            if(node->obj.state.state == DIE)
            {
                node_tmp = node->next;
                ai_kill_object(node);
                node = node_tmp;
                continue;
            }
        }
        if(node->obj.state.agains_flag)
        {
        }
        if(node->obj.state.path_flag)
        {
            //printf("path flag is on!\n");
            node->obj.state.count++;
            if(node->obj.state.count > 10)
            {
                node->obj.state.count = 0;
                if(node->obj.state.path_count >= 0)
                {
                    node->obj.x = node->obj.state.path->path[node->obj.state.path_count].x;
                    node->obj.y = node->obj.state.path->path[node->obj.state.path_count].y;
                    printf("Modify path count %d -> (%d,%d)\n", node->obj.state.path_count,
                            node->obj.x, node->obj.y);
                    node->obj.state.path_count = node->obj.state.path_count - 1;
                }
                else
                {
                    if(node->obj.state.state == GOTO)
                    {
                        ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                        node->obj.state.path_flag = 0;
                    }
                    else
                    {
                        node->obj.state.path_flag = 0;
                        node->obj.state.count = 0;
                    }
                    //ai_free_path(node->obj.state.path);
                }
            }
        }
        else 
        {
            if(node->obj.state.state == ATTACK)
            {
                //printf("Attacking and not walking...\n");
                if(ai_close_obj(&(node->obj), node->obj.state.target_obj))
                {
                    node->obj.state.count++;
                    if(node->obj.state.count > 10)
                    {
                        node->obj.state.count = 0;
                        printf("%s attacking %s, %s live is %d\n", node->obj.rname,
                            node->obj.state.target_obj->rname, 
                            node->obj.state.target_obj->rname, 
                            node->obj.state.target_obj->actual_live);
                        node->obj.state.target_obj->state.action_againts = ATTACK;
                        node->obj.state.target_obj->state.agains_flag = 1;
                        tmp = node->obj.attack - node->obj.state.target_obj->defence;
                        if(tmp > 0)
                            node->obj.state.target_obj->live = node->obj.state.target_obj->actual_live - tmp;
                        else
                            node->obj.state.target_obj->live = node->obj.state.target_obj->actual_live - 1;
                    }
                }
                else
                {
                    ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                    node->obj.state.target_obj = NULL;
                    node->obj.state.count = 0;
                }
            }
            if(node->obj.state.state == USE)
            {
                //printf("Using and not walking...\n");
                if(ai_close_obj(&(node->obj), node->obj.state.target_obj))
                {
                    node->obj.state.count++;
                    if(node->obj.state.count > 10)
                    {
                        node->obj.state.count = 0;
                        node->obj.state.target_obj->state.action_againts = USE;
                        node->obj.state.target_obj->state.agains_flag = 1;
                        tmp = node->obj.attack;// - node->obj.state.target_obj->defence;
                        node->obj.state.target_obj->actual_live = node->obj.state.target_obj->actual_live - tmp;
                        node->obj.state.carrying = node->obj.state.carrying + tmp;
                        if(node->obj.state.target_obj->type == FOREST)
                            node->obj.state.resource_type = REC_WOOD;
                        if(node->obj.state.target_obj->type == GOLD)
                            node->obj.state.resource_type = REC_GOLD;
                        if(node->obj.state.target_obj->type == STONE)
                            node->obj.state.resource_type = REC_STONE;
                        if(node->obj.state.target_obj->name_enum == FARM)
                            node->obj.state.resource_type = REC_FOOD;

                        printf("%s using %s, %s live is %d\n", node->obj.rname,
                            node->obj.state.target_obj->rname, 
                            node->obj.state.target_obj->rname, 
                            node->obj.state.target_obj->actual_live);
                        if(node->obj.state.carrying > 50)
                        {
                            tmp_point.x = node->obj.x;
                            tmp_point.y = node->obj.y;
                            printf("Finish collecting at: (%d,%d)\n", tmp_point.x, tmp_point.y);
                            point = ai_find_closest_center(tmp_point);
                            if(point.x != -1 && point.y != -1)
                            {
                                printf("Closest center al (%d, %d)\n", point.x, point.y);
                                rts_goto(&(node->obj), point);
                                node = node->next;
                                continue;
                            }
                            else
                            {
                                printf("No town center in map!\n");
                                ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                            }
                        }
                    }
                }
                else
                {
                    ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                    node->obj.state.target_obj = NULL;
                    node->obj.state.count = 0;
                }
            }
            if(node->obj.state.state == STORE)
            {
                if(ai_close_obj(&(node->obj), node->obj.state.target_obj))
                {
                    if(node->obj.state.carrying > 0)
                    {
                        if(node->obj.state.resource_type == REC_WOOD)
                            player_vars[node->obj.player].wood += node->obj.state.carrying;
                        else if(node->obj.state.resource_type == REC_FOOD)
                            player_vars[node->obj.player].food += node->obj.state.carrying;
                        else if(node->obj.state.resource_type == REC_GOLD)
                            player_vars[node->obj.player].gold += node->obj.state.carrying;
                        else if(node->obj.state.resource_type == REC_STONE)
                            player_vars[node->obj.player].stone += node->obj.state.carrying;

                        printf("Player %d have now WOOD %d, FOOD %d, STONE %d, GOLD %d\n",
                                node->obj.player,
                                player_vars[node->obj.player].wood,
                                player_vars[node->obj.player].food,
                                player_vars[node->obj.player].stone,
                                player_vars[node->obj.player].gold);
                        
                        node->obj.state.carrying = 0;
                        node->obj.state.resource_type = REC_NONE;

                        if(node->obj.state.rec_point_flag)
                        {
                            rts_goto(&(node->obj), node->obj.state.rec_point);
                        }
                        else
                        {
                            node->obj.state.rec_point_flag = 0;
                            ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                        }
                    }
                    else
                    {
                            node->obj.state.rec_point_flag = 0;
                            ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                    }
                }
                else
                {
                    ai_modify_state(node->obj.player, &(node->obj), INACTIVE);
                    node->obj.state.target_obj = NULL;
                    node->obj.state.count = 0;
                }
            }
        }
        if(node->obj.actual_live <= 0)
            ai_modify_state(node->obj.player, &(node->obj), DIE);

        node = node->next;
    }while(node != NULL);
}

