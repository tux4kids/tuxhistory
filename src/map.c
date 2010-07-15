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

#include "tuxhistory.h"

#include<ctype.h>
#include<mxml.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_extras.h"

#include "globals.h"
#include "fileops.h"
#include "map.h"
#include "hashtable.h"
#include "llist.h"
#include "graphs.h"

SDL_Surface* map_image;

static int init_map_hash(void);
static void end_map_hash(void);
static int get_terrain_enum(char *);
static int *get_context_tildes(int, int);
static th_vector get_iso_vector(int dir);
static int *get_draw_tilde(int *, int);
static int get_tile_num(int, int);
static void str_upper(char *);

static int init_map_hash(void)
{
    th_obj object_types[NUM_OF_TYPES];

    map_table_hash = make_hashtable(hashtable_default_hash, 30);

    if(map_table_hash == NULL)
        return 1;

    object_types[FOREST].type = FOREST;
    object_types[FOREST].live = 30;
    object_types[FOREST].defence = 0;
    object_types[FOREST].attack = 0;
    object_types[FOREST].move = 0;

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
   
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    if(init_map_hash())
        return 1;

    x = 0;
    y = 0;

    x_tildes = -1;
    y_tildes = -1;

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
    
            //value = get_terrain_enum(node->child->value.text.string);

            value = hashtable_lookup(map_table_hash, node->child->value.text.string);
            if(value != -1)
            {
                map[x][y].terrain = value;
                printf("%s",node->child->value.text.string);
            }

            
             
            node = mxmlFindElement(jnode, jnode, "height",
                    NULL, NULL, MXML_DESCEND);
            
            //printf("%d",node->child->value.integer);

            if(node->child->value.integer >= 0)
            {
                map[x][y].height = node->child->value.integer;
            }

            printf("%d ", map[x][y].terrain);

            // Get objects
            node = mxmlFindElement(jnode, jnode, "object",
                    NULL, NULL, MXML_DESCEND);

            if(node->child != NULL)
            {
                if(node->child->value.text.string)
                    printf("(%s", node->child->value.text.string);
                value=hashtable_lookup(map_table_hash, node->child->value.text.string);
                if(value!=-1)
                    printf(" Hash object: %d) ", value);
            }

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
    int i;
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
static th_vector get_iso_vector(int dir)
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
    int i;
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
                //printf("NUM: %d, %d, %d\n", *(array + i), (NUM_COMPTILDE - 1), 
                //    *(array + i) * (NUM_COMPTILDE) + j);
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

int generate_map(void)
{
    SDL_Rect dest;
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

            printf(" (%d,%d) (%d,%d)\n", dest.x, dest.y, i, j);

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

            printf(".");
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
    return 0;
}

void free_map(void)
{
  SDL_FreeSurface(map_image);
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


