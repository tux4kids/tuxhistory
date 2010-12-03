/*
 * util.c
 * This file is part of Tux4Kids
 *
 * Copyright (C) 2010 - Aviral Dasgupta
 *
 * Tux4Kids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tux4Kids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tux4Kids; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include "util.h"
#include <stdlib.h>
#include <stdio.h>

struct _LinkedList {
	Node *first;
	Node *last;
};

struct _Node {
	void *data;
	Node *next;
};

struct _LinkedList *list_Create() {
	struct _LinkedList *list = malloc(sizeof(struct _LinkedList));
	list->first = NULL;
	list->last = NULL;
	return list;
}

void list_Destroy(LinkedList *list) {
	while((list->first)) {
		Node *tmp = list->first;
		list->first = tmp->next;
		free(tmp);
	}
	free(list);
}
Node *list_GetFirst(LinkedList *list) {return list->first;}
Node *list_Traverse(Node *node) {return node->next;}
void *list_GetNodeValue(Node *node) {return node->data;}

void list_AddNode(struct _LinkedList *list, void *val) {
	struct _Node *node = malloc(sizeof(struct _Node));
	node->data = val;
	node->next = NULL;
	if(list->last==NULL) {
		list->first = node;
		list->last = node;
		return;
	}
	list->last->next = node;
	list->last = node;
}

