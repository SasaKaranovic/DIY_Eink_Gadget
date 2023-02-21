#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sgd, LOG_LEVEL_INF);

#include <stdio.h>

#include <syscalls/rand32.h>

#include "screen_game_dashboard.h"
#include "gfx.h"
#include "gfx_font.h"
#include "gfx_icon.h"

// Icons we will use
#include "icons/sword.h"
#include "icons/shield.h"
#include "icons/heart.h"
#include "icons/minion.h"
#include "icons/agent.h"
#include "icons/dice_single.h"
#include "icons/dice_multi.h"
#include "icons/dice_20.h"

// Public variables
char str_buff[32] = {0};

static void draw_icon_and_value(const gfx_icon_typedef *icon, uint8_t x, uint8_t y, EPD_Color color, gfx_mirror_t mirror, gfx_rotate_t rotate, uint8_t spacing, int16_t value, int16_t value_max)
{
    if (value_max > 0)
    {
        sprintf(str_buff, "%d/%d", value, value_max);
    }
    else
    {
        sprintf(str_buff, "%d", value);
    }

    gfx_set_font(10);
    gfx_draw_icon(icon, x, y, color, mirror, rotate);
    gfx_draw_text(icon->width + spacing, y + icon->height/2 + 20, EPD_BLACK, EPD_WHITE, 1,1, (const char *)str_buff);
}

void screen_game_dashboard(uint8_t icon, uint16_t attack, uint16_t defense, uint16_t life, uint16_t life_max, bool display_immediately)
{
    // XY cursor to help us draw things
    uint8_t dashboard_x = 0;
    uint8_t dashboard_y = 0;

    gfx_clear_buffer();

    dashboard_x = 4;
    dashboard_y = 4;

    // Attack
    draw_icon_and_value(&icon_sword_bwr_png, dashboard_x, dashboard_y, EPD_BLACK, MIRROR_XY, ROTATE_NONE, 10, attack, -1);
    gfx_draw_icon(&icon_red_sword_bwr_png, dashboard_x, dashboard_y, EPD_RED, MIRROR_NONE, ROTATE_CCW);
    dashboard_y += 62;

    // Armor
    draw_icon_and_value(&icon_shield_bwr_png, dashboard_x, dashboard_y, EPD_BLACK, MIRROR_NONE, ROTATE_CCW, 10, defense, -1);
    gfx_draw_icon(&icon_red_shield_bwr_png, dashboard_x, dashboard_y, EPD_RED, MIRROR_NONE, ROTATE_CCW);
    dashboard_y += 62;

    // Health
    draw_icon_and_value(&icon_heart_bwr_png, dashboard_x, dashboard_y, EPD_BLACK, MIRROR_NONE, ROTATE_CCW, 10, life, life_max);
    gfx_draw_icon(&icon_red_heart_bwr_png, dashboard_x, dashboard_y, EPD_RED, MIRROR_NONE, ROTATE_CCW);

    // Avatar
    dashboard_x = 110;
    dashboard_y = 20;

    if (icon == 0)
    {
        gfx_draw_icon(&icon_agent_bwr_png, dashboard_x, dashboard_y, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
        gfx_draw_icon(&icon_red_agent_bwr_png, dashboard_x, dashboard_y, EPD_RED, MIRROR_NONE, ROTATE_CCW);
    }
    else
    {
        gfx_draw_icon(&icon_minion_png, dashboard_x, dashboard_y, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
    }

    if(display_immediately)
    {
        gfx_display();
    }
}


void screen_dice_simple(uint8_t max)
{
    gfx_clear_buffer();

    const int16_t middle = (EPD_WIDTH-1)/2;
    uint32_t random_value = sys_rand32_get();
    random_value = random_value % max;

    // random value will start from 0, we want to start from 1
    if(random_value == 0)
    {
        random_value += 1;
    }

    sprintf(str_buff, "You have rolled");

    gfx_set_font(FREE_SANS_12PT);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 32, EPD_BLACK, EPD_WHITE, 1,1, str_buff);


    // Draw simple dice
    gfx_draw_fill_round_rect(middle-36, 60, 72, 72, 4, EPD_RED);
    gfx_draw_fill_round_rect(middle-32, 64, 64, 64, 4, EPD_RED_INVERSE);
    gfx_draw_fill_round_rect(middle-32, 64, 64, 64, 4, EPD_BLACK);

    gfx_set_font(FREE_SANS_24PT);
    sprintf(str_buff, "%d", random_value);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 110, EPD_WHITE, EPD_BLACK, 1,1, str_buff);

    gfx_set_font(FREE_SANS_9PT);
    sprintf(str_buff, "on a %d sided dice", max);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 170, EPD_BLACK, EPD_WHITE, 1,1, str_buff);

    gfx_display();
}

void screen_wound_counter(int16_t max)
{
    uint32_t random_value = sys_rand32_get();
    gfx_clear_buffer();

    const int16_t middle = (EPD_WIDTH-1)/2;

    sprintf(str_buff, "You have rolled");

    gfx_set_font(FREE_SANS_12PT);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 32, EPD_BLACK, EPD_WHITE, 1,1, str_buff);


    // Draw simple dice
    gfx_draw_fill_round_rect(middle-36, 60, 72, 72, 4, EPD_RED);
    gfx_draw_fill_round_rect(middle-32, 64, 64, 64, 4, EPD_RED_INVERSE);
    gfx_draw_fill_round_rect(middle-32, 64, 64, 64, 4, EPD_BLACK);

    gfx_set_font(FREE_SANS_24PT);
    sprintf(str_buff, "%d", random_value);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 110, EPD_WHITE, EPD_BLACK, 1,1, str_buff);

    gfx_set_font(FREE_SANS_9PT);
    sprintf(str_buff, "on a %d sided dice", max);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 170, EPD_BLACK, EPD_WHITE, 1,1, str_buff);

    gfx_display();
}


void screen_pick_function(uint8_t nOption)
{
    gfx_clear_buffer();

    sprintf(str_buff, "Function like");
    gfx_set_font(FREE_SANS_12PT);
    gfx_draw_centered_text((EPD_WIDTH-1)/2, 32, EPD_BLACK, EPD_WHITE, 1,1, str_buff);

    if(nOption == 1)
    {
        sprintf(str_buff, "D6 Dice");
        gfx_draw_icon(&icon_dice_multi_png, 67, 68, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
        gfx_draw_centered_text((EPD_WIDTH-1)/2, 170, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    }
    else if(nOption == 2)
    {
        sprintf(str_buff, "Two D6 Dice");
        gfx_draw_icon(&icon_dice_multi_png, 67, 68, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
        gfx_draw_centered_text((EPD_WIDTH-1)/2, 170, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    }
    else if(nOption == 3)
    {
        sprintf(str_buff, "D20 Dice");
        gfx_draw_icon(&icon_dice_20_png, 67, 68, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
        gfx_draw_centered_text((EPD_WIDTH-1)/2, 170, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    }
    else if(nOption == 4)
    {
        sprintf(str_buff, "Health dashboard");
        gfx_draw_icon(&icon_agent_bwr_png, 51, 52, EPD_BLACK, MIRROR_NONE, ROTATE_CCW);
        gfx_draw_centered_text((EPD_WIDTH-1)/2, 180, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    }

    gfx_display();
}


void screen_tic_tac_toe_sample(void)
{
    gfx_clear_buffer();

    // Draw horizontal lines
    gfx_draw_h_line(15, (EPD_HEIGHT/2)-25-5, 170, EPD_BLACK);
    gfx_draw_h_line(15, (EPD_HEIGHT/2)+25+5, 170, EPD_BLACK);

    // Draw vertical lines
    gfx_draw_v_line( ((EPD_WIDTH-1)/2)-25-5, 15, 170, EPD_BLACK);
    gfx_draw_v_line( ((EPD_WIDTH-1)/2)+25+5, 15, 170, EPD_BLACK);

    gfx_set_font(FREE_SANS_18PT);
    sprintf(str_buff, "X");
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)+60, (EPD_HEIGHT/2)-50, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)-60, (EPD_HEIGHT/2)-50, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)-60, (EPD_HEIGHT/2)+70, EPD_BLACK, EPD_WHITE, 1,1, str_buff);

    sprintf(str_buff, "O");
    gfx_draw_centered_text(((EPD_WIDTH-1)/2),    (EPD_HEIGHT/2)+10, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)+60, (EPD_HEIGHT/2)+70, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)-60, (EPD_HEIGHT/2)+10, EPD_BLACK, EPD_WHITE, 1,1, str_buff);
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)+60, (EPD_HEIGHT/2)+70, EPD_BLACK, EPD_WHITE, 1,1, str_buff);


    gfx_display();
}


void screen_subscribe(void)
{
    gfx_clear_buffer();

    // Subscribe button - Border
    gfx_draw_fill_round_rect(10, (EPD_HEIGHT/2)-30, 180, 60, 8, EPD_BLACK);

    // Play button - border
    gfx_draw_fill_round_rect(20, (EPD_HEIGHT/2)-20, 40, 40, 8, EPD_RED);

    // Play button - rectangle
    gfx_draw_fill_triangle(30, (EPD_HEIGHT/2)-10, 30, (EPD_HEIGHT/2)+10, 50, (EPD_HEIGHT/2), EPD_RED_INVERSE);
    gfx_draw_fill_triangle(30, (EPD_HEIGHT/2)-10, 30, (EPD_HEIGHT/2)+10, 50, (EPD_HEIGHT/2), EPD_WHITE);

    // Subscribe text
    gfx_set_font(FREE_SANS_9PT);
    sprintf(str_buff, "Subscribe");
    gfx_draw_centered_text(((EPD_WIDTH-1)/2)+20, (EPD_HEIGHT/2)+5, EPD_WHITE, EPD_BLACK, 1,1, str_buff);

    sprintf(str_buff, "SasaKaranovic.com");
    gfx_draw_centered_text(((EPD_WIDTH-1)/2), 180, EPD_BLACK, EPD_WHITE, 1,1, str_buff);

    gfx_display();
}

