/* ai.h
 *
 * Description: AI mainy path finding functions for the game lives here
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#ifndef AI_H
#define AI_H

#include "lua.h"
#include "tuxhistory.h"
#include "globals.h"
#include "graphs.h"
#include "llist.h"


// Global state!
lua_State *L;

int ai_init(int players);

th_path *ai_shortes_path(int, int, th_point source, th_point goal);

void ai_free_path(th_path *path);

int ai_modify_state(int player, th_obj *object, int state);

th_point ai_alternative_tile(th_point p1, th_point p2);

int ai_state_update(list_node *node);

int ai_valid_tile(int player, int unit, th_point coords);
#endif
