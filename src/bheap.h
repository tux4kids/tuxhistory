#ifndef BHEAP_H
#define BHEAP_H

typedef struct bheap_node{
    int val;
}bheap_node;

typedef struct bheap{
    int size;
    int count;
    bheap_node **items;
}bheap;

bheap *bheap_init(int size);
int bheap_add(bheap *heap, bheap_node data);
void bheap_print(bheap *heap);
bheap_node bheap_del(bheap *heap);
void bheap_free(bheap *heap);

#endif
