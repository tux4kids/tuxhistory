/*
 * llist.h
 *
 * Description: Linked list h file
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) (C) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */
#ifndef DSTRUCTS_H 
#define DSTRUCTS_H

#include "tuxhistory.h"

typedef struct list_node{
    struct list_node *next;
    th_obj object;
}list_node;

list_node *list_add(list_node **, th_obj);
void list_remove(list_node **);
list_node **list_search(list_node **, int);
void list_clean(list_node **);
 
#endif
