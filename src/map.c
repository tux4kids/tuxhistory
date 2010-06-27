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
#include "fileops.h"

#include<ctype.h>
#include<mxml.h>

#include "map.h"

int get_terrain_enum(char *);
void str_upper(char *string);

int map_xml(FILE *fp)
{
    int value;
    int x,y;
    char *text;
    mxml_node_t *tree;
    mxml_node_t *node;
    mxml_node_t *inode;
    mxml_node_t *jnode;

    tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);

    x = 0;
    y = 0;

    for(inode = mxmlFindElement(tree, tree, "row", 
                NULL, NULL, MXML_DESCEND);
            inode != NULL;
            inode = mxmlFindElement(inode, tree, "row",
                NULL, NULL, MXML_DESCEND))
    {
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

            y++;
        }
        x++;
        printf("\n");
    }

    str_upper(text);
    
    printf("%s\n",text);
    

    mxmlDelete(jnode);
    mxmlDelete(inode);
    mxmlDelete(node);

    fclose(fp);
}

// Returns the enum value for each terrain type. If the terrain
// type don't exists it returns -1

int get_terrain_enum(char *terrain_string)
{
    if(strcmp(terrain_string, "HIGHSEA"))
        return HIGHSEA;
    else if(strcmp(terrain_string, "TUNDRA"))
        return TUNDRA;
    else if(strcmp(terrain_string, "SWAMP"))
        return SWAMP;
    else if(strcmp(terrain_string, "UNEXPLORED"))
        return UNEXPLORED;
    else if(strcmp(terrain_string, "DESERT"))
        return DESERT;
    else if(strcmp(terrain_string, "GRASSLAND"))
        return GRASSLAND;
    else if(strcmp(terrain_string, "ARCTIC"))
        return ARCTIC;
    else if(strcmp(terrain_string, "OCEAN"))
        return OCEAN;
    else if(strcmp(terrain_string, "MARSH"))
        return MARSH;
    else if(strcmp(terrain_string, "SAVANNAH"))
        return SAVANNAH;
    else if(strcmp(terrain_string, "PLAINS"))
        return PLAINS;
    else if(strcmp(terrain_string, "PRAIRIE"))
        return PRAIRIE;
    else
        return -1;
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


