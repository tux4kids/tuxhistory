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

/* put this first so we get <config.h> and <gettext.h> immediately: */
#include "tuxhistory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "transtruct.h"
#include "game.h"
#include "fileops.h"
#include "setup.h"
#include "loaders.h"
#include "titlescreen.h"
#include "options.h"
#include "SDL_extras.h"
#include "pixels.h"
#include "map.h"


#define FPS 15               /* 15 frames per second */
#define MS_PER_FRAME (1000 / FPS)

/************ Static Variable definitions ***********/
//Game variables

static int quit;

// ControlKeys
static int left_pressed;
static int right_pressed;
static int up_pressed;
static int shift_pressed;
static int shoot_pressed;

// GameControl
static int game_status;
static int paused;
static int user_quit_received;
static int SDL_quit_received;
//static int gameover_counter;
static int escape_received;

// Background Surfaces
static SDL_Surface* bkgd = NULL; //640x480 background (windowed)
static SDL_Surface* scaled_bkgd = NULL; //native resolution (fullscreen)

/********** Static functions definitions *********/

static int game_init(void);

static void game_draw(void);
static void game_handle_user_events(void);
static int game_mouse_event(SDL_Event event);

static int check_exit_conditions(void);
static int game_over(int);
static int pause_game(void);

/************** Implementation *******************/

static SDL_Surface* current_bkgd()
{ 
    return screen->flags & SDL_FULLSCREEN ? scaled_bkgd : bkgd; 
}

static int game_init(void)
{
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);
    quit = 0;

    left_pressed = 0;
    right_pressed = 0;
    up_pressed = 0;
    shift_pressed = 0;
    shoot_pressed = 0;

    game_status = 0;
    paused = 0;
    user_quit_received = 0;
    SDL_quit_received = 0;
    escape_received = 0;

    return 1;
}


int game(void)
{
    Uint32 last_time, now_time;

    DEBUGMSG(debug_game, "Entering game():\n");

      //see if the option matches the actual screen
    if (Opts_GetGlobalOpt(FULLSCREEN) == !(screen->flags & SDL_FULLSCREEN) )
    {
        ;//SwitchScreenMode();  //Huh??
    }

    if(!game_init())
    {
        DEBUGMSG(debug_game, "Error loading game using game_init()\n");
        return -1;
    }

    while(game_status == GAME_IN_PROGRESS)
    {
        last_time = SDL_GetTicks();
        
        game_handle_user_events();
        game_status = check_exit_conditions();
        game_draw();
        SDL_Flip(screen);

        if(paused)
        {
            pause_game();
            paused = 0;
        }

        // Maintain Frame-rate
        now_time = SDL_GetTicks();
        if(now_time < last_time + MS_PER_FRAME)
        {
            now_time = (last_time+MS_PER_FRAME) - now_time;
            if (now_time > MS_PER_FRAME)
                now_time = MS_PER_FRAME;
            SDL_Delay(now_time);
        }
    }
    game_over(game_status);
}

static void game_draw(void)
{
    SDL_Surface* surf;
    SDL_Rect dest;
    
    dest.x = 200;
    dest.y = 200;

    SDL_BlitSurface(terrain[TUNDRA_CENTER_1], NULL, screen, &dest);
    SDL_BlitSurface(objects[OBJ_TROPICAL], NULL, screen, &dest);

    dest.x = (screen->w - images[IMG_STOP]->w - 5);
    dest.y = glyph_offset;
    SDL_BlitSurface(images[IMG_STOP], NULL, screen, &dest);
 
    
}

static int pause_game(void)
{
  /* NOTE - done and quit changed to pause_done and pause_quit */
  /* due to potentially confusing name collision */
  int pause_done, pause_quit;
  SDL_Event event;
  SDL_Rect dest;

  /* Only pause if pause allowed: */
  if (!Opts_AllowPause())
  {
    fprintf(stderr, "Pause requested but not allowed by Opts!\n");
    return 0;
  }

  pause_done = 0;
  pause_quit = 0;

  dest.x = (screen->w - images[IMG_PAUSED]->w) / 2;
  dest.y = (screen->h - images[IMG_PAUSED]->h) / 2;
  dest.w = images[IMG_PAUSED]->w;
  dest.h = images[IMG_PAUSED]->h;

  DarkenScreen(1);  // cut all channels by half
  SDL_BlitSurface(images[IMG_PAUSED], NULL, screen, &dest);
  SDL_UpdateRect(screen, 0, 0, 0, 0);

#ifndef NOSOUND
  if (Opts_UsingSound())
    Mix_PauseMusic();
#endif

  do
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_KEYDOWN)
        pause_done = 1;
      else if (event.type == SDL_QUIT)
      {
        user_quit_received = GAME_OVER_WINDOW_CLOSE;
        pause_quit = 1;
      }
    }

    SDL_Delay(100);
  }
  while (!pause_done && !pause_quit);

#ifndef NOSOUND
  if (Opts_UsingSound())
    Mix_ResumeMusic();
#endif

  return (pause_quit);
}

static int game_over(int game_status)
{
  Uint32 last_time, now_time; 
  SDL_Rect dest_message;
  SDL_Event event;


  /* TODO: need better "victory" screen with animation, special music, etc., */
  /* as well as options to review missed questions, play again using missed  */
  /* questions as question list, etc.                                        */
  switch (game_status)
  {
    case GAME_OVER_WON:
    {
      int looping = 1;
//      int frame;
      /* set up victory message: */
      dest_message.x = (screen->w - images[IMG_GAMEOVER_WON]->w) / 2;
      dest_message.y = (screen->h - images[IMG_GAMEOVER_WON]->h) / 2;
      dest_message.w = images[IMG_GAMEOVER_WON]->w;
      dest_message.h = images[IMG_GAMEOVER_WON]->h;

      do
      {
        last_time = SDL_GetTicks();

        SDL_BlitSurface(images[IMG_GAMEOVER_WON], NULL, screen, &dest_message);
        SDL_Flip(screen);

        while (1)
        {
	      SDL_PollEvent(&event);
          if  (event.type == SDL_QUIT
            || event.type == SDL_KEYDOWN
            || event.type == SDL_MOUSEBUTTONDOWN)
          {
            looping = 0;
	    break;
          }
        }

        now_time = SDL_GetTicks();

        if (now_time < last_time + MS_PER_FRAME)
	  SDL_Delay(last_time + MS_PER_FRAME - now_time);
      }
      while (looping);
      break;
    }

    case GAME_OVER_ERROR:
    {
      DEBUGMSG(debug_factoroids, "game() exiting with error");
    }
    case GAME_OVER_LOST:
    case GAME_OVER_OTHER:
    {
      int looping = 1;

      /* set up GAMEOVER message: */
      dest_message.x = (screen->w - images[IMG_GAMEOVER]->w) / 2;
      dest_message.y = (screen->h - images[IMG_GAMEOVER]->h) / 2;
      dest_message.w = images[IMG_GAMEOVER]->w;
      dest_message.h = images[IMG_GAMEOVER]->h;

      do
      {
        //frame++;
        last_time = SDL_GetTicks();

        SDL_BlitSurface(images[IMG_GAMEOVER], NULL, screen, &dest_message);
        SDL_Flip(screen);

        while (1)
        {
	  SDL_PollEvent(&event);
          if  (event.type == SDL_QUIT
            || event.type == SDL_KEYDOWN
            || event.type == SDL_MOUSEBUTTONDOWN)
          {
            looping = 0;
	    break;
          }
        }

        now_time = SDL_GetTicks();

        if (now_time < last_time + MS_PER_FRAME)
	  SDL_Delay(last_time + MS_PER_FRAME - now_time);
      }
      while (looping);

      break;
    }

    case GAME_OVER_ESCAPE:
    {
      break;
    }

    case GAME_OVER_WINDOW_CLOSE:
    {
      break;
    }

  }

  /* Return the chosen command: */
  if (GAME_OVER_WINDOW_CLOSE == game_status)
  {
    /* program exits: */
    return 1;
  }
  else
  {
    /* return to title() screen: */
    return 0;
  }
}



static void game_handle_user_events(void)
{
  SDL_Event event;
  SDLKey key;

  while (SDL_PollEvent(&event) > 0)
  {
    if (event.type == SDL_QUIT)
    {
      SDL_quit_received = 1;
      quit = 1;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
      key = game_mouse_event(event);
    }
    if (event.type == SDL_KEYDOWN ||
	event.type == SDL_KEYUP)
    {
      key = event.key.keysym.sym;
      
      if (event.type == SDL_KEYDOWN)
	  {
	    if (key == SDLK_ESCAPE)
	    {
            // Return to menu! 
            escape_received = 1;

	    }
	  
	    // Key press... 
	 
	    if (key == SDLK_RIGHT)
	    {
	      // Rotate CW 
	      
 	      left_pressed = 0;
	      right_pressed = 1;
	    }
	    else if (key == SDLK_LEFT)
	    {
	      // Rotate CCW 
	      
	      left_pressed = 1;
	      right_pressed = 0;
	    }
	    else if (key == SDLK_UP)
	    {
	      // Thrust! 
	      
	      up_pressed = 1;
	    }
	  
	    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
	    {
	      // Respawn now (if applicable) 
	      shift_pressed = 1;
	    }

	    if (key == SDLK_TAB || key == SDLK_p)
  	    {
        /* [TAB] or [P]: Pause! (if settings allow) */
    	  if (Opts_AllowPause())
    	  {
    	    paused = 1;
    	  }
  	    }
	  }
      else if (event.type == SDL_KEYUP)
	  {
	    // Key release... 
	  
	    if (key == SDLK_RIGHT)
	    {
	      right_pressed = 0;
	    }
	    else if (key == SDLK_LEFT)
	    {
               left_pressed = 0;
 	    }
	    else if (key == SDLK_UP)
	    {
	      up_pressed = 0;
	    }
	    if (key == SDLK_LSHIFT ||
	        key == SDLK_RSHIFT)
	    {
	      // Respawn now (if applicable) 
	      shift_pressed = 0;
	    }
      }
    }
  }
}

static int game_mouse_event(SDL_Event event)
{
  SDLKey key = SDLK_UNKNOWN;

  /* Check to see if user clicked exit button: */
  /* The exit button is in the upper right corner of the screen: */
  if ((event.button.x >= (screen->w - images[IMG_STOP]->w))
    &&(event.button.y <= images[IMG_STOP]->h))
  {
    key = SDLK_ESCAPE;
    //game_key_event(key);
    escape_received = 1;
    quit = 1;
    return -1;
  } 

  if (key == SDLK_UNKNOWN)
  {
      return -1;
  }

    /* now can proceed as if keyboard was used */
    //game_key_event(key);
    return key;
  
}


static int check_exit_conditions(void)
{
  if(SDL_quit_received)
  {
    return GAME_OVER_WINDOW_CLOSE;
  }

  if(escape_received)
  {
    return GAME_OVER_ESCAPE;
  }
  // TODO: Loose or win...
  return GAME_IN_PROGRESS;
}

