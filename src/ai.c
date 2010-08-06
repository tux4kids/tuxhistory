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

#include "tuxhistory.h"
#include "globals.h"
#include "graphs.h"
#include "ai.h"
#include "tuxrts.h"
#include "bheap.h"
#include "hashtable.h"

// Hueristic distance between to points
#define HDIST(x1, y1, x2, y2) (((x1<x2)?(x2-x1):(x1-x2) + ((y1<y2)?(y2-y1):(y1-y2)))*10)

/* itoa: thanks to Lukás Chmel */
static char* itoa(int value, char* result, int base)
{
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

th_point *ai_shortes_path(int player, int unit, th_point source, th_point goal)
{
    int i, a;
    int count;

    th_vector vector;
    th_point pt;
    th_point *solution;

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
        
        e = (bheap_node *)malloc(x_tildes * y_tildes * sizeof(bheap_node));
        if(e == NULL)
            return NULL;

        count = 0;
        i = 0;

        // Creating open and closed lists
        open = bheap_init(x_tildes * y_tildes);
        closed = make_hashtable(hashtable_default_hash, x_tildes * y_tildes);

        // Defining the initial node 
        sprintf(e[count].id, "%03d%03d", source.x, source.y);
        printf("====================== A* STARTING... =====================\n");
        printf("Element id to store: %s\n", e[count].id);
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
        bheap_print(open);

        while(open->count >= 0)
        {
            printf("********** New Loop Cycle\n");
            // Remove the lowest element in open list
            // and add it to the closed list
            n = bheap_del(open);
            if(n == NULL)
            {
                printf("Error deleting the priority element from open list!\n");
                return NULL;
            }
            printf("Removed id: %s\n", n->id);
            bheap_print(open);
            
            printf("Element id to store in loop: %s, index: %d\n", n->id, n->index);

            if(!hashtable_add(closed, e[n->index].id, &e[n->index]))
            {
                printf("Error adding to hashtable!\n");
                return NULL;
            }
            hashtable_iter(closed, hashtable_default_hash);
            

            //Is this element the goal?
            if(n->point.x == goal.x && n->point.y == goal.y)
            {
                printf("Solution deph is %d\n", n->deph);
                solution = (th_point *)malloc(n->deph * sizeof(th_point));
                i = 0;

                while(n->parent)
                {
                    printf("(%d,%d)\n",n->point.x, n->point.y);
                    solution[i] = n->point;
                    n = n->parent;
                    i++;
                } 
                return solution;
            }

            printf("This element is not the goal!.. Trying...\n");

            //For each valid move for n
            for(a = 0; a < NUM_DIRS; a++)
            {
                vector = get_vector(n->point, a);
                if(vector.x != -2 && vector.y != -2)
                {
                    printf("Vector is valid... \n");
                    printf("For %d direction tile in (%d,%d) is valid?\n", a, n->point.x, n->point.y);

                    pt.x = vector.x + n->point.x;
                    pt.y = vector.y + n->point.y;
                    if(rts_valid_tile(player, unit, pt))
                    {

                        printf("Adding direction %d to open list!\n", a);

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
                        printf("Actual id: %s, H: %d G:%d F:%d Deph:%d\n", e[count].id, e[count].h,
                               e[count].g, e[count].val, e[count].deph);

                        //Is this element in closed list?
                        if((p = hashtable_lookup(closed, e[count].id)) != NULL)
                        {
                            printf("P exists in cloded list!\n");
                            if(p->val > e[count].val)
                            {
                                if(!hashtable_remove(closed, p->id))
                                {
                                    printf("Error ocurred while trying to remove key in hashtable!\n");
                                    hashtable_iter(closed, hashtable_default_hash);
                                    return NULL;
                                }
                                else
                                {
                                    printf("Removes OK, let's check integrity!\n");
                                    hashtable_iter(closed, hashtable_default_hash);
                                }
                                if(!bheap_add(open, p))
                                {
                                    printf("Error ocurred while adding a element to open list\n");
                                    return NULL;
                                }
                                printf("Succesfully removed from closed list and added to open list\n");
                            }
                        }   
                        else
                        {
                            printf("P doesn't exist in closed list!\n");
                            if(!bheap_add(open, &e[count]))
                            {
                                printf("Error ocurred while adding a new element to open list\n");
                                return NULL;
                            }
                        }
                        bheap_print(open);
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

