/*
 * llist.h
 *
 * Description: Linked list implementations 
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) (C) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */
#include<stdio.h>
#include<stdlib.h>

#include "llist.h"

list_node *list_add(list_node **ptr, th_obj obj)
{
    if (ptr == NULL)
    {
        return NULL;
    }

    list_node *node = (list_node *)malloc(sizeof(list_node));
    if (node == NULL)
    {
        return NULL;
    }

    node->next = *ptr;
    *ptr = node;
    node->object = obj;
    return node;
}

void list_remove(list_node** ptr)
{
    if(ptr != NULL && *ptr != NULL)
    {
        list_node *node = *ptr;
        *ptr = (*ptr)->next;
        free(node);
    }
}

list_node **list_search(list_node **node, int data)
{
    if(node == NULL)
    {
        return NULL;
    }

    while(*node != NULL)
    {
        if((*node)->object.id == data)
        {
            return node;
        }
        node = &(*node)->next;
    }
    return NULL;
}

void list_clean(list_node **node)
{
    while(*node != NULL)
        list_remove(node);
}


