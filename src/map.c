/*
 * map.c
 *
 * Description: Map functions for TuxHistory
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */


#include<ctype.h>
#include<mxml.h>
#include<assert.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_extras.h"
#include "SDL_rotozoom.h"

#include "tuxhistory.h"
#include "globals.h"
#include "fileops.h"
#include "objects.h"
#include "map.h"
#include "hashtable.h"
#include "llist.h"
#include "graphs.h"
#include "panel.h"
#include "players.h"

SDL_Surface* map_image;
SDL_Surface* mini_map_image;

static int  init_map_hash(void);
static void end_map_hash(void);
static int get_terrain_enum(char *);
static int *get_context_tildes(int, int);
static int *get_draw_tilde(int *, int);
static int get_tile_num(int, int);
static void str_upper(char *);
static Uint32 get_pcolor(SDL_Surface *surface, int x, int y);

static int init_map_hash(void)
{
    map_table_hash = make_hashtable(hashtable_default_hash, 30);

    if(map_table_hash == NULL)
        return 1;

    hashtable_add(map_table_hash, "FOREST_MIXED", FOREST_MIXED);
    hashtable_add(map_table_hash, "FOREST_TROPICAL", FOREST_TROPICAL);
    hashtable_add(map_table_hash, "FOREST_CONIFER", FOREST_CONIFER);
    hashtable_add(map_table_hash, "FOREST_SCRUB", FOREST_SCRUB);
    hashtable_add(map_table_hash, "FOREST_BOREAL", FOREST_BOREAL);
    hashtable_add(map_table_hash, "FOREST_WETLAND", FOREST_WETLAND);
    hashtable_add(map_table_hash, "FOREST_RAIN", FOREST_RAIN);
    hashtable_add(map_table_hash, "FOREST_BROADLEAF", FOREST_BROADLEAF);

    hashtable_add(map_table_hash, "HIGHSEA", HIGHSEA);
    hashtable_add(map_table_hash, "TUNDRA", TUNDRA);
    hashtable_add(map_table_hash, "SWAMP", SWAMP);
    hashtable_add(map_table_hash, "DESERT", DESERT);
    hashtable_add(map_table_hash, "GRASSLAND", GRASSLAND);
    hashtable_add(map_table_hash, "ARCTIC", ARCTIC);
    hashtable_add(map_table_hash, "OCEAN", OCEAN);
    hashtable_add(map_table_hash, "MARSH", MARSH);
    hashtable_add(map_table_hash, "SAVANNAH", SAVANNAH);
    hashtable_add(map_table_hash, "PLAINS", PLAINS);
    hashtable_add(map_table_hash, "PRAIRIE", PRAIRIE);

    return 0;
}

int map_xml(FILE *fp)
{
    int value;
    int x,y;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;
    mxml_node_t *jnode;

    th_obj *object_ptr;
    th_obj tmp_obj;

    char *tmp_text;

    object_ptr = NULL;
    list_nodes = NULL;

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    if(init_map_hash())
    {
        printf("Couldn't initalize init_map_hash()\n");
        return 1;
    }

    x = 0;
    y = 0;

    x_tildes = -1;
    y_tildes = -1;

    // Get the game vars
    
    node = mxmlFindElement(tree, tree, "message1", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            strcpy(thegame.message1, node->child->value.text.string);
        }
    }
    else
    {
        strcpy(thegame.message1, "");
    }

    node = mxmlFindElement(tree, tree, "message2", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            strcpy(thegame.message2, node->child->value.text.string);
        }
    }
    else
    {
        strcpy(thegame.message2, "");
    }

    node = mxmlFindElement(tree, tree, "message3", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            strcpy(thegame.message2, node->child->value.text.string);
        }
    }
    else
    {
        strcpy(thegame.message2, "");
    }

    node = mxmlFindElement(tree, tree, "players", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            value = atoi(node->child->value.text.string);
            thegame.players = value;
        }
    }
    else
    {
        thegame.players = 2;
    }

    node = mxmlFindElement(tree, tree, "population", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("YES",node->child->value.text.string)==0)
            {
                thegame.goal_population = 1;
            }
            else
            {
                thegame.goal_population = 0;
            }
        }
    }
    else
    {
        thegame.goal_population = 0;
    }

    node = mxmlFindElement(tree, tree, "conquest", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("YES",node->child->value.text.string)==0)
            {
                thegame.goal_conquest = 1;
            }
            else
            {
                thegame.goal_conquest = 0;
            }
        }
    }
    else
    {
        thegame.goal_conquest = 0;
    }

    node = mxmlFindElement(tree, tree, "goal_food", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("NO",node->child->value.text.string)==0)
            {
                thegame.goal_food = -1;
            }
            else
            {
                thegame.goal_food = atoi(node->child->value.text.string);
            }
        }
    }
    else
    {
        thegame.goal_food = -1;
    }

    node = mxmlFindElement(tree, tree, "goal_wood", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("NO",node->child->value.text.string)==0)
            {
                thegame.goal_wood = -1;
            }
            else
            {
                thegame.goal_wood = atoi(node->child->value.text.string);
            }
        }
    }
    else
    {
        thegame.goal_wood = -1;
    }
    
    node = mxmlFindElement(tree, tree, "goal_gold", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("NO",node->child->value.text.string)==0)
            {
                thegame.goal_gold = -1;
            }
            else
            {
                thegame.goal_gold = atoi(node->child->value.text.string);
            }
        }
    }
    else
    {
        thegame.goal_gold = -1;
    }

    node = mxmlFindElement(tree, tree, "goal_stone", 
                NULL, NULL, MXML_DESCEND);
    if(node)
    {
        if(node->child)
        {
            if(strcmp("NO",node->child->value.text.string)==0)
            {
                thegame.goal_stone = -1;
            }
            else
            {
                thegame.goal_stone = atoi(node->child->value.text.string);
            }
        }
    }
    else
    {
        thegame.goal_stone = -1;
    }

    for(inode = mxmlFindElement(tree, tree, "row", 
                NULL, NULL, MXML_DESCEND);
            inode != NULL;
            inode = mxmlFindElement(inode, tree, "row",
                NULL, NULL, MXML_DESCEND))
    {
        y = 0;
        for(jnode = mxmlFindElement(inode, inode, "tilde",
                    NULL, NULL, MXML_DESCEND);
                jnode != NULL;
                jnode = mxmlFindElement(jnode, inode, "tilde",
                    NULL, NULL, MXML_DESCEND))
        {
            // Get terrain value
            node = mxmlFindElement(jnode, jnode, "terrain",
                    NULL, NULL, MXML_DESCEND);
            
            if(node == NULL)
            {
                printf("Error: No terrain field...");
                return 0;
            }
            value = NULL;
            value = (int)hashtable_lookup(map_table_hash, node->child->value.text.string);
            if(value != NULL)
            {
                map[x][y].terrain = value;
                printf("%s",node->child->value.text.string);
            }

            node = mxmlFindElement(jnode, jnode, "height",
                    NULL, NULL, MXML_DESCEND);
            
            if(node == NULL)
            {
                printf("Error: field not found...");
                //return 0;
            }

            if(node->child->value.integer >= 0)
            {
                map[x][y].height = node->child->value.integer;
            }

            printf("%d ", map[x][y].terrain);

            // Get objects
            node = mxmlFindElement(jnode, jnode, "object",
                    NULL, NULL, MXML_DESCEND);
            if(node)
            {
                if(node->child != NULL)
                {
                    object_ptr = hashtable_lookup(objects_hash, node->child->value.text.string);
                    if(object_ptr != NULL)
                    {

                        printf("(%s", node->child->value.text.string);
                    
                        printf(" *%s ", object_ptr->description);
                        tmp_obj = *object_ptr;

                        if(tmp_text = mxmlElementGetAttr(node, "player"))
                        {
                            tmp_obj.player = atoi(tmp_text);
                            printf(" PLAYER: %03d ", tmp_obj.player);
                        }
                        else
                        {
                            tmp_obj.player = 0;
                            printf(" NO PLAYER ");
                        }

                        tmp_obj.id = object_counter;
                        tmp_obj.x = x;
                        tmp_obj.y = y;
                        list_add(&list_nodes, tmp_obj);
                        if(tmp_obj.type == UNIT)
                        {
                            player_vars[tmp_obj.player].pop++;
                        }
                        if(tmp_obj.name_enum == HOUSE)
                        {
                            player_vars[tmp_obj.player].limit_pop += 4;
                        }

                        object_counter++;
                    }
                    else
                    {
                        printf("Wrong object name\n");
                    }
                
                    value=(int)hashtable_lookup(map_table_hash, node->child->value.text.string);
                    if(value!=-1)
                    {
                        printf(" Hash object: %d) ", value);
                    }
                }
            }
            //node = NULL;

            y++;
        }
        if(y_tildes == -1)
        {
            y_tildes = y - 1;
        }
        else
        {
            if((y - 1) != y_tildes)
            {
                printf("\nBad map file...\n");
                return 1;
            }
        }
        x++;
        printf("\n");
    }

    if(x_tildes == -1)
    {
        x_tildes = x - 1;
    }

    if(x_tildes != y_tildes)
    {
        printf("Map file must be a perfect squer");
        return 1;
    }
    
    list_print(list_nodes);
    free_hashtable(map_table_hash);

    mxmlDelete(jnode);
    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);

    return 0;
}

/*  ---------------------------
 * |        |         |        |
 * |     1  |    2    |  3     |
 * |     NW |    N    | NE     |
 *  --------+---------+-------- 
 * |        | NW N NE |        |
 * |   4 W  |  W 0 E  | E 5    |
 * |        | SW S SE |        |
 *  --------+---------+-------- 
 * |   6 SW |    S    | SE 8   |
 * |        |    7    |        |
 * |        |         |        |
 *  --------------------------- 
 */

//Return array has 9 elements
static int *get_context_tildes(int x, int y)
{
    int *a;

    a = (int *)malloc(9*sizeof(int));
    if( a == NULL )
    {
        printf("get_context_tildes: Error trying to allocate memory!");
        return NULL;
    }


    *a = map[x][y].terrain;
    if(y <= 0)
    {
        *(a + 1) = -1;
        *(a + 2) = -1;
        *(a + 3) = -1;
        if(x <= 0)
        {
            *(a + 4) = -1;
            *(a + 6) = -1;
            *(a + 5) = map[x+1][y].terrain;
            *(a + 7) = map[x+1][y].terrain;
            *(a + 8) = map[x+1][y+1].terrain;
 
        }
        else if(x >= x_tildes)
        {
            *(a + 5) = -1;
            *(a + 8) = -1;
            *(a + 4) = map[x-1][y].terrain;
            *(a + 6) = map[x-1][y+1].terrain;
            *(a + 7) = map[x][y+1].terrain;
         }
        else
        {
            *(a + 4) = map[x-1][y].terrain;
            *(a + 5) = map[x+1][y].terrain;
            *(a + 6) = map[x-1][y+1].terrain;
            *(a + 8) = map[x+1][y+1].terrain;
         }
    }
    else if(y >= y_tildes)
    {
        *(a + 6) = -1;
        *(a + 7) = -1;
        *(a + 8) = -1;
        if(x <= 0)
        {
            *(a + 1) = -1;
            *(a + 4) = -1;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 3) = map[x+1][y-1].terrain;
            *(a + 5) = map[x+1][y].terrain;
         }
        else if(x >= x_tildes)
        {
            *(a + 3) = -1;
            *(a + 5) = -1;
            *(a + 1) = map[x-1][y-1].terrain;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 4) = map[x-1][y].terrain;
        }
        else
        {
            *(a + 1) = map[x-1][y-1].terrain;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 3) = map[x+1][y-1].terrain;
            *(a + 4) = map[x-1][y].terrain;
            *(a + 5) = map[x+1][y].terrain;
         }
    }
    else
    {
        if(x <= 0)
        {
            *(a + 1) = -1;
            *(a + 4) = -1;
            *(a + 6) = -1;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 3) = map[x+1][y-1].terrain;
            *(a + 5) = map[x+1][y].terrain;
            *(a + 7) = map[x][y+1].terrain;
            *(a + 8) = map[x+1][y+1].terrain;
         }
        else if(x >= x_tildes)
        {
            *(a + 3) = -1;
            *(a + 5) = -1;
            *(a + 8) = -1;
            *(a + 1) = map[x-1][y-1].terrain;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 4) = map[x-1][y].terrain;
            *(a + 6) = map[x-1][y+1].terrain;
            *(a + 7) = map[x][y+1].terrain;
         }
        else
        {
            *(a + 1) = map[x-1][y-1].terrain;
            *(a + 2) = map[x][y-1].terrain;
            *(a + 3) = map[x+1][y-1].terrain;
            *(a + 4) = map[x-1][y].terrain;
            *(a + 5) = map[x+1][y].terrain;
            *(a + 6) = map[x-1][y+1].terrain;
            *(a + 7) = map[x][y+1].terrain;
            *(a + 8) = map[x+1][y+1].terrain;
        }
    }
    return a; 
}        

// Returns a th_vector that contains a vector
// representing the tile walking of a isometric
// direction
th_vector get_iso_vector(int dir)
{
    th_vector vector;
    if(dir == ISO_NW)
    {
        vector.x = -1;
        vector.y = -1;
    }
    else if(dir == ISO_N)
    {
        vector.x = 0;
        vector.y = -1;
    }
    else if(dir == ISO_NE)
    {
        vector.x = 1;
        vector.y = -1;
    }
    else if(dir == ISO_W)
    {
        vector.x = -1;
        vector.y = 0;
    }
    else if(dir == ISO_E)
    {
        vector.x = 1;
        vector.y = 0;
    }
    else if(dir == ISO_SW)
    {
        vector.x = -1;
        vector.y = 1;
    }
    else if(dir == ISO_S)
    {
        vector.x = 0;
        vector.y = 1;
    }
    else if(dir == ISO_SE)
    {
        vector.x = 1;
        vector.y = 1;
    }
    else
    {
        vector.x = -2;
        vector.y = -2;
    }
    return vector;
}

//Uses th_map and returns a vector datatype that gives
//the move if posible. -2 if the move is not possible
//in this direction, 1,0, and -1 are valid moves.

th_vector get_vector(th_point point, int iso_dir)
{
    th_vector vector;

    vector = get_iso_vector(iso_dir);
    point.x = point.x + vector.x;
    point.y = point.y + vector.y;

    if (point.x < 0 || 
        point.x > x_tildes)
    {
        vector.x = -2;
        vector.y = -2;
    }
    if (point.y < 0 ||
        point.y > y_tildes)
    {
        vector.x = -2;
        vector.y = -2;
    }
    return vector;
}
        

// return a array with the terrain enum values to draw the map.
static int *get_draw_tilde(int *array, int oe)
{
    int *a;
    int i,j;

    if(!array)
    {
        return NULL;
    }

    // Allocate a as a dinamic array, please free it using FREE()
    a = (int *)malloc(9*sizeof(int));
    if( a == NULL )
    {
        printf("get_context_tildes: Error trying to allocate memory!");
        return NULL;
    }

    // First we define the center tilde type, and define if it is
    // odd or even.
    if(*array == HIGHSEA)
    {
        *a = ((oe%2)==0) ? HIGHSEAS_CENTER_0 : HIGHSEAS_CENTER_1;
    }
    else if(*array == TUNDRA)
    {
        *a = ((oe%2)==0) ? TUNDRA_CENTER_0 : TUNDRA_CENTER_1;
    }
    else if(*array == SWAMP)
    {
        *a = ((oe%2)==0) ? SWAMP_CENTER_0 : SWAMP_CENTER_1;
    }
    else if(*array == UNEXPLORED)
    {
        *a = ((oe%2)==0) ? UNEXPLORED_CENTER_0 : UNEXPLORED_CENTER_1;
    }
    else if(*array == DESERT)
    {
        *a = ((oe%2)==0) ? DESERT_CENTER_0 : DESERT_CENTER_1;
    }
    else if(*array == GRASSLAND)
    {
        *a = ((oe%2)==0) ? GRASSLAND_CENTER_0 : GRASSLAND_CENTER_1;
    }
    else if(*array == ARCTIC)
    {
        *a = ((oe%2)==0) ? ARCTIC_CENTER_0 : ARCTIC_CENTER_1;
    }
    else if(*array == OCEAN)
    {
        *a = ((oe%2)==0) ? OCEAN_CENTER_0 : OCEAN_CENTER_1;
    }
    else if(*array == MARSH)
    {
        *a = ((oe%2)==0) ? MARSH_CENTER_0 : MARSH_CENTER_1;
    }
    else if(*array == SAVANNAH)
    {
        *a = ((oe%2)==0) ? SAVANNAH_CENTER_0 : SAVANNAH_CENTER_1;
    }
    else if(*array == PLAINS)
    {
        *a = ((oe%2)==0) ? PLAINS_CENTER_0 : PLAINS_CENTER_1;
    }
    else if(*array == PRAIRIE)
    {
        *a = ((oe%2)==0) ? PRAIRIE_CENTER_0 :  PRAIRIE_CENTER_1;
    }
    else
    {
        printf("Unknown: %d\n", *array);
        return NULL;

    }

    for(i = 1; i < 9; i++)
    {
        //printf("I: %d ", i);
        if(*(array+i) != -1)
        {
            if(*array != *(array+i))
            {
                j=get_tile_num(i,oe);
                //printf("J: %d ", i, j);
                if(j < 0)
                {
                    printf("Error parsing tiles\n");
                    return NULL;
                }
                *(a + i) = *(array + i) * (NUM_COMPTILDE) + j;
            }
            else
            {
                *(a + i) = -1;
            }

        }
        else
        {
            *(a + i) = -1;
        }
    }
    return a;
}

static int get_tile_num(int i, int k)
{
    int oe;
    if (i >= NUM_COMPTILDE)
        return -1;
    oe = k%2;
    if (i == 1) return (k) ? BODER_SE_ODD : BORDER_SE_EVEN;
    else if (i == 2) return (k) ? BORDER_S_ODD : BORDER_S_EVEN;
    else if (i == 3) return (k) ? BORDER_SW_ODD : BORDER_SW_EVEN;
    else if (i == 4) return (k) ? BORDER_E_ODD : BORDER_E_EVEN;
    else if (i == 5) return (k) ? BORDER_W_ODD : BORDER_W_EVEN;
    else if (i == 6) return (k) ? BORDER_NE_ODD : BORDER_NE_EVEN;
    else if (i == 7) return (k) ? BORDER_N_ODD : BORDER_N_EVEN;
    else if (i == 8) return (k) ? BORDER_NW_ODD : BORDER_NW_EVEN; 
    else return -1;
}

int generate_anchormap(void)
{
    int x, y;
    int i,j;
    int ii, jj;

    // Set mapping colors
    iso_colors[1] = get_pcolor(images[IMG_ISOMAPPER], 2, 2);
    iso_colors[2] = get_pcolor(images[IMG_ISOMAPPER], images[IMG_ISOMAPPER]->w-2, 2);
    iso_colors[3] = get_pcolor(images[IMG_ISOMAPPER], 2 , images[IMG_ISOMAPPER]->h-2);
    iso_colors[4] = get_pcolor(images[IMG_ISOMAPPER], images[IMG_ISOMAPPER]->w-2, images[IMG_ISOMAPPER]->h-2);

    x = (int)((map_image->w - terrain[0]->w/2) / terrain[0]->w) + 2;
    y = (int)(map_image->h / terrain[0]->h);

    printf("Grid size: %d %d\n", x, y);
    anchor_map = (th_point **)malloc(x * sizeof(th_point *));
    
    if(anchor_map == NULL)
    {
        printf("Error: out of memory!\n");
        return 1;
    }

    for(i=0;i<x;i++)
    {
        anchor_map[i] = (th_point *)malloc(y * sizeof(th_point));
        if(anchor_map[i] == NULL)
        {
            printf("Error: out of memoy!\n");
            return 1;
        }
    }
    for(i = 0; i < x; i++)
    {
        for(j = 0; j < y; j++)
        {
            anchor_map[i][j].x = -1;
            anchor_map[i][j].y = -1;
            //printf("X Point: %d ", terrain[0]->w*i);
            //printf("Y Point: %d\n", terrain[0]->h*j);
            for(ii = 0; ii <= x_tildes; ii++)
            {
                for(jj = 0; jj <= y_tildes; jj++)
                {
                    if( gmaps[0][ii][jj].anchor.x > (terrain[0]->w*i) - terrain[0]->w/2&&
                        gmaps[0][ii][jj].anchor.x < (terrain[0]->w*i + terrain[0]->w) - terrain[0]->w/2&&
                        gmaps[0][ii][jj].anchor.y > (terrain[0]->h*j) &&
                        gmaps[0][ii][jj].anchor.y < (terrain[0]->h*j + terrain[0]->h))
                    {
                        anchor_map[i][j].x = ii;
                        anchor_map[i][j].y = jj;
                        goto endineriter;
                    }
                }
            }
endineriter:
            if(anchor_map[i][j].x != -1 && anchor_map[i][j].y != -1)
                printf("%d", gmaps[0][i][j].terrain);
            else
                printf("-");
        }
        printf("\n");
    }
    return 0;
}

void free_anchormap(void)
{
    int i;
    if(!anchor_map)
        return;
    for(i = 0; i < (int)(map_image->w / terrain[0]->w); i++)
        FREE(anchor_map[i]);
    FREE(anchor_map);
}

int generate_map(void)
{
    SDL_Surface* orig = NULL;

    SDL_Rect dest;
    th_point anchor;
    int i, j, k, l;
    int oe;
    int x, y;
    int *context_array;
    int *img_enums;

    map_image = NULL;
    int w, h;

    //Create a SDL_Surface that contains the terrain._
    w = terrain[TUNDRA_CENTER_1]->w * (x_tildes + 1);
    h = terrain[TUNDRA_CENTER_1]->h * (y_tildes + 1);
    map_image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 
            32, rmask, gmask, bmask, amask); 

    if(map_image == NULL)
    {
        DEBUGMSG(debug_setup,"Couldn't create img_image\n");
        return 1;
    }



    // Prepare the variables...
    SDL_FillRect(map_image, NULL, SDL_MapRGB(map_image->format, 0, 0 ,0));

    dest.x = map_image->w/2-terrain[TUNDRA_CENTER_1]->w/2;
    dest.y = 0;

    //printf("[%d,%d]\n", x_tildes, y_tildes);

    x = dest.x;
    y = dest.y;
    k = 0;
    
    //This loop blits all tiles to map_image.
    for (i = 0; i <= x_tildes; i++)
    {
        oe = k + 1;
        for (j = 0; j <= y_tildes; j++)
        {
            k++;


            context_array = get_context_tildes(i,j);
            if (context_array == NULL)
            {
                return 1;
            }
            img_enums = get_draw_tilde(context_array, k);
            if(img_enums == NULL)
            {
                return 1;
            }
            
            //printf("ENUM: %d GRASSLAND: %d\n", *img_enums, GRASSLAND_CENTER_0);
            //Draw in the map buffer the resulting values
            for(l = 0; l < 1; l++)
            {
                if (*(img_enums+l)!=-1)
                {
                    SDL_BlitSurface(terrain[*(img_enums+l)], NULL, map_image, &dest);
                }
            }

            
            //Write values to gmaps
            anchor.x = dest.x + terrain[*img_enums]->w/2;
            anchor.y = dest.y + terrain[*img_enums]->h/2;


            //printf(" Tile dest in px: (%d,%d) from: (%d,%d) with anchori (%d,%d)\n", 
            //        dest.x, dest.y, i, j, anchor.x, anchor.y);

            gmaps[0][i][j].anchor = anchor;
            gmaps[0][i][j].rect.x = dest.x;
            gmaps[0][i][j].rect.y = dest.y;
            
            // TODO: This is better in graph.h
            if(map[i][j].terrain == HIGHSEA ||
               map[i][j].terrain == OCEAN)
            {
                gmaps[0][i][j].usable = 0;
            }
            else
            {
                gmaps[0][i][j].usable = 1;
            }

            gmaps[0][i][j].terrain = map[i][j].terrain;


            //Prepare te new coords for the next tile
            dest.x = dest.x - (terrain[*img_enums]->w/2);
            dest.y = dest.y + (terrain[*img_enums]->h/2);
            
            FREE(context_array);
            FREE(img_enums);

        }
        x = x + (terrain[TUNDRA_CENTER_1]->w/2);
        y = y + (terrain[TUNDRA_CENTER_1]->h/2);
        dest.x = x;
        dest.y = y;

        //k counter defines if the tilde is even or odd.
        //The next row must be diferent to this row.
        if((((oe%2)==0) && (((k+1)%2)==0)) || 
           (((oe%2)!=0) && (((k+1)%2)!=0))) 
            k++;
    }

    // Optimizing img_map
    orig = map_image;

    SDL_SetAlpha(orig, SDL_RLEACCEL, SDL_ALPHA_OPAQUE);
    map_image = SDL_DisplayFormat(orig); /* optimize the format */
    SDL_FreeSurface(orig);
    
    // Create a anchors map and allocates int **anchor_map
    generate_anchormap(); 


    // Generate mini map
    mini_map_image = rotozoomSurface(map_image, 0, 0.1, 1);
    if(mini_map_image == NULL)
    {
        printf("Error: No mini map!");
        return 1;
    }
    return 0;
}

void free_map(void)
{
    SDL_FreeSurface(map_image);
    free_anchormap();
}

// TODO: Segfault error still in this function!
void str_upper(char *string)
{
    char c;
    while(*string)
    {
        c = *string;
        *string = toupper(c);
        string++;
    }
}

static Uint32 get_pcolor(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[ ( y * surface->w ) + x ];
}

th_point mouse_map(th_point mouse_p, th_point screen_p)
{
    int i, j;
    int terr_e;
    Uint32 color;
    th_point *anchor_p;
    th_point Pmousemap;
    th_point Ptilemap;


    Pmousemap.x = (int)(mouse_p.x + screen_p.x + terrain[TUNDRA_CENTER_1]->w/2)/terrain[TUNDRA_CENTER_1]->w;
    Pmousemap.y = (int)(mouse_p.y + screen_p.y - panel.panel_header_dest.h)/terrain[TUNDRA_CENTER_1]->h;
    
    Ptilemap.x = (int)(mouse_p.x + screen_p.x + terrain[TUNDRA_CENTER_1]->w/2)%terrain[TUNDRA_CENTER_1]->w;
    Ptilemap.y = (int)(mouse_p.y + screen_p.y - panel.panel_header_dest.h)%terrain[TUNDRA_CENTER_1]->h;
    
    anchor_p = &anchor_map[Pmousemap.x][Pmousemap.y];
    if(anchor_p->x == -1 && anchor_p->y == -1)
    {
        Pmousemap.x = -1;
        Pmousemap.y = -1;
        return Pmousemap;
    }

    Pmousemap.x = anchor_p->x;
    Pmousemap.y = anchor_p->y; 

    color = get_pcolor(images[IMG_ISOMAPPER], Ptilemap.x, Ptilemap.y);

    // NW
    if(color == iso_colors[1])
    {
        Pmousemap.x--;
    }
    //NE
    if(color == iso_colors[2])
    {
        Pmousemap.y--;
    }
    //SW
    if(color == iso_colors[3])
    {
        Pmousemap.y++;
    }
    //SE
    if(color == iso_colors[4])
    {
        Pmousemap.x++;
    }
    //printf("Mouse Maping: %d, %d Terrain: %d Color: %d\n", Pmousemap.x, Pmousemap.y, terr_e, color);

    return Pmousemap;
}
