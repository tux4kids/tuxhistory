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
#include "tuxrts.h"


#define FPS 50 /* 15 frames per second */
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
static int screen_x;
static int screen_y;
static rts_vars selection;

static th_point Pscreen;
static int screen_margin_in;
static int screen_margin_out;

int this_player;

typedef struct io_vars
{
    th_point Pmouse;
    th_point Prclick;
    th_point Plclick;
    int mousedown_flag;
    int mousedownr_flag;
    int mouseclicked_flag;
    SDL_Rect select_xy;
    SDL_Rect select_rect;
    SDL_Rect select_rect_dest;
    SDL_Rect select;
    th_point go_xy;
    SDL_Rect go_rect;
    SDL_Rect go_rect_dest;
    int go_valid_flag;
}io_vars;

io_vars io;

/********** Static functions definitions *********/

static int game_init(void);

static void game_draw(int player);
static void game_handle_user_events(void);
static void game_handle_mouse(void);
static int game_mouse_event(SDL_Event event);

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

    // Input/output data strucure init.
    io.Pmouse.x = 0;
    io.Pmouse.y = 0;
    io.Prclick.x = -1;
    io.Prclick.y = -1;
    io.Plclick.x = -1;
    io.Plclick.y = -1;
    io.mousedown_flag = 0;
    io.mousedownr_flag = 0;
    io.mouseclicked_flag = 0;
    io.select_xy.x = 0;
    io.select_xy.y = 0;
    io.select_xy.w = 0;
    io.select_xy.h = 0;
    io.select_rect.x = 0;
    io.select_rect.y = 0;
    io.select_rect.w = 0;
    io.select_rect.h = 0;
    io.select_rect_dest.x = 0;
    io.select_rect_dest.y = 0;
    io.select_rect_dest.w = 0;
    io.select_rect_dest.h = 0;
    io.select.x = 0;
    io.select.y = 0;
    io.select.w = 0;
    io.select.h = 0;
    io.go_xy.x = -1;
    io.go_xy.y = -1;
    io.go_rect.x = 0;
    io.go_rect.y = 0;
    io.go_rect.w = 0;
    io.go_rect.h = 0;
    io.go_rect_dest.x = 0;
    io.go_rect_dest.y = 0;
    io.go_rect_dest.w = 0;
    io.go_rect_dest.h = 0;
    io.go_valid_flag = 0;

    // Player?
    this_player = 1;

    if(tuxrts_init("objects", "map", 2))
        return 1;

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

    if(!game_init())
    {
        DEBUGMSG(debug_game, "Error loading game using game_init()\n");
        return -1;
    }

    while(game_status == GAME_IN_PROGRESS)
    {
        last_time = SDL_GetTicks();

        //update_gmaps();
        rts_update_game();
        game_handle_user_events();
        game_handle_mouse();

        game_status = check_exit_conditions();
        game_draw(this_player);
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
    return 1;
}

static void draw_unexplored(int player, th_point point)
{
    int l;
    SDL_Rect dest;

    if(point.x == -1)
        return;
    if(point.y == -1)
        return;

    //Is the point in screen?
    if( gmaps[0][point.x][point.y].rect.x >= (Pscreen.x - images[IMG_EXPLORED]->w) &&
        gmaps[0][point.x][point.y].rect.x <  (Pscreen.x + screen->w) &&
        gmaps[0][point.x][point.y].rect.y >= (Pscreen.y - images[IMG_EXPLORED]->h) &&
        gmaps[0][point.x][point.y].rect.y <  (Pscreen.x + screen->w) &&
        gmaps[player][point.x][point.y].drawed == 0 )
    {
        //printf(" + ");
        dest.x = gmaps[0][point.x][point.y].rect.x - Pscreen.x;
        dest.y = gmaps[0][point.x][point.y].rect.y - Pscreen.y;

        //Is the point visible or unexplored? If so plaint!
        if(gmaps[player][point.x][point.y].visible == 0)
            if(gmaps[player][point.x][point.y].explored == 1)
                SDL_BlitSurface(images[IMG_EXPLORED], NULL, screen, &dest);
            else
                SDL_BlitSurface(images[IMG_NOVISIBLE], NULL, screen, &dest);
        gmaps[player][point.x][point.y].drawed = 1;
        
        //Call neighboors to draw!
        for(l = 0; l <= NUM_DIRS; l++)
        {
            if(gmaps[player][point.x][point.y].nodes_flag[l] == 1)
            {
                if(gmaps[player][point.x][point.y].nodes[l] != NULL)
                {
                    //printf(" -%d(%d,%d)- \n", l, point.x, point.y);
                    draw_unexplored(player, gmaps[player][point.x][point.y].nodes[l]->point);
                }
            }
        }
    }
    //else
        /*printf("NO DRAW: drawed:%d, (%d,%d) is not in (%d,%d)\n",
                gmaps[player][point.x][point.y].drawed,
                gmaps[0][point.x][point.y].rect.x,
                gmaps[0][point.x][point.y].rect.y,
                (Pscreen.x - images[IMG_EXPLORED]->w),
                (Pscreen.y - images[IMG_EXPLORED]->h));*/
}




static void game_draw(int player)
{
    SDL_Rect dest;
    list_node *obj_node;
    char tmp_text[50];
    th_point point;
    th_point dest_point;


    origin.x = Pscreen.x;
    origin.y = Pscreen.y;

    dest.x = 0;
    dest.y = 0;


    /*TODO: Separate each Layer drawing in different functions.*/

    /*First layer: terrain*/
    SDL_BlitSurface(map_image, &origin, screen, &dest);

    
    point.x = 0;
    point.y = 0;
    dest_point = mouse_map(point, Pscreen);
    while(dest_point.x == -1 && dest_point.y == -1)
    {
        point.x = point.x + images[IMG_EXPLORED]->w;
        point.y = point.y + images[IMG_EXPLORED]->h;
        dest_point = mouse_map(point, Pscreen);
    }
    //printf("Init in: (%d,%d)\n", dest_point.x, dest_point.y);
    draw_unexplored(player, dest_point); 

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

                // Is the any object selected?
                if(selection.selected_num != -1)
                {
                    if(selection.selected_objs[0] != NULL)
                    {
                        if(obj_node->obj.id == selection.selected_objs[0]->id)
                        {
                            dest.w = dest.x + objects[obj_node->obj.name_enum]->w;
                            dest.h = dest.y;

                            draw_line(screen, dest, 0,255,0);
                        }
                    }
                }
             }
            obj_node = obj_node->next;
        }while(obj_node != NULL);
    }
// User interaction
    if(io.select_rect_dest.x != -1 && io.select_rect_dest.y != -1)
        SDL_BlitSurface(images[IMG_ISOSELECT], NULL, screen, &io.select_rect_dest);

    if((io.go_rect.x != -1 && io.go_rect.y != -1) &&
        io.go_valid_flag == 1 && io.go_rect_dest.x != -1)
    {
        SDL_BlitSurface(images[IMG_ISOGO], NULL, screen, &io.go_rect_dest);
        io.go_rect.x = -1;
        io.go_rect.y = -1;
    } 
    else if((io.go_rect.x != -1 && io.go_rect.y != -1) &&
            !io.go_valid_flag && io.go_rect_dest.x != -1)
    {
        SDL_BlitSurface(images[IMG_ISOWRONG], NULL, screen, &io.go_rect_dest);
        io.go_rect.x = -1;
        io.go_rect.y = -1;
    }
   
    io.go_valid_flag = 0;

    /*Third layer: User Interface*/

    //TODO: Write a panel function to manipulate the game...
    
    dest.x = 0;
    dest.y = (screen->h / 5) * 4;
    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], NULL, screen, &dest);
    
    if(selection.selected_num != -1)
    {
        if(selection.selected_objs[0] != NULL)
        {
            dest.x = dest.x + 10;
            dest.y = dest.y + 10;
            dest.h = 100;
            dest.w = screen->w / 5;
            //FillRect(dest, 0x000000);

            dest.x = dest.x + 2;
            dest.y = dest.y + 2;

            th_ShowMessage(selection.selected_objs[0]->rname, 12, dest.x+2, dest.y+2);

            sprintf(tmp_text,"%d / %d", selection.selected_objs[0]->actual_live,
                                        selection.selected_objs[0]->live);
            //printf("dir is: %s\n", tmp_text);
            th_ShowMessage(tmp_text, 15, 
                    objects[selection.selected_objs[0]->name_enum]->w + dest.x + 10, dest.y+20);


            dest.y = dest.y + 20;

            SDL_BlitSurface(objects[selection.selected_objs[0]->name_enum], NULL, screen, &dest);
        }
    }

    dest.x = (screen->w - mini_map_image->w - 5);
    dest.y = (screen->h - mini_map_image->h - 5);
    SDL_BlitSurface(mini_map_image, NULL, screen, &dest);

    dest.x = (screen->w - images[IMG_STOP]->w - 5);
    dest.y = glyph_offset;
    SDL_BlitSurface(images[IMG_STOP], NULL, screen, &dest);
    

    /*dest.x = 20;
    dest.y = 20;
    dest.h = 100;
    dest.w = 100;

    draw_rect(screen, dest);*/

}

static void game_handle_mouse(void)
{
    th_point Pmousemap;
    th_point Pdtmap;
    th_point *path;
    int i, j;
    
    Pmousemap = mouse_map(io.Pmouse, Pscreen);
    Pdtmap = Pscreen;
    if(Pmousemap.x != -1 && Pmousemap.y != -1)
    {
   
        if( Pscreen.x < (map_image->w - screen->h) &&
            Pscreen.x > 0 &&
            Pscreen.y < (map_image->h - screen->h) &&
            Pscreen.y > 0)
        {
            // Is the mouse close to the border? Move the
            // map...
            if(io.Pmouse.x < screen_margin_in && Pscreen.x > IN_SCROLL){
                Pscreen.x = Pscreen.x - IN_SCROLL;
            }
            if(io.Pmouse.x > screen->w - screen_margin_in &&
                (Pscreen.x + screen->w) < (map_image->w - IN_SCROLL)){
                Pscreen.x = Pscreen.x + IN_SCROLL;
            }
            if(io.Pmouse.y < screen_margin_in && Pscreen.y > IN_SCROLL){
                Pscreen.y = Pscreen.y - IN_SCROLL;
            }
            if(io.Pmouse.y > screen->h - screen_margin_in &&
                    (Pscreen.y + screen->h) < (map_image->h - IN_SCROLL)){
                Pscreen.y = Pscreen.y + IN_SCROLL;
            }

            // Is the mouse VERY close to the border?
            // Move the map faster!
            if(io.Pmouse.x < screen_margin_out && Pscreen.x > OUT_SCROLL){
                Pscreen.x = Pscreen.x - OUT_SCROLL;
            }
            if(io.Pmouse.x > screen->w - screen_margin_out &&
            (Pscreen.x + screen->w) < (map_image->w - OUT_SCROLL)){
                Pscreen.x = Pscreen.x + OUT_SCROLL;
            }
            if(io.Pmouse.y < screen_margin_out && Pscreen.y > OUT_SCROLL){
                Pscreen.y = Pscreen.y - OUT_SCROLL;
            }
            if(io.Pmouse.y > screen->h - screen_margin_out &&
                    (Pscreen.y + screen->h) < (map_image->h - OUT_SCROLL)){
                Pscreen.y = Pscreen.y + OUT_SCROLL;
            }
            
        }
        if( io.select_rect.x > Pscreen.x &&
            io.select_rect.x < Pscreen.x + screen->w &&
            io.select_rect.y > Pscreen.y &&
            io.select_rect.y < Pscreen.y + screen->h)
        {
            io.select_rect_dest.x = io.select_rect.x - Pscreen.x;
            io.select_rect_dest.y = io.select_rect.y - Pscreen.y;
        }
        else
        {
            io.select_rect_dest.x = -1;
            io.select_rect_dest.y = -1;
        }
    }
    if(io.mousedown_flag != 0)
    {
        //printf("Mouse down, ... ");
        if(io.mouseclicked_flag != 0)
        {
            //io.select.x = Plclick.x;
            //io.select.y = Plclick.y; 
            Pmousemap = mouse_map(io.Plclick, Pscreen);
            if(Pmousemap.x != -1 && Pmousemap.y != -1)
            {
                printf("Mouse clicked in a valid tile!\n");
                io.mousedown_flag = 0;
                io.select_rect.x = gmaps[0][Pmousemap.x][Pmousemap.y].rect.x; 
                io.select_rect.y = gmaps[0][Pmousemap.x][Pmousemap.y].rect.y;
                io.select_xy.x = Pmousemap.x;
                io.select_xy.y = Pmousemap.y;

                // Search for a object in current selected tile and
                // select that object

                selection.selected_objs[0]=rts_get_object(0,Pmousemap);

                if(selection.selected_objs[0] != NULL)
                {
                    selection.selected_num = 0;
                    printf("Selected: %s, in (%d, %d)\n", selection.selected_objs[0]->name,
                            selection.selected_objs[0]->x,
                            selection.selected_objs[0]->y);
                }
                else
                {
                    selection.selected_num = -1;
                }
                
            }
        }
    }
    if(io.mousedownr_flag != 0)
    {
        Pmousemap = mouse_map(io.Prclick, Pscreen);
        if(Pmousemap.x != -1 && Pmousemap.y != -1)
        {
            io.go_rect.x = gmaps[0][Pmousemap.x][Pmousemap.y].rect.x; 
            io.go_rect.y = gmaps[0][Pmousemap.x][Pmousemap.y].rect.y;
            io.mousedownr_flag = 0;
            if( io.go_rect.x > Pscreen.x &&
                io.go_rect.x < Pscreen.x + screen->w &&
                io.go_rect.y > Pscreen.y &&
                io.go_rect.y < Pscreen.y + screen->h)
            {
                io.go_rect_dest.x = io.go_rect.x - Pscreen.x;
                io.go_rect_dest.y = io.go_rect.y - Pscreen.y;
                io.go_xy.x = Pmousemap.x;
                io.go_xy.y = Pmousemap.y;
                if(rts_valid_tile(0,1,io.go_xy))
                {
                    printf("Is a valid tile...\n");
                    io.go_valid_flag = 1;
                }
                else
                {
                    printf("Is a invalid tile...\n");
                    io.go_valid_flag = 0;
                }
            }
            else
            {
                io.go_rect_dest.x = -1;
                io.go_rect_dest.y = -1;
            }
        }
    }
    else
    {
        io.go_rect.x = -1;
        io.go_rect.y = -1;
        io.go_valid_flag = 0;
    }
    if( selection.selected_num != -1        && 
        selection.selected_objs[0] != NULL  &&
        io.go_valid_flag)
    {
        Pmousemap.x = selection.selected_objs[0]->x;
        Pmousemap.y = selection.selected_objs[0]->y;
        //printf("Go to: (%d, %d) No.4\n", io.go_xy.x, io.go_xy.y);
        printf("Path to go: from (%d,%d) to (%d,%d)\n", selection.selected_objs[0]->x,
                selection.selected_objs[0]->y,
                io.go_xy.x,
                io.go_xy.y);
        if(!(path = ai_shortes_path(0,0,Pmousemap, io.go_xy)))
            printf("No shortes path found or a error ocurred!\n");
        else
            printf("Path found!\n");
    }
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
        io.Pmouse.x = event.button.x;
        io.Pmouse.y = event.button.y;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            io.Plclick.x = event.button.x;
            io.Plclick.y = event.button.y;
            io.mousedown_flag = 1;
            io.mouseclicked_flag = 1;
        }

        if(event.button.button == SDL_BUTTON_RIGHT)
        {
            io.mousedownr_flag = 1;
            io.Prclick.x = event.button.x;
            io.Prclick.y = event.button.y;
        }

        key = game_mouse_event(event);
    }
    if(event.type == SDL_MOUSEBUTTONUP)
    {
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            io.mousedown_flag = 0;
        }
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



