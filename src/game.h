/*
 * game.c
 *
 * Description: The main game loop for TuxHistory
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#ifndef GAME_H
#define GAME_H

#define NUM_BKGDS 8

#define GAME_MESSAGE_LENGTH 100

typedef struct {
  int x,y;
  int alpha;
  char message[GAME_MESSAGE_LENGTH];
} game_message;

enum {
  GAME_IN_PROGRESS,
  GAME_OVER_WON,
  GAME_OVER_LOST,
  GAME_OVER_OTHER,
  GAME_OVER_ESCAPE,
  GAME_OVER_WINDOW_CLOSE,
  GAME_OVER_CHEATER,
  GAME_OVER_ERROR
};

int game(void);
void game_set_start_message(const char*, const char*, const char*, const char*);

void draw_nums(const char* str, int x, int y);
void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b);
void draw_numbers(const char* str, int x, int y);

#endif
