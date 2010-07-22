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
#include "graphs.h"
#include "map.h"
#include "objects.h"
#include "llist.h"


#define FPS 15 /* 15 frames per second */
#define MS_PER_FRAME (1000 / FPS)

#define IN_SCROLL 3 // Scroll speed
#define OUT_SCROLL 6
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

// Game vars
static SDL_Rect origin;
static SDL_Rect select_rect;
static int screen_x;
static int screen_y;

static th_point Pmouse;
static th_point Pscreen;
static int screen_margin_in;
static int screen_margin_out;
/********** Static functions definitions *********/

static int game_init(void);

static void game_draw(void);
static void game_handle_user_events(void);
static void game_handle_mouse(void);
static int game_mouse_event(SDL_Event event);

static th_point mouse_map(th_point mouse_p, th_point screen_p);

static int check_exit_conditions(void);
static int game_over(int);
static int pause_game(void);

static void get_mouse_pos(int, int);
/************** Implementation *******************/

static SDL_Surface* current_bkgd()
{ 
    return screen->flags & SDL_FULLSCREEN ? scaled_bkgd : bkgd; 
}

static int game_init(void)
{
    FILE *fp;

    //SDL variables init
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_Flip(screen);

    generate_map();

    //SDL_Flip(map_image);
    quit = 0;
    
    Pscreen.x = 500;//map_image->w/2 - screen->w/2;
    Pscreen.y = 100;//map_image->h/2 - screen->h/2;
    origin.x = Pscreen.x;
    origin.y = Pscreen.y;
    origin.w = screen->w;
    origin.h = screen->h;

    screen_margin_in = (int)screen->w/15;
    screen_margin_out= (int)screen->w/50;

    //Control variables init
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

    fp = LoadObj("objects");
    if(fp == NULL)
    {
        printf("File not found!\n");
        DEBUGMSG(debug_game, "File not found!");
        return 1;
    }
    printf("Object files in memory!\n");
    if(objects_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the objects description file.\n");
        return 1;
    }
    printf("Object file parsed.\n");
   
    fp = LoadMap("map");
    if(fp == NULL)
    {
        DEBUGMSG(debug_game, "File not found!");
        return 1;
    }
    printf("Map file in memory.\n");

    if(map_xml(fp))
    {
        printf("Error parsing file!");
        DEBUGMSG(debug_game, "Error loading the map file.\n");
        return 1;
    }
    printf("Map file parsed!\n");
    if(create_gmaps(2))
    {
        printf("Couldn't generate grpah mesh!\n");
    }
    generate_map();

    return 0;
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

    if(game_init())
    {
        DEBUGMSG(debug_game, "Error loading game using game_init()\n");
        return -1;
    }

    while(game_status == GAME_IN_PROGRESS)
    {
        last_time = SDL_GetTicks();
        
        game_handle_user_events();
        game_handle_mouse();

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
    SDL_Rect dest;
    list_node *obj_node;

    origin.x = Pscreen.x;
    origin.y = Pscreen.y;
    
    /*
    origin.w = screen->w;
    origin.h = screen->h;
    
    
    origin.x = map_image->w/2 - screen->w/2;
    origin.y = map_image->h/2 - screen->h/2;
    origin.w = screen->w;
    origin.h = screen->h;*/

    dest.x = 0;
    dest.y = 0;

    /*TODO: Separate each Layer drawing in different functions.*/


    /*First layer: terrain*/
    SDL_BlitSurface(map_image, &origin, screen, &dest);

    /*Second layer: objects*/

    obj_node = list_nodes;
    if(obj_node != NULL)
    {
        do{
            if( gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.x > origin.x -
                    objects[obj_node->obj.name_enum]->w/2&&
                gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.y > origin.y - 
                    objects[obj_node->obj.name_enum]->w/2&&
                gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.x < origin.x + origin.w &&
                gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.y < origin.y + origin.w)
            {
                dest.x = gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.x - 
                    origin.x - objects[obj_node->obj.name_enum]->w/2;
                dest.y = gmaps[0][obj_node->obj.x][obj_node->obj.y].anchor.y - 
                    origin.y - objects[obj_node->obj.name_enum]->h/2;
                SDL_BlitSurface(objects[obj_node->obj.name_enum], NULL, screen, &dest);
             }
            obj_node = obj_node->next;
        }while(obj_node != NULL);
    }

    SDL_BlitSurface(images[IMG_ISOSELECT], NULL, screen, &select_rect);
   
    /*Third layer: User Interface*/
    dest.x = (screen->w - images[IMG_STOP]->w - 5);
    dest.y = glyph_offset;
    SDL_BlitSurface(images[IMG_STOP], NULL, screen, &dest);
 
    
}
static void game_handle_mouse(void)
{
    th_point Pmousemap;
    int i, j;

    Pmousemap = mouse_map(Pmouse, Pscreen);
    if(Pmousemap.x != -1 && Pmousemap.y != -1)
    {
        select_rect.x = gmaps[0][Pmousemap.x][Pmousemap.y].rect.x - Pscreen.x; 
        select_rect.y = gmaps[0][Pmousemap.x][Pmousemap.y].rect.y - Pscreen.y;
        printf("Draw select: %d %d ", select_rect.x, select_rect.y);
    

        if( Pscreen.x < (map_image->w - screen->h) &&
            Pscreen.x > 0 &&
            Pscreen.y < (map_image->h - screen->h) &&
            Pscreen.y > 0)
        {
            // Is the mouse close to the border? Move the
            // map...
            if(Pmouse.x < screen_margin_in && Pscreen.x > IN_SCROLL){
                Pscreen.x = Pscreen.x - IN_SCROLL;
            }
            if(Pmouse.x > screen->w - screen_margin_in &&
                (Pscreen.x + screen->w) < (map_image->w - IN_SCROLL)){
                Pscreen.x = Pscreen.x + IN_SCROLL;
            }
            if(Pmouse.y < screen_margin_in && Pscreen.y > IN_SCROLL){
                Pscreen.y = Pscreen.y - IN_SCROLL;
            }
            if(Pmouse.y > screen->h - screen_margin_in &&
                    (Pscreen.y + screen->h) < (map_image->h - IN_SCROLL)){
                Pscreen.y = Pscreen.y + IN_SCROLL;
            }

            // Is the mouse VERY close to the border?
            // Move the map faster!
            if(Pmouse.x < screen_margin_out && Pscreen.x > OUT_SCROLL){
                Pscreen.x = Pscreen.x - OUT_SCROLL;
            }
            if(Pmouse.x > screen->w - screen_margin_out &&
            (Pscreen.x + screen->w) < (map_image->w - OUT_SCROLL)){
                Pscreen.x = Pscreen.x + OUT_SCROLL;
            }
            if(Pmouse.y < screen_margin_out && Pscreen.y > OUT_SCROLL){
                Pscreen.y = Pscreen.y - OUT_SCROLL;
            }
            if(Pmouse.y > screen->h - screen_margin_out &&
                    (Pscreen.y + screen->h) < (map_image->h - OUT_SCROLL)){
                Pscreen.y = Pscreen.y + OUT_SCROLL;
            }
        }
    }

}

Uint32 get_pcolor(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[ ( y * surface->w ) + x ];
}


// TODO: This function should be in map.c
//       give the mouse point and the creen point as arguments 
//       and returns the actual tile in ***gmaps
static th_point mouse_map(th_point mouse_p, th_point screen_p)
{
    int i, j;
    int terr_e;
    Uint32 color;
    Uint32 iso_colors[4];
    th_point *anchor_p;
    th_point Pmousemap;
    th_point Ptilemap;

    Pmousemap.x = (int)(mouse_p.x + screen_p.x + terrain[TUNDRA_CENTER_1]->w/2)/terrain[TUNDRA_CENTER_1]->w;
    Pmousemap.y = (int)(mouse_p.y + screen_p.y)/terrain[TUNDRA_CENTER_1]->h;
    
    Ptilemap.x = (int)(mouse_p.x + screen_p.x + terrain[TUNDRA_CENTER_1]->w/2)%terrain[TUNDRA_CENTER_1]->w;
    Ptilemap.y = (int)(mouse_p.y + screen_p.y)%terrain[TUNDRA_CENTER_1]->h;
    

    anchor_p = &anchor_map[Pmousemap.x][Pmousemap.y];
    if(anchor_p->x != -1 && anchor_p->y != -1)
        terr_e = gmaps[0][anchor_p->x][anchor_p->y].terrain;
    else
        terr_e = -1;


    Pmousemap.x = anchor_p->x;
    Pmousemap.y = anchor_p->y; 

    iso_colors[1] = get_pcolor(images[IMG_ISOMAPPER], 2, 2);
    iso_colors[2] = get_pcolor(images[IMG_ISOMAPPER], images[IMG_ISOMAPPER]->w-2, 2);
    iso_colors[3] = get_pcolor(images[IMG_ISOMAPPER], 2 , images[IMG_ISOMAPPER]->h-2);
    iso_colors[4] = get_pcolor(images[IMG_ISOMAPPER], images[IMG_ISOMAPPER]->w-2, images[IMG_ISOMAPPER]->h-2);

    color = get_pcolor(images[IMG_ISOMAPPER], Ptilemap.x, Ptilemap.y);


    // NW
    if(color == iso_colors[1])
    {
        Pmousemap.x--;
    }
    //NE
    if(color == iso_colors[2])
    {
        Pmousemap.y--;
    }
    //SW
    if(color == iso_colors[3])
    {
        Pmousemap.y++;
    }
    //SE
    if(color == iso_colors[4])
    {
        //Pmousemap.y++;
        Pmousemap.x++;
    }

    printf("Mouse Maping: %d, %d Terrain: %d Color: %d\n", Pmousemap.x, Pmousemap.y, terr_e, color);

    return Pmousemap;
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
    //eval_mouse_pos(event.);
    if (event.type == SDL_QUIT)
    {
      SDL_quit_received = 1;
      quit = 1;
    }
    if (event.type == SDL_MOUSEMOTION)
    {
        get_mouse_pos(event.motion.x, event.motion.y);
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

static void get_mouse_pos(int x, int y)
{
    Pmouse.x = x;
    Pmouse.y = y;
}

