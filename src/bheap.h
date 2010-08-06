/* bheap.h
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

#ifndef BHEAP_H
#define BHEAP_H

#include "globals.h"

#define ISEMPTY(heap) (heap->count<0)?1:0

typedef struct bheap_node{
    char id[7];
    int index;
    int val;
    int deph;
    int g, h;
    th_point point;
    struct bheap_node *parent;
}bheap_node;

typedef struct bheap{
    int size;
    int count;
    int item_count;
    bheap_node **items;
}bheap;

bheap *bheap_init(int size);
int bheap_add(bheap *heap, bheap_node *data);
void bheap_print(bheap *heap);
bheap_node *bheap_del(bheap *heap);
void bheap_free(bheap *heap);

#endif
