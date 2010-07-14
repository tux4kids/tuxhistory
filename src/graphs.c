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

//gmaps is a tree dimensional array og gnode's, each 
//gnode must be linked with their niehboors.
static int gmaps_alloc(int xsize, int ysize, int maps)
{
    int i,j;
    gmaps = (gnode ***)malloc(maps * sizeof(gnode **));
    if(map[i] == NULL)
    {
        printf("Error: Allocation of objects faild!\n");
        return 1; 
    }

    for(i = 0; i < maps; i++)
    {
        gmaps[i] = (gnode **)malloc(xsize * sizeof(gnode *));
        if(map[i] == NULL)
        {
            printf("Error: Allocation of objects faild!\n");
            return 1; 
        }
        for(j = 0; j < ysize; j++)
        {
            gmaps[i][j] = (gnode *)malloc(ysize * sizeof(gnode));
            if(map[i][j] == NULL)
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
    int i,j,k;
    
    players++;
    if(gmaps_alloc(x_tildes, y_tildes, players))
    {
        printf("Error: out of memory!\n");
        return 1;
    }

    for(i = 0; i < players; i++)
    {
        for(j = 0; j < x_tildes; j++)
        {
            for

    






void cleanup_gmaps(int maps, int xsize)
{
    int i,j;
    for(i = 0; i < maps; i++)
    {
        for(j = 0; j < xsize; j++)
        {
            FREE(gmaps[i][j]);
        }
    }
}





