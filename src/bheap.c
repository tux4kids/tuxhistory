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

    heap->count++;

    if(heap->size < heap->count)
        return 0;

    if(heap == NULL)
        return 0;

    if(data == NULL)
        return 0;

    /*
    node = (bheap_node *)malloc(sizeof(bheap_node));
    if(node == NULL)
        return 0;

    *node = data;
    */
    
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
    if(heap->count < 0)
    {
        printf("Error, there are no elements to print!\n");
        return;
    }
    for(i=0; i<=heap->count; i++)
        printf("%d ", heap->items[i]->val);
    printf("\n");
}

void bheap_free(bheap *heap)
{
    /*int i;
    for(i=0; i<=heap->count; i++)
    {
        free(heap->items[i]);
        heap->items[i] = NULL;
    }*/
    free(heap->items);
    heap->items = NULL;
    free(heap);
    heap = NULL;
}
