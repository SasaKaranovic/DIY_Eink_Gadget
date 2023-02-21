#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(scrlog, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>
#include <soc.h>
#include "retained.h"
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "gfx.h"
#include "gfx_icon.h"
#include "screen_controller.h"
#include "screen_game_dashboard.h"

// Private function forward declaration
static void screen_logic_setup(void);
static void screen_logic_health_dashboard(void);

#define SELECT_NUM_OPTIONS   4

void screen_logic_state_tick(void)
{
    // Check what is the device state, this indicates how we should process
    // button presses and other events
    switch (retained.app_state)
    {
        case 0:
            LOG_INF("Setup screen");
            screen_logic_setup();
        break;

        case 1:
            LOG_INF("One D6");
            screen_dice_simple(6);
        break;

        case 2:
            LOG_INF("Two D6");
            screen_dice_simple(12);
        break;

        case 3:
            LOG_INF("One D20");
            screen_dice_simple(20);
        break;

        case 4:
            LOG_INF("Health dashboard");
            screen_logic_health_dashboard();
        break;

        default:
        break;
    }
}


static void screen_logic_setup(void)
{
    if(button_state.left)
    {
        retained.current_selection++;
    }
    else if (button_state.right)
    {
        if(retained.current_selection > 0)
        {
            retained.current_selection--;
        }
        else
        {
            retained.current_selection = SELECT_NUM_OPTIONS;
        }
    }

    if (retained.current_selection <= 0)
    {
        retained.current_selection=SELECT_NUM_OPTIONS;
    }
    else if (retained.current_selection > SELECT_NUM_OPTIONS)
    {
        retained.current_selection=1;
    }
    retained_update();

    if (button_state.middle)
    {
        retained.app_state = retained.current_selection;  // Zero is setup screen
        retained_update();

        button_state.left = false;
        button_state.middle = false;
        button_state.right = false;
        screen_logic_state_tick();
    }
    else
    {
        screen_pick_function(retained.current_selection);

    }

    retained_update();
}



static void screen_logic_health_dashboard(void)
{
    if(button_state.left)
    {
        screen_game_dashboard(0, 1, 2, 3, 100, true);
    }
    else if (button_state.middle)
    {
        screen_game_dashboard(0, 2, 3, 4, 100, true);
    }
    else if (button_state.right)
    {
        screen_game_dashboard(0, 3, 4, 5, 100, true);
    }
    else
    {
        screen_game_dashboard(0, 0, 0, 0, 100, true);
    }
}
