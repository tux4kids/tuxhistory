
/* ai.c
 *
 * Description: AI mainy path finding functions for the game lives here
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#include "tuxhistory.h"
#include "globals.h"
#include "graphs.h"
#include "ai.h"
#include "tuxrts.h"
#include "bheap.h"

// Hueristic distance between to points
#define HDIST(x1, y1, x2, y2) ((x1<x2)?(x2-x1):(x1-x2)) + ((y1<y2)?(y2-y1):(y1-y2))

void ai_shortes_path(th_point source, th_point goal)
{
    int H, G, F;
    if( source.x >= 0 && source.x < x_tildes   &&
        source.y >= 0 && source.y < y_tildes   &&
        goal.x >= 0 && goal.x < x_tildes       &&
        goal.y >= 0 && goal.y < y_tildes       )
    {
        H = HDIST(source.x, source.y, goal.x, goal.y);
        printf("H Distance %d\n", H);
    }
    else
    {
        printf("Bad point references!\n");
    }
}

