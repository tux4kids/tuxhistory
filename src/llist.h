#ifndef DSTRUCTS_H 

#include "tuxhistory.h"

typedef struct list_node{
    struct list_node *next;
    th_obj object;
}list_node;

list_node *list_add(list_node **, th_obj);
void list_remove(list_node **);
list_node **list_search(list_node **, int);
void list_clean(list_node **);
 
#define DSTRUCT_H
#endif
