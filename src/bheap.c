/* bheap.c
 *
 * Description: Binary Heap functions.
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
#include<assert.h>

#include "bheap.h"

#define LEFT(K) (2*K)
#define RIGHT(K) (2*K+1)
#define SWAP(H,A,B) void *tmp = H->items[A]; \
                    H->items[A] = H->items[B]; \
                    H->items[B] = tmp;

bheap *bheap_init(int size)
{
    bheap *heap;
    bheap_node **items;

    heap = (bheap *)malloc(sizeof(bheap));
    if(heap == NULL)
        return NULL;
 
    items = (bheap_node **)malloc(size * sizeof(bheap_node *));
    if(items == NULL)
        return NULL;
   
    heap->size = size;
    heap->count = -1;
    heap->items = items;

    return heap;
}

int bheap_add(bheap *heap, bheap_node *data)
{
    //bheap_node *node;
    int m;
    int i;

    if(!heap)
        return 0;

    if(!data)
        return 0;

    /*
    node = (bheap_node *)malloc(sizeof(bheap_node));
    if(node == NULL)
        return 0;

    *node = data;
    */

    printf("Begining bheap_add... the count is %d\n", heap->count);

    printf("Data to store, val %d, x %d, y %d.\n", data->val, data->point.x, data->point.y);
    for(i=0; i<heap->count; i++)
    {
        if(!heap->items[i])
        {
            printf("Error accesing item!");
            return 0;
        }
        printf("In loop %d, val %d\n", i, heap->items[i]->val);
        if( heap->items[i]->point.x == data->point.x &&
            heap->items[i]->point.y == data->point.y)
        {
            printf("Is allready in openlist\n"); 
            return 1;
        }
    }
    printf("Is not already in open list!\n");

    heap->count++;
    printf("* New count value %d\n", heap->count);
    if(heap->size < heap->count)
        return 0;

    printf("* Count is smaller than size..\n");
    m = heap->count;
    heap->items[heap->count] = data;
    while(m != 0)
    {
        if(heap->items[m]->val < heap->items[m/2]->val)
        {
            SWAP(heap, m, m/2);
            m = m/2;
        }
        else
        {
            break;
        }
    }

    printf("Ending bheap_add...\n");
    printf("OK Cheking integrity...\n");
    printf("Priority :%d New element: %d ALL O.K.\n", heap->items[0]->val, heap->items[heap->count]->val);
    return 1;
}

bheap_node *bheap_del(bheap *heap)
{
    int u, v;
    bheap_node *node;

    if(heap->count < 0)
    {
        printf("Error, there is no element to delete!\n");
        return NULL;
    }
    
    node = heap->items[0];
    //free(heap->items[0]);
    heap->items[0] = heap->items[heap->count];
    heap->items[heap->count] = NULL;
    heap->count--;
    if(heap->count < 0)
        printf("heap is empty!\n");
    v = 0;
    do{
        u = v;
        if(RIGHT(u) <= heap->count)
        {
            if(heap->items[u]->val >= heap->items[LEFT(u)]->val)
            {
                v = LEFT(u);
            }
            if(heap->items[v]->val >= heap->items[RIGHT(u)]->val)
            {
                v = RIGHT(u);
            }
        }
        else if(LEFT(u) <= heap->count)
        {
            if(heap->items[u]->val >= heap->items[LEFT(u)]->val)
            {
                v = LEFT(u);
            }
        }
        if(u != v)
        {
            SWAP(heap,u, v);
        }
        else
        {
            return node;
        }
    }while(1);
}

void bheap_print(bheap *heap)
{
    int i;
    if(!heap)
    {
        printf("bheap_print: NULL parameter!\n");
        return;
    }
    printf("Begining bheap_printf... the count is %d\n", heap->count);
    if(heap->count < 0 || heap->count >= heap->size)
    {
        printf("Error, there are no elements to print!\n");
        return;
    }
    for(i=0; i<heap->count; i++)
    {
        if(!heap->items)
        {
            printf("Error trying to print item %d\n", i);
            return;
        }
        printf("loop %d ",i);
        printf("%s:%d\n ", heap->items[i]->id, heap->items[i]->val);
    }
    printf("End of bheap_print\n");
}

void bheap_free(bheap *heap)
{
    /*int i;
    for(i=0; i<=heap->count; i++)
    {
        free(heap->items[i]);
        heap->items[i] = NULL;
    }*/
    assert(heap->items != NULL);
    free(heap->items);
    heap->items = NULL;
    assert(heap != NULL);
    free(heap);
    heap = NULL;
}
