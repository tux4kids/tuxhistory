#ifndef PLAYERS_H
#define PLAYERS_H

#include "globals.h"
#include "graphs.h"

#define NAME_LEN 50

enum{
    CIV_DEFAULT,
    NUM_CIVILIZATIONS
};

typedef struct th_players{
    char name[NAME_LEN];
    int player_num;
    int civ;
    int max_pop;
    int pop;
    int stone;
    int wood;
    int food;
    int gold;
    gnode *pos;
}th_players;

int num_of_players;
int human_player;
int last_player;
th_players *player;

int init_players(int players, int human);
int add_player(char *name, int civ, int max_pop, int stone, 
               int wood, int food, int gold, th_point pos);
void clean_players(void);



#endif
