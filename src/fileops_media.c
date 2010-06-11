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

/* TODO load only "igloo" or "city" files, not both.             */
/* TODO get rid of files no longer used.                         */

int load_image_data()
{
  int i;

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
