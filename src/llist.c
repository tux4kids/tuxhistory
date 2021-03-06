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

#include "map.h"
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
    node->obj = obj;
    node->prev = NULL;

    if(object_counter != 0)
        node->next->prev = node;

    (*ptr)->obj.id = object_counter;

    object_counter++;

    return node;
}

void list_remove(list_node** ptr)
{
    list_node *node;
    list_node *tmp;
    if(ptr != NULL && *ptr != NULL)
    {
        node = *ptr;
        if(node->prev)
        {
            tmp = (*ptr)->prev;
        }
        else
        {
            tmp = NULL;
            list_nodes = (*ptr)->next;
        }
        *ptr = (*ptr)->next; //
        (*ptr)->prev = tmp;
        if(tmp)
            tmp->next = *ptr;
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
        if((*node)->obj.id == data)
        {
            return node;
        }
        node = &(*node)->next;
    }
    return NULL;
}

void list_print(list_node *node)
{
    if (node == NULL)
    {
        printf("list is empty\n");
    }
    while (node != NULL)
    {
        printf("in List objecin List object: %p %p %s\n", node, node->next, node->obj.name);
        node = node->next;
    }
}


void list_clean(list_node **node)
{
    while(*node != NULL)
        list_remove(node);
}


