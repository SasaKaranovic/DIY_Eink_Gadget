#ifndef _SK_BOARD_H_
#define _SK_BOARD_H_

#define BOARD_GET_TICK()  k_uptime_get()
#define BOARD_DELAY(x)    k_sleep(K_MSEC(x));

typedef enum
{
    BUTTON_LEFT = 0,
    BUTTON_MIDDLE = 1,
    BUTTON_RIGHT = 2
} board_button_t;

typedef struct board_buttons_s
{
    bool left;
    bool middle;
    bool right;
} board_buttons_s;

int board_switch_init(void);
int board_gpio_init(void);
void board_led_act_toggle(void);
void board_gpio_reset(uint8_t state);
void board_gpio_data_cmd(uint8_t state);
bool board_gpio_busy(void);
bool board_button_is_pressed(board_button_t button);
void board_update_button_states(void);
int spi_transfer(uint8_t *data, uint32_t nDataLen);

extern struct board_buttons_s button_state;

#endif
