#include "tuxhistory.h"
#include "fileops.h"
#include "loaders.h"
#include "options.h"
#include "SDL_extras.h"

int glyph_offset;

/*****************************************************************/
/*   Loading of data files for images and sounds.                */
/*   These functions also draw some user feedback to             */
/*   display the progress of the loading.                        */
/*****************************************************************/

/* returns 1 if all data files successfully loaded, 0 otherwise. */

int load_image_data()
{
  int i, j, k;

  static char* image_filenames[NUM_IMAGES] = {
  "status/title.png",
  "status/left.png",
  "status/left_gray.png",
  "status/right.png",
  "status/right_gray.png",
  "status/tux4kids.png",
  "status/nbs.png",
  "status/nums.png",
  "status/lednums.png",
  "status/led_neg_sign.png",
  "status/paused.png",
  "status/demo.png",
  "status/demo-small.png",
  "status/keypad.png",
  "status/keypad_no_neg.png",
  "tux/console_led.png",
  "tux/console_bash.png",
  "tux/tux-console1.png",
  "tux/tux-console2.png",
  "tux/tux-console3.png",
  "tux/tux-console4.png",
  "tux/tux-relax1.png",
  "tux/tux-relax2.png",
  "tux/tux-egypt1.png",
  "tux/tux-egypt2.png",
  "tux/tux-egypt3.png",
  "tux/tux-egypt4.png",
  "tux/tux-drat.png",
  "tux/tux-yipe.png",
  "tux/tux-yay1.png",
  "tux/tux-yay2.png",
  "tux/tux-yes1.png",
  "tux/tux-yes2.png",
  "tux/tux-sit.png",
  "tux/tux-fist1.png",
  "tux/tux-fist2.png",
  "status/wave.png",
  "status/score.png",
  "status/stop.png",
  "status/numbers.png",
  "status/gameover.png",
  "status/gameover_won.png",
  };


  static char* sprite_filenames[NUM_IMAGES] = {
  "comets/comet",
  "comets/bonus_comet",
  "comets/cometex",
  "comets/bonus_cometex"
  };

  static char* terrain_filenames[NUM_TERRAINS] = {
    "terrain/highSeas/border_E_odd.png",//HIGHSEA
    "terrain/highSeas/border_SW_even.png",
    "terrain/highSeas/border_NW_odd.png",
    "terrain/highSeas/border_W_even.png",
    "terrain/highSeas/border_NE_odd.png",
    "terrain/highSeas/border_SE_even.png",
    "terrain/highSeas/border_W_odd.png",
    "terrain/highSeas/border_E_even.png",
    "terrain/highSeas/border_SW_odd.png",
    "terrain/highSeas/border_NW_even.png",
    "terrain/highSeas/center0.png",
    "terrain/highSeas/border_N_even.png",
    "terrain/highSeas/border_SE_odd.png",
    "terrain/highSeas/border_NE_even.png",
    "terrain/highSeas/border_S_odd.png",
    "terrain/highSeas/border_N_odd.png",
    "terrain/highSeas/center1.png",
    "terrain/highSeas/border_S_even.png",
    "terrain/tundra/border_E_odd.png",//TUNDRA
    "terrain/tundra/border_SW_even.png",
    "terrain/tundra/border_NW_odd.png",
    "terrain/tundra/border_W_even.png",
    "terrain/tundra/border_NE_odd.png",
    "terrain/tundra/border_SE_even.png",
    "terrain/tundra/border_W_odd.png",
    "terrain/tundra/border_E_even.png",
    "terrain/tundra/border_SW_odd.png",
    "terrain/tundra/border_NW_even.png",
    "terrain/tundra/center0.png",
    "terrain/tundra/border_N_even.png",
    "terrain/tundra/border_SE_odd.png",
    "terrain/tundra/border_NE_even.png",
    "terrain/tundra/border_S_odd.png",
    "terrain/tundra/border_N_odd.png",
    "terrain/tundra/center1.png",
    "terrain/tundra/border_S_even.png",
    "terrain/swamp/border_E_odd.png",//SWAMP
    "terrain/swamp/border_SW_even.png",
    "terrain/swamp/border_NW_odd.png",
    "terrain/swamp/border_W_even.png",
    "terrain/swamp/border_NE_odd.png",
    "terrain/swamp/border_SE_even.png",
    "terrain/swamp/border_W_odd.png",
    "terrain/swamp/border_E_even.png",
    "terrain/swamp/border_SW_odd.png",
    "terrain/swamp/border_NW_even.png",
    "terrain/swamp/center0.png",
    "terrain/swamp/border_N_even.png",
    "terrain/swamp/border_SE_odd.png",
    "terrain/swamp/border_NE_even.png",
    "terrain/swamp/border_S_odd.png",
    "terrain/swamp/border_N_odd.png",
    "terrain/swamp/center1.png",
    "terrain/swamp/border_S_even.png",
    "terrain/unexplored/border_E_odd.png",//UNEXPLORED
    "terrain/unexplored/border_SW_even.png",
    "terrain/unexplored/border_NW_odd.png",
    "terrain/unexplored/border_W_even.png",
    "terrain/unexplored/border_NE_odd.png",
    "terrain/unexplored/border_SE_even.png",
    "terrain/unexplored/border_W_odd.png",
    "terrain/unexplored/border_E_even.png",
    "terrain/unexplored/border_SW_odd.png",
    "terrain/unexplored/border_NW_even.png",
    "terrain/unexplored/center0.png",
    "terrain/unexplored/border_N_even.png",
    "terrain/unexplored/border_SE_odd.png",
    "terrain/unexplored/border_NE_even.png",
    "terrain/unexplored/border_S_odd.png",
    "terrain/unexplored/border_N_odd.png",
    "terrain/unexplored/center1.png",
    "terrain/unexplored/border_S_even.png",
    "terrain/desert/border_E_odd.png",//DESERT
    "terrain/desert/border_SW_even.png",
    "terrain/desert/border_NW_odd.png",
    "terrain/desert/border_W_even.png",
    "terrain/desert/border_NE_odd.png",
    "terrain/desert/border_SE_even.png",
    "terrain/desert/border_W_odd.png",
    "terrain/desert/border_E_even.png",
    "terrain/desert/border_SW_odd.png",
    "terrain/desert/border_NW_even.png",
    "terrain/desert/center0.png",
    "terrain/desert/border_N_even.png",
    "terrain/desert/border_SE_odd.png",
    "terrain/desert/border_NE_even.png",
    "terrain/desert/border_S_odd.png",
    "terrain/desert/border_N_odd.png",
    "terrain/desert/center1.png",
    "terrain/desert/border_S_even.png",
    "terrain/grassland/border_E_odd.png",//GRASSLAND
    "terrain/grassland/border_SW_even.png",
    "terrain/grassland/border_NW_odd.png",
    "terrain/grassland/border_W_even.png",
    "terrain/grassland/border_NE_odd.png",
    "terrain/grassland/border_SE_even.png",
    "terrain/grassland/border_W_odd.png",
    "terrain/grassland/border_E_even.png",
    "terrain/grassland/border_SW_odd.png",
    "terrain/grassland/border_NW_even.png",
    "terrain/grassland/center0.png",
    "terrain/grassland/border_N_even.png",
    "terrain/grassland/border_SE_odd.png",
    "terrain/grassland/border_NE_even.png",
    "terrain/grassland/border_S_odd.png",
    "terrain/grassland/border_N_odd.png",
    "terrain/grassland/center1.png",
    "terrain/grassland/border_S_even.png",
    "terrain/arctic/border_E_odd.png",//ARCTIC
    "terrain/arctic/border_SW_even.png",
    "terrain/arctic/border_NW_odd.png",
    "terrain/arctic/border_W_even.png",
    "terrain/arctic/border_NE_odd.png",
    "terrain/arctic/border_SE_even.png",
    "terrain/arctic/border_W_odd.png",
    "terrain/arctic/border_E_even.png",
    "terrain/arctic/border_SW_odd.png",
    "terrain/arctic/border_NW_even.png",
    "terrain/arctic/center0.png",
    "terrain/arctic/border_N_even.png",
    "terrain/arctic/border_SE_odd.png",
    "terrain/arctic/border_NE_even.png",
    "terrain/arctic/border_S_odd.png",
    "terrain/arctic/border_N_odd.png",
    "terrain/arctic/center1.png",
    "terrain/arctic/border_S_even.png",
    "terrain/ocean/border_E_odd.png",//OCEAN
    "terrain/ocean/border_SW_even.png",
    "terrain/ocean/border_NW_odd.png",
    "terrain/ocean/border_W_even.png",
    "terrain/ocean/border_NE_odd.png",
    "terrain/ocean/border_SE_even.png",
    "terrain/ocean/border_W_odd.png",
    "terrain/ocean/border_E_even.png",
    "terrain/ocean/border_SW_odd.png",
    "terrain/ocean/border_NW_even.png",
    "terrain/ocean/center0.png",
    "terrain/ocean/border_N_even.png",
    "terrain/ocean/border_SE_odd.png",
    "terrain/ocean/border_NE_even.png",
    "terrain/ocean/border_S_odd.png",
    "terrain/ocean/border_N_odd.png",
    "terrain/ocean/center1.png",
    "terrain/ocean/border_S_even.png"
    "terrain/marsh/border_E_odd.png",//MARSH
    "terrain/marsh/border_SW_even.png",
    "terrain/marsh/border_NW_odd.png",
    "terrain/marsh/border_W_even.png",
    "terrain/marsh/border_NE_odd.png",
    "terrain/marsh/border_SE_even.png",
    "terrain/marsh/border_W_odd.png",
    "terrain/marsh/border_E_even.png",
    "terrain/marsh/border_SW_odd.png",
    "terrain/marsh/border_NW_even.png",
    "terrain/marsh/center0.png",
    "terrain/marsh/border_N_even.png",
    "terrain/marsh/border_SE_odd.png",
    "terrain/marsh/border_NE_even.png",
    "terrain/marsh/border_S_odd.png",
    "terrain/marsh/border_N_odd.png",
    "terrain/marsh/center1.png",
    "terrain/marsh/border_S_even.png",
    "terrain/savannah/border_E_odd.png",//SAVANNAH
    "terrain/savannah/border_SW_even.png",
    "terrain/savannah/border_NW_odd.png",
    "terrain/savannah/border_W_even.png",
    "terrain/savannah/border_NE_odd.png",
    "terrain/savannah/border_SE_even.png",
    "terrain/savannah/border_W_odd.png",
    "terrain/savannah/border_E_even.png",
    "terrain/savannah/border_SW_odd.png",
    "terrain/savannah/border_NW_even.png",
    "terrain/savannah/center0.png",
    "terrain/savannah/border_N_even.png",
    "terrain/savannah/border_SE_odd.png",
    "terrain/savannah/border_NE_even.png",
    "terrain/savannah/border_S_odd.png",
    "terrain/savannah/border_N_odd.png",
    "terrain/savannah/center1.png",
    "terrain/savannah/border_S_even.png",
    "terrain/plains/border_E_odd.png",//PLAINS
    "terrain/plains/border_SW_even.png",
    "terrain/plains/border_NW_odd.png",
    "terrain/plains/border_W_even.png",
    "terrain/plains/border_NE_odd.png",
    "terrain/plains/border_SE_even.png",
    "terrain/plains/border_W_odd.png",
    "terrain/plains/border_E_even.png",
    "terrain/plains/border_SW_odd.png",
    "terrain/plains/border_NW_even.png",
    "terrain/plains/center0.png",
    "terrain/plains/border_N_even.png",
    "terrain/plains/border_SE_odd.png",
    "terrain/plains/border_NE_even.png",
    "terrain/plains/border_S_odd.png",
    "terrain/plains/border_N_odd.png",
    "terrain/plains/center1.png",
    "terrain/plains/border_S_even.png",
    "terrain/prairie/border_E_odd.png",//PRAIRIE
    "terrain/prairie/border_SW_even.png",
    "terrain/prairie/border_NW_odd.png",
    "terrain/prairie/border_W_even.png",
    "terrain/prairie/border_NE_odd.png",
    "terrain/prairie/border_SE_even.png",
    "terrain/prairie/border_W_odd.png",
    "terrain/prairie/border_E_even.png",
    "terrain/prairie/border_SW_odd.png",
    "terrain/prairie/border_NW_even.png",
    "terrain/prairie/center0.png",
    "terrain/prairie/border_N_even.png",
    "terrain/prairie/border_SE_odd.png",
    "terrain/prairie/border_NE_even.png",
    "terrain/prairie/border_S_odd.png",
    "terrain/prairie/border_N_odd.png",
    "terrain/prairie/center1.png",
    "terrain/prairie/border_S_even.png"
  };


  /* Load static images: */
  for (i = 0; i < NUM_IMAGES; i++)
  {
    images[i] = LoadImage(image_filenames[i], IMG_ALPHA);

    if (images[i] == NULL)
    {
      fprintf(stderr,
              "\nError: I couldn't load a graphics file:\n"
              "%s\n"
              "The Simple DirectMedia error that occured was:\n"
              "%s\n\n", image_filenames[i], SDL_GetError());
      return 0;
    }
  }

  /* Load animated graphics: */
  for (i = 0; i < NUM_SPRITES; i++)
  {
    sprites[i] = LoadSprite(sprite_filenames[i], IMG_ALPHA);

    if (sprites[i] == NULL)
    {
      fprintf(stderr,
              "\nError: I couldn't load a graphics file:\n"
              "%s\n"
              "The Simple DirectMedia error that occured was:\n"
              "%s\n\n", sprite_filenames[i], SDL_GetError());
      return 0;
    }
  }

  /* Load terrain tildes*/

  j = 0;
  k = 0;
  for (i = 0; i < NUM_TERRAINS; i++)
  {
    terrain[i] = LoadImage(terrain_filenames[i], IMG_ALPHA);
    /*terrain[j][k] = LoadImage(terrain_filenames[i], IMG_ALPHA);
    if (++k > NUM_COMPTILDE)
    {
        k=0;
        j++;
    }
    if (j > NUM_OF_TERRAINS)
    {
        break;
    }*/
   }



  glyph_offset = 0;

#ifdef REPLACE_WAVESCORE
  /* Replace the "WAVE" and "SCORE" with translate-able versions */
  SDL_FreeSurface(images[IMG_WAVE]);
  images[IMG_WAVE] = SimpleTextWithOffset(_("WAVE"), 28, &white, &glyph_offset);
  SDL_FreeSurface(images[IMG_SCORE]);
  images[IMG_SCORE] = SimpleTextWithOffset(_("SCORE"), 28, &white, &glyph_offset);
  glyph_offset++;
#endif

  /* If we make it to here OK, return 1: */
  return 1;
}





#ifndef NOSOUND
int load_sound_data(void)
{
  int i = 0;

  static char* sound_filenames[NUM_SOUNDS] = {
  DATA_PREFIX "/sounds/harp.wav",
  DATA_PREFIX "/sounds/pop.wav",
  DATA_PREFIX "/sounds/tock.wav",
  DATA_PREFIX "/sounds/laser.wav",
  DATA_PREFIX "/sounds/buzz.wav",
  DATA_PREFIX "/sounds/alarm.wav",
  DATA_PREFIX "/sounds/shieldsdown.wav",
  DATA_PREFIX "/sounds/explosion.wav",
  DATA_PREFIX "/sounds/sizzling.wav",
  DATA_PREFIX "/sounds/towerclock.wav",
  DATA_PREFIX "/sounds/cheer.wav"
  };

  static char* music_filenames[NUM_MUSICS] = {
  DATA_PREFIX "/sounds/game.mod",
  DATA_PREFIX "/sounds/game2.mod",
  DATA_PREFIX "/sounds/game3.mod"
  };

  /* skip loading sound files if sound system not available: */
  if (Opts_UsingSound())
  {
    for (i = 0; i < NUM_SOUNDS; i++)
    {
      sounds[i] = Mix_LoadWAV(sound_filenames[i]);

      if (sounds[i] == NULL)
      {
        fprintf(stderr,
                "\nError: I couldn't load a sound file:\n"
                "%s\n"
                "The Simple DirectMedia error that occured was:\n"
                "%s\n\n", sound_filenames[i], SDL_GetError());
        return 0;
      }
    }


    for (i = 0; i < NUM_MUSICS; i++)
    {
      musics[i] = Mix_LoadMUS(music_filenames[i]);

      if (musics[i] == NULL)
      {
        fprintf(stderr,
                "\nError: I couldn't load a music file:\n"
                "%s\n"
                "The Simple DirectMedia error that occured was:\n"
                "%s\n\n", music_filenames[i], SDL_GetError());
        return 0;
      }

    }
  }
  return 1;
}

#endif /* NOSOUND */
