#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

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
#include "ssd1680.h"
#include "gfx.h"
#include "gfx_icon.h"
#include "screen_controller.h"
#include "screen_game_dashboard.h"

/* Prevent deep sleep (system off) from being entered on long timeouts
 * or `K_FOREVER` due to the default residency policy.
 *
 * This has to be done before anything tries to sleep, which means
 * before the threading system starts up between PRE_KERNEL_2 and
 * POST_KERNEL.  Do it at the start of PRE_KERNEL_2.
 */
static int disable_ds_1(const struct device *dev)
{
    ARG_UNUSED(dev);
    pm_policy_state_lock_get(PM_STATE_SOFT_OFF, PM_ALL_SUBSTATES);
    return 0;
}

SYS_INIT(disable_ds_1, PRE_KERNEL_2, 0);

static void reset_to_main_menu(void)
{
    LOG_INF("Resetting to setup menu");
    retained.app_state = 0;             // Reset app state to main-menu
    retained.current_selection = 1;     // Set first menu item as selected
    retained_update();                  // Update retained section
    button_state.left = false;          // Clear left button press
    button_state.middle = false;        // Clear middle button press
    button_state.right = false;         // Clear right button press
}

void main(void)
{
    // Wake up time is 16.8us so we should be able to capture button press
    // First get the GPIO state before going forward
    board_switch_init();
    board_update_button_states();

    // Initialize rest of the board
    board_gpio_init();
    gfx_initialize();

    // Validate retained data
    bool retained_ok = retained_validate();
    retained.boots += 1;
    retained_update();

    LOG_INF("Left: %s", button_state.left ? "On" : "Off");
    LOG_INF("Middle: %s", button_state.middle ? "On" : "Off");
    LOG_INF("Right: %s", button_state.right ? "On" : "Off");

    LOG_INF("Retained data: %s", retained_ok ? "valid" : "INVALID");
    LOG_INF("Boot count: %u", retained.boots);
    LOG_INF("Off count: %u", retained.off_count);
    LOG_INF("App state: %u", retained.app_state);
    LOG_INF("Selection: %u", retained.current_selection);

    // Check if middle button is held for 3+ sec
    uint8_t nMiddleCount=0;
    while( board_button_is_pressed(BUTTON_MIDDLE) )
    {
        nMiddleCount++;
        k_sleep(K_MSEC(500));
        if(nMiddleCount>=6)
        {
            reset_to_main_menu();
            break;
        }
    }

    // Update the screen based on state of the device
    screen_logic_state_tick();

    // Put screen and the device into deep sleep
    eink_deep_sleep();
    LOG_INF("Entering deep sleep mode. Night night.");
    pm_state_force(0u, &(struct pm_state_info){PM_STATE_SOFT_OFF, 0, 0});

    while (1)
    {
        k_sleep(K_MSEC(500));
    }
}
