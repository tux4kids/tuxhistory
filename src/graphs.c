/*
 * graphs.c
 *
 * Description: Graph functions 
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */
#include<stdio.h>
#include<stdlib.h>

#include "globals.h"
#include "graphs.h"
#include "players.h"
#include "llist.h"


static int gmaps_alloc(int xsize, int ysize, int maps);
static void gmaps_free(int xsize, int ysize, int maps);

//gmaps is a tree dimensional array og gnode's, each 
//gnode must be linked with their niehboors.
static int gmaps_alloc(int xsize, int ysize, int maps)
{
    int i,j;
    xsize++;
    ysize++;
    gmaps = (gnode ***)malloc(maps * sizeof(gnode **));
    if(gmaps[i] == NULL)
    {
        printf("Error: Allocation of objects faild!\n");
        return 1; 
    }

    for(i = 0; i < maps; i++)
    {
        gmaps[i] = (gnode **)malloc(xsize * sizeof(gnode *));
        if(gmaps[i] == NULL)
        {
            printf("Error: Allocation of objects faild!\n");
            return 1; 
        }
        for(j = 0; j < ysize; j++)
        {
            gmaps[i][j] = (gnode *)malloc(ysize * sizeof(gnode));
            if(gmaps[i][j] == NULL)
            {
                printf("Error: Allocation of objects faild!\n");
                return 1; 
            }
        }
    }
    return 0;
}

int create_gmaps(int players)
{
    int i,j,k,l;
    int count;
    th_point point;
    th_vector vector;
    
    players++;
    if(gmaps_alloc(x_tildes, y_tildes, players))
    {
        printf("Error: out of memory!\n");
        return 1;
    }

    printf("x_tiles: %d, y_tiles: %d\n", x_tildes, y_tildes);
    for(i = 0; i < players; i++)
    {
        count = 0;
        for(j = 0; j <= x_tildes; j++)
        {
            for(k = 0; k <= y_tildes; k++)
            {
                count++;
                gmaps[i][j][k].terrain = map[j][k].terrain;
                gmaps[i][j][k].object = NULL;
                gmaps[i][j][k].point.x = j;
                gmaps[i][j][k].point.y = k;
                if(i == 0)
                {
                    gmaps[i][j][k].visible = 1;
                    gmaps[i][j][k].explored = 1;
                }
                else
                {
                    gmaps[i][j][k].visible = 0;
                    gmaps[i][j][k].explored = 0;
                }

                for(l = 0; l < NUM_DIRS; l++)
                {
                    vector = get_vector(point, l);
                    if(vector.x != -2 && vector.y != -2)
                    {
                        gmaps[i][j][k].nodes[l] = &gmaps[0][j+vector.x][k+vector.y];
                    }
                    else
                    {
                        gmaps[i][j][k].nodes[l] = NULL;
                    }
                }
                gmaps[i][j][k].id = count;
            }
        }
    }

    return 0;
}

int update_gmaps(void)
{
    list_node *obj_node;

    obj_node = list_nodes;
    if(obj_node != NULL)
    {
        do{
            gmaps[0][obj_node->obj.x][obj_node->obj.y].object = obj_node;
            gmaps[0][obj_node->obj.x][obj_node->obj.y].object = obj_node;
            obj_node = obj_node->next;
        }while(obj_node != NULL);
    }

    return 0;
}

static void gmaps_free(int xsize, int ysize, int maps)
{
    int i, j;
    for(i = 0; i < maps; i++)
    {
        for(j = 0; j < xsize; j++)
        {
            FREE(gmaps[i][j]);
        }
        FREE(gmaps[i]);
    }
}

void clean_gmaps(int players)
{
   gmaps_free(x_tildes, y_tildes, players);
}


