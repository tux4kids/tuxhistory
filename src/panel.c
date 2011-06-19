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
#include "ai.h"
#include "t4kui/ui_system.h"
#include "t4kui/ui_window.h"
#include "t4kui/ui_button.h"
#include "t4kui/ui_proxywidget.h"
#include "t4kui/ui_colors.h"
#include "t4kui/ui_events.h"
#include "t4kui/ui_layouts.h"
#include "t4kui/ui_callback.h"

static int rect_coll(th_point *p, SDL_Rect *r);

int panel_init(void)
{
    int i;
    float zoom;
    SDL_Surface *tmp_surf;
    static UI_Window *window;

    window = ui_CreateWindow(0, 0, 320, 240);
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
        panel.panel_actions[i] = INACTIVE;
    }

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
    SDL_Rect dest, rect;
    char tmp_text[100];

    rect.x = 0;
    rect.y = 0;
    rect.w = images[IMG_GUIKILL]->w;
    rect.h = images[IMG_GUIKILL]->h;

    SDL_BlitSurface(images[IMG_GUIBG_BYZANTINE], &panel.panel_header_dest, screen, 
            &panel.panel_header_origin);
    sprintf(tmp_text,"Wood %5d   Food %5d   Stone %5d   Gold %5d  Population: %d/%d", 
                                player_vars[1].wood,
                                player_vars[1].food,
                                player_vars[1].stone,
                                player_vars[1].gold,
                                player_vars[human_player].pop,
                                player_vars[human_player].limit_pop);
            
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
                rect.x = objects[VILLAGE_CENTER]->w/2 - panel.panel_option[0].w/2;
                SDL_FillRect(screen, &panel.panel_option[0], 0x000000);
                SDL_BlitSurface(objects[VILLAGE_CENTER], &rect, screen, &panel.panel_option[0]);
                panel.panel_actions[0] = BUILD;
                panel.panel_actions_obj[0] = VILLAGE_CENTER;

                rect.x = objects[FARM]->w/2 - panel.panel_option[1].w/2;
                SDL_FillRect(screen, &panel.panel_option[1], 0x000000);
                SDL_BlitSurface(objects[FARM], &rect, screen, &panel.panel_option[1]);
                panel.panel_actions[1] = BUILD;
                panel.panel_actions_obj[1] = FARM;               

                rect.x = objects[HOUSE]->w/2 - panel.panel_option[2].w/2;
                SDL_FillRect(screen, &panel.panel_option[2], 0x000000);
                SDL_BlitSurface(objects[HOUSE], &rect, screen, &panel.panel_option[2]);
                panel.panel_actions[2] = BUILD;
                panel.panel_actions_obj[2] = HOUSE;

                SDL_BlitSurface(images[IMG_GUIKILL], NULL, screen, &panel.panel_option[4]);
                panel.panel_actions[4] = DIE;

                panel.panel_actions[3] = INACTIVE;
                panel.panel_actions_obj[3] = -1;               
            }
            else if(select->name_enum == VILLAGE_CENTER)
            {
                SDL_BlitSurface(images[IMG_GUIPAWN], NULL, screen, &panel.panel_option[0]);
                panel.panel_actions[0] = BUILD;
                panel.panel_actions_obj[0] = VILLAGER_MILKMAID;

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

// Return -2 if a non building option was clicked, -1 if no click in
// the panel was done, and 0+ to specify the building type to
// create.

int panel_click(th_point *point, th_obj *obj)
{
    int i;
    if(rect_coll(point, &panel.panel_header_dest))
    {
        printf("Click in header!\n");
        return -2;
    }
    else if(rect_coll(point, &panel.panel_dest))
    {
        printf("Click in panel!\n");
    
        if(rect_coll(point, &panel.panel_description))
        {
            printf("Click in the object description\n");
        }
        else if(rect_coll(point, &panel.panel_minimap))
        {
            printf("Click in minimap!\n");
        }
        else
        {
            for(i = 0; i < 5; i++)
            {
                if(rect_coll(point, &panel.panel_option[i]))
                {
                    printf("Action: %d, target %d \n", panel.panel_actions[i],
                            panel.panel_actions_obj[i]);
                    if(panel.panel_actions[i] == DIE)
                    {
                        ai_modify_state(human_player, obj, DIE);
                        return -3;
                    }
                    return panel.panel_actions_obj[i];
                    return -2;
                }
            }
        }
        return -2;
    }
    return -1;
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

