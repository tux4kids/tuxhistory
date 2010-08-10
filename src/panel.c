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
#include "SDL_rotozoom.h"
#include "SDL_extras.h"
#include "panel.h"
#include "objects.h"
#include "tuxhistory.h"
#include "setup.h"
#include "fileops.h"
#include "players.h"


int panel_init(void)
{
    int i;
    float zoom;
    SDL_Surface *tmp_surf;

    panel.panel_dest.x = 0;
    panel.panel_dest.y = (screen->h / 5) * 4;
    panel.panel_dest.w = screen->w;
    panel.panel_dest.h = screen->h / 5;

    panel.panel_description.x = panel.panel_dest.x + 10;
    panel.panel_description.y = panel.panel_dest.y + 10;
    panel.panel_description.w = panel.panel_dest.w / 7;
    panel.panel_description.h = (panel.panel_dest.h / 5) * 4;

    for(i = 0; i < 5; i++)
    {
        panel.panel_option[i].x = panel.panel_description.x + 
            panel.panel_description.w + 10 + (images[IMG_GUIBUILD]->w+10)*i;
        panel.panel_option[i].y = panel.panel_description.y;
        panel.panel_option[i].w = images[IMG_GUIBUILD]->w;
        panel.panel_option[i].h = images[IMG_GUIBUILD]->h;
    }

    panel.panel_actions[0] = INACTIVE;
    panel.panel_actions[1] = INACTIVE;
    panel.panel_actions[2] = INACTIVE;
    panel.panel_actions[3] = INACTIVE;
    panel.panel_actions[4] = INACTIVE;

    panel.panel_header_dest.x = 0;
    panel.panel_header_dest.y = 0;
    panel.panel_header_dest.w = screen->w;
    panel.panel_header_dest.h = screen->h/20;

    panel.panel_header_origin.x = 0;
    panel.panel_header_origin.y = 0;
    panel.panel_header_origin.w = screen->w / 20;
    panel.panel_header_origin.h = images[IMG_GUIBG_BYZANTINE]->w;

    panel.panel_game.x = 0;
    panel.panel_game.y = screen->h/20;
    panel.panel_game.w = screen->w;
    panel.panel_game.h = (screen->h / 5) * 4;

    panel.panel_minimap.x = (screen->w - mini_map_image->w - 5);
    panel.panel_minimap.y = (screen->h - mini_map_image->h - 5);
    panel.panel_minimap.x = mini_map_image->w;
    panel.panel_minimap.y = mini_map_image->h;

    zoom = (float)screen->w/(float)images[IMG_GUIBG_BYZANTINE]->w;

    //rotozoomSurface (SDL_Surface *src, double angle, double zoom, int smooth);
    tmp_surf = rotozoomSurface(images[IMG_GUIBG_BYZANTINE], 0, zoom, 1);

    if (tmp_surf == NULL)
    {
      fprintf(stderr,
              "\nError: Zoom of GUI Backgrund not possible\n");
      return 0;
    }

    SDL_FreeSurface(images[IMG_GUIBG_BYZANTINE]);
    images[IMG_GUIBG_BYZANTINE] = tmp_surf;


    return 1;

}

// TODO: This is all hard coded. In a future we need make this menu 
// from the object xml file info!
void panel_draw(th_obj *select, int num)
{
    SDL_Rect dest, dest2;
    char tmp_text[100];
    int menu_y =(screen->h / 5) * 4;

    //TODO: Write a panel function to manipulate the game...


    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], &panel.panel_header_dest, screen, 
            &panel.panel_header_origin);
    sprintf(tmp_text,"Wood %5d   Food %5d   Stone %5d   Gold %5d ", 
                                player_vars[1].wood,
                                player_vars[1].food,
                                player_vars[1].stone,
                                player_vars[1].gold);
            
    th_ShowMessage(tmp_text, 16, panel.panel_header_dest.x+2, panel.panel_header_dest.y+2);


    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], NULL, screen, &panel.panel_dest);
    
    if(num != -1)
    {
        if(select != NULL)
        {
            FillRect(panel.panel_description, 0x000000);

            th_ShowMessage(select->rname, 12, panel.panel_description.x+2, 
                    panel.panel_description.y+2);
            sprintf(tmp_text,"%d / %d", select->actual_live,
                                        select->live);
            th_ShowMessage(tmp_text, 15, 
                    objects[select->name_enum]->w + panel.panel_description.x + 10, 
                    panel.panel_description.y+20);

            dest.x = panel.panel_description.x + 5;
            dest.y = panel.panel_description.y + 20;

            SDL_BlitSurface(objects[select->name_enum], NULL, screen, &dest);
            
            if(select->name_enum == VILLAGER_MILKMAID)
            {
                SDL_BlitSurface(images[IMG_GUIBUILD], NULL, screen, &panel.panel_option[0]);
                panel.panel_actions[0] = BUILD;

                SDL_BlitSurface(images[IMG_GUIKILL], NULL, screen, &panel.panel_option[4]);
                panel.panel_actions[4] = DIE;

                panel.panel_actions[1] = INACTIVE;
                panel.panel_actions[2] = INACTIVE;  
                panel.panel_actions[3] = INACTIVE;
            }
            else if(select->name_enum == VILLAGE_CENTER)
            {
                SDL_BlitSurface(images[IMG_GUIPAWN], NULL, screen, &panel.panel_option[0]);
                panel.panel_actions[0] = BUILD;

                SDL_BlitSurface(images[IMG_GUIKILL], NULL, screen, &panel.panel_option[4]);
                panel.panel_actions[4] = DIE;

                panel.panel_actions[1] = INACTIVE;
                panel.panel_actions[2] = INACTIVE;  
                panel.panel_actions[3] = INACTIVE;  
            }
            else if(select->name_enum == HOUSE)
            {
                SDL_BlitSurface(images[IMG_GUIKILL], NULL, screen, &panel.panel_option[4]);
                panel.panel_actions[4] = DIE;

                panel.panel_actions[0] = INACTIVE;
                panel.panel_actions[1] = INACTIVE;
                panel.panel_actions[2] = INACTIVE;  
                panel.panel_actions[3] = INACTIVE;  
    
            }
        }
    }

    dest.x = (screen->w - mini_map_image->w - 5);
    dest.y = (screen->h - mini_map_image->h - 5);
    SDL_BlitSurface(mini_map_image, NULL, screen, &dest);

    dest.x = (screen->w - images[IMG_STOP]->w - 5);
    dest.y = glyph_offset;
    SDL_BlitSurface(images[IMG_STOP], NULL, screen, &dest);
}

static int rect_coll(th_point *p, SDL_Rect *r)
{
    if( p->x >= r->x        &&
        p->x <= r->x + r->w &&
        p->y >= r->y        &&
        p->y <= r->y + r->h )
        return 1;
    else
        return 0;
}



int panel_click(th_point *point)
{
    if(rect_coll(point, &panel.panel_header_dest))
    {
        printf("Click in header!\n");
        return 1;
    }
    else if(rect_coll(point, &panel.panel_dest))
    {
        printf("Click in panel!\n");
    
        if(rect_coll(point, &panel.panel_description))
        {
            printf("Click in the object description\n");
        }
        if(rect_coll(point, &panel.panel_minimap))
        {
            printf("Click in minimap!\n");
        }
        return 1;
    }
    return 0;
}


int panel_rclick(th_point *point)
{
    if(rect_coll(point, &panel.panel_header_dest))
    {
        printf("Right Click in header!\n");
        return 1;
    }
    else if(rect_coll(point, &panel.panel_dest))
    {
        printf("Right Click in panel!\n");
        return 1;
    }
    return 0;
}

