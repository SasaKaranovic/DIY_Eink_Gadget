#ifndef _SK_GAME_DASHBOARD_H_
#define _SK_GAME_DASHBOARD_H_

#include "gfx_icon.h"

void screen_game_dashboard(uint8_t icon, uint16_t attack, uint16_t defense, uint16_t life, uint16_t life_max, bool display_immediately);
void screen_wound_counter(int16_t max);
void screen_dice_simple(uint8_t max);
void screen_pick_function(uint8_t nOption);
void screen_subscribe(void);
void screen_tic_tac_toe_sample(void);
#endif
