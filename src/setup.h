/*
  setup.h

  For TuxHistory 
  Contains functions to initialize the settings structs, 
  read in command-line arguments, and to clean up on exit.
  All code involving file I/O has been moved to fileops.h/fileops.c
  and is called from the main setup function.

  Some globals are declared in setup.c - all globals throughout tuxmath
  are now extern'd in the same place in tuxhistory.h

  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/


  Part of "Tux4Kids" Project
  http://www.tux4kids.org/
      
  August 26, 2001 - February 18, 2004

  Modified by David Bruce
  davidstuartbruce@gmail.com
  September 1, 2006

  And by Jesus Mager
  June 2010
*/


#ifndef SETUP_H
#define SETUP_H

#include "fileops.h"

#define MAP_HEIGHT MAX_X_TILDES * terrain[0]->h
#define MAP_WIDTH MAX_Y_TILDES * terrain[0]->w


void setup(int argc, char * argv[]);
void cleanup(void);
void cleanup_on_error(void);
extern void initialize_options_user(void);

#endif
