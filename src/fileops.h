/*
C Interface: fileops.h

Description: File operations - together, fileops.h and fileops.c contain
all code involving disk operations. The older header files images.h and
sounds.h have been incorporated here. The intention is to make it easier to
port tuxmath to other operating systems, as code to read and write as
well as paths and file locations may be more OS-dependent.


Author: David Bruce <davidstuartbruce@gmail.com>, (C) 2006
Contains code originally written by Bill Kendrick (C) 2001.
Copyright: See COPYING file that comes with this distribution (briefly, GNU GPL)
*/

#ifndef FILEOPS_H
#define FILEOPS_H

#include "globals.h"

/* Flag basically telling whether or not to allow admin-level */
/* settings to be changed: */
enum {
  USER_CONFIG_FILE,
  GLOBAL_CONFIG_FILE
};

/* Names for images (formerly in images.h) */
enum {
  IMG_TITLE,
  IMG_LEFT,
  IMG_LEFT_GRAY,
  IMG_RIGHT,
  IMG_RIGHT_GRAY,
  IMG_TUX4KIDS,
  IMG_NBS,
  IMG_NUMS,
  IMG_LEDNUMS,
  IMG_LED_NEG_SIGN,
  IMG_PAUSED,
  IMG_DEMO,
  IMG_DEMO_SMALL,
  IMG_KEYPAD,
  IMG_KEYPAD_NO_NEG,
  IMG_CONSOLE_LED,
  IMG_CONSOLE_BASH,
  IMG_TUX_CONSOLE1,
  IMG_TUX_CONSOLE2,
  IMG_TUX_CONSOLE3,
  IMG_TUX_CONSOLE4,
  IMG_TUX_RELAX1,
  IMG_TUX_RELAX2,
  IMG_TUX_EGYPT1,
  IMG_TUX_EGYPT2,
  IMG_TUX_EGYPT3,
  IMG_TUX_EGYPT4,
  IMG_TUX_DRAT,
  IMG_TUX_YIPE,
  IMG_TUX_YAY1,
  IMG_TUX_YAY2,
  IMG_TUX_YES1,
  IMG_TUX_YES2,
  IMG_TUX_SIT,
  IMG_TUX_FIST1,
  IMG_TUX_FIST2,
  IMG_WAVE,
  IMG_SCORE,
  IMG_STOP,
  IMG_NUMBERS,
  IMG_GAMEOVER,
  IMG_GAMEOVER_WON,
  NUM_IMAGES
};

/* Names for animated images (sprites) */
enum {
  IMG_COMET,
  IMG_BONUS_COMET,
  IMG_COMET_EXPL,
  IMG_BONUS_COMET_EXPL,
  NUM_SPRITES
};

/* Names for game sounds (formerly in sounds.h): */
enum {
  SND_HARP,
  SND_POP,
  SND_TOCK,
  SND_LASER,
  SND_BUZZ,
  SND_ALARM,
  SND_SHIELDSDOWN,
  SND_EXPLOSION,
  SND_SIZZLE,
  SND_BONUS_COMET,
  SND_EXTRA_LIFE,
  NUM_SOUNDS
};

/* Names for background music (also formerly in sounds.h): */
enum {
  MUS_GAME,
  MUS_GAME2,
  MUS_GAME3,
  NUM_MUSICS
};

/* Names for game summary files: */
enum {
  SUMMARY1,
  SUMMARY2,
  SUMMARY3,
  SUMMARY4,
  SUMMARY5,
  SUMMARY6,
  SUMMARY7,
  SUMMARY8,
  SUMMARY9,
  SUMMARY10,
  NUM_SUMMARIES
};

/* These functions used by setup() and titlescreen() to read in settings: */
int read_global_config_file(void);
int read_user_config_file(void);
int parse_lesson_file_directory(void);
int read_named_config_file(const char* fn);
int write_user_config_file(void);
int read_high_scores(void);
int append_high_score(int tableid,int score,char *player_name);
void set_high_score_path(void);
void set_user_data_dir(const char* dirname);
int write_goldstars(void);

/* These functions are used by titlescreen() to assist with the login */
int read_user_menu_entries(char ***user_names);
int read_user_login_questions(char ***user_login_questions);
int high_scores_found_in_user_dir(void);
void set_high_score_path(void);
void user_data_dirname_up(void);
void user_data_dirname_down(char *subdir);

/* These functions used by game() to record game summary: */
int write_pregame_summary(void);
int write_postgame_summary(void);

int load_image_data();


#ifndef NOSOUND
int load_sound_data();
#endif

#endif
