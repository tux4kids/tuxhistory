/*
 * map.c
 *
 * Description: The main game loop for TuxHistory
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

#include "fileops.h"
#include "map.h"

SDL_Surface* map_image;


int get_terrain_enum(char *);
void str_upper(char *string);

int map_xml(FILE *fp)
{
    int value;
    int x,y;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;
    mxml_node_t *jnode;
   
    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

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
            node = mxmlFindElement(jnode, jnode, "terrain",
                    NULL, NULL, MXML_DESCEND);

            value = get_terrain_enum(node->child->value.text.string);
            if(value != -1)
            {
                map[x][y].terrain = value;
            }

            printf("%s",node->child->value.text.string);
            
            node = mxmlFindElement(jnode, jnode, "height",
                    NULL, NULL, MXML_DESCEND);
            
            printf("%d",node->child->value.integer);

            if(node->child->value.integer >= 0)
            {
                map[x][y].height = node->child->value.integer;
            }

            printf("%d ", map[x][y].terrain);

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
    
    

    mxmlDelete(jnode);
    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);

    return 0;
}

// Returns the enum value for each terrain type. If the terrain
// type don't exists it returns -1

int get_terrain_enum(char *terrain_string)
{
    if(strcmp(terrain_string, "HIGHSEA") == 0)
        return HIGHSEA;
    else if(strcmp(terrain_string, "TUNDRA") == 0)
        return TUNDRA;
    else if(strcmp(terrain_string, "SWAMP") == 0)
        return SWAMP;
    else if(strcmp(terrain_string, "UNEXPLORED") == 0)
        return UNEXPLORED;
    else if(strcmp(terrain_string, "DESERT") == 0)
        return DESERT;
    else if(strcmp(terrain_string, "GRASSLAND") == 0)
        return GRASSLAND;
    else if(strcmp(terrain_string, "ARCTIC") == 0)
        return ARCTIC;
    else if(strcmp(terrain_string, "OCEAN") == 0)
        return OCEAN;
    else if(strcmp(terrain_string, "MARSH") == 0)
        return MARSH;
    else if(strcmp(terrain_string, "SAVANNAH") == 0)
        return SAVANNAH;
    else if(strcmp(terrain_string, "PLAINS") == 0)
        return PLAINS;
    else if(strcmp(terrain_string, "PRAIRIE") == 0)
        return PRAIRIE;
    else
        return -1;
}


int generate_map(void)
{
    SDL_Rect dest;
    int i,j;
    int x, y;

    map_image = NULL;
    int w, h;
    w = terrain[TUNDRA_CENTER_1]->w * x_tildes * 2;
    h = terrain[TUNDRA_CENTER_1]->h * y_tildes * 2;
    map_image = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 
            32, rmask, gmask, bmask, amask); 

    if(map_image == NULL)
    {
        DEBUGMSG(debug_setup,"Couldn't create img_image\n");
        return 1;
    }

    SDL_FillRect(map_image, NULL, SDL_MapRGB(map_image->format, 0, 0 ,0));

    dest.x = (map_image->w/2)-(terrain[TUNDRA_CENTER_1]->w/2);
    dest.y = terrain[TUNDRA_CENTER_1]->h/2;

    printf("[%d,%d]\n", x_tildes, y_tildes);

    x = dest.x;
    y = dest.y;

    for (i = 0; i <= x_tildes; i++)
    {
        for (j = 0; j <= y_tildes; j++)
        {
            printf(" (%d,%d) (%d,%d)\n", dest.x, dest.y, i, j);
            SDL_BlitSurface(terrain[TUNDRA_CENTER_1], NULL, map_image, &dest);
            dest.x = dest.x + (terrain[TUNDRA_CENTER_1]->w/2);
            dest.y = dest.y + (terrain[TUNDRA_CENTER_1]->h/2);
        }
        x = x - (terrain[TUNDRA_CENTER_1]->w/2);
        y = y + (terrain[TUNDRA_CENTER_1]->h/2);
        dest.x = x;
        dest.y = y;
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


