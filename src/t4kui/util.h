/*
 * util.h
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

#ifndef __UTIL_H__
#define __UTIL_H__

typedef struct _LinkedList LinkedList;
typedef struct _Node Node;

LinkedList *list_Create();
void list_AddNode(LinkedList *list, void *val);
Node *list_GetFirst(LinkedList *list);
Node *list_Traverse(Node *node);
void *list_GetNodeValue(Node *node);
void list_Destroy(LinkedList *list);

#endif /* __UTIL_H__ */

