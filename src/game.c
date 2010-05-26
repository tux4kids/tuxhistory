/*
  game.c

  For TuxMath
  The main game loop!

  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/


  Part of "Tux4Kids" Project
  http://www.tux4kids.org/

  August 26, 2001 - February 18, 2004

  Revised by David Bruce, Tim Holy and others
  2005-2009
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


#define FPS 15                     /* 15 frames per second */
#define MS_PER_FRAME (1000 / FPS)

static SDL_Surface* bkgd = NULL; //640x480 background (windowed)
static SDL_Surface* scaled_bkgd = NULL; //native resolution (fullscreen)


static SDL_Surface* current_bkgd()
  { 
      return screen->flags & SDL_FULLSCREEN ? scaled_bkgd : bkgd; 
  }

int game(void)
{
  Uint32 timer = 0;

  DEBUGMSG(debug_game, "Entering game():\n");

  //see if the option matches the actual screen
  if (Opts_GetGlobalOpt(FULLSCREEN) == !(screen->flags & SDL_FULLSCREEN) )
  {
    ;//SwitchScreenMode();  //Huh??
  }

}
