/*
 * panel.c
 *
 * Description: GUI panel for TuxHistory
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#include<stdio.h>
#include<string.h>

#include "SDL.h"
#include "SDL_extras.h"
#include "panel.h"
#include "objects.h"
#include "tuxhistory.h"
#include "fileops.h"
#include "players.h"

void panel_draw(th_obj *select, int num)
{
    SDL_Rect dest, dest2;
    char tmp_text[100];

    //TODO: Write a panel function to manipulate the game...
    dest2.x = 0;
    dest2.y = 0;
    dest2.h = screen->h / 20;
    dest2.w = images[IMG_GUIBG_BYZANTINE]->w;

    dest.x = 0;
    dest.y = 0;

    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], &dest2, screen, &dest);
    sprintf(tmp_text,"Wood %5d   Food %5d   Stone %5d   Gold %5d ", 
                                player_vars[1].wood,
                                player_vars[1].food,
                                player_vars[1].stone,
                                player_vars[1].gold);
            
    th_ShowMessage(tmp_text, 16, dest.x+2, dest.y+2);

    dest.x = 0;
    dest.y = (screen->h / 5) * 4;
    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], NULL, screen, &dest);
    
    if(num != -1)
    {
        if(select != NULL)
        {
            dest.x = dest.x + 10;
            dest.y = dest.y + 10;
            dest.h = 100;
            dest.w = screen->w / 5;
            //FillRect(dest, 0x000000);

            dest.x = dest.x + 2;
            dest.y = dest.y + 2;

            th_ShowMessage(select->rname, 12, dest.x+2, dest.y+2);

            sprintf(tmp_text,"%d / %d", select->actual_live,
                                        select->live);
            //printf("dir is: %s\n", tmp_text);
            th_ShowMessage(tmp_text, 15, 
                    objects[select->name_enum]->w + dest.x + 10, dest.y+20);


            dest.y = dest.y + 20;

            SDL_BlitSurface(objects[select->name_enum], NULL, screen, &dest);
        }
    }

    dest.x = (screen->w - mini_map_image->w - 5);
    dest.y = (screen->h - mini_map_image->h - 5);
    SDL_BlitSurface(mini_map_image, NULL, screen, &dest);

    dest.x = (screen->w - images[IMG_STOP]->w - 5);
    dest.y = glyph_offset;
    SDL_BlitSurface(images[IMG_STOP], NULL, screen, &dest);
    
}
