
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(board, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <hal/nrf_gpio.h>
#include <stdio.h>
#include <string.h>
#include "board.h"

#define SPIBB_NODE  DT_NODELABEL(spibb0)
static const struct device *const dev_spibb = DEVICE_DT_GET(SPIBB_NODE);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define EINK_RESET DT_ALIAS(einkreset)
static const struct gpio_dt_spec dev_eink_reset = GPIO_DT_SPEC_GET(EINK_RESET, gpios);

#define EINK_BUSY DT_ALIAS(einkbusy)
static const struct gpio_dt_spec dev_eink_busy = GPIO_DT_SPEC_GET(EINK_BUSY, gpios);

#define EINK_DC DT_ALIAS(einkdc)
static const struct gpio_dt_spec dev_eink_dc = GPIO_DT_SPEC_GET(EINK_DC, gpios);


struct spi_cs_control cs_ctrl = (struct spi_cs_control){
    .gpio = GPIO_DT_SPEC_GET(SPIBB_NODE, cs_gpios),
    .delay = 0u,
};


// Buttons
#define SW_COUNT    3
static const struct gpio_dt_spec buttons[SW_COUNT] = {
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios, {0}),
    GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw2), gpios, {0}),
};

struct board_buttons_s button_state;

int board_switch_init(void)
{
    int ret;
    // Buttons
    for (uint8_t i=0; i<SW_COUNT; i++)
    {
        if (!device_is_ready(buttons[i].port)) {
            printk("Error: button device %s is not ready\n",
                   buttons[i].port->name);
            return -1;
        }

        ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT | GPIO_PULL_UP);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n",
                   ret, buttons[i].port->name, buttons[i].pin);
            return ret;
        }
    }

    return ret;
}

int board_gpio_init(void)
{
    LOG_DBG("Setting up board");
    int ret;

    // EINK
    LOG_DBG("Setting up EINK display");
    ret = gpio_pin_configure_dt(&dev_eink_busy, GPIO_INPUT);
    if (ret < 0) {
        return ret;
    }
    ret = gpio_pin_configure_dt(&dev_eink_reset, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return ret;
    }
    ret = gpio_pin_configure_dt(&dev_eink_dc, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return ret;
    }

    // LED
    LOG_DBG("Setting up activity LED");
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return ret;
    }


    for (uint8_t i=0; i<SW_COUNT; i++)
    {
        /* Configure to generate PORT event (wakeup) on button press. */
        nrf_gpio_cfg_input(buttons[i].pin, NRF_GPIO_PIN_PULLUP);
        nrf_gpio_cfg_sense_set(buttons[i].pin, NRF_GPIO_PIN_SENSE_LOW);
    }


    return 0;
}

bool board_button_is_pressed(board_button_t button)
{
    int val=0;
    val = gpio_pin_get_dt(&buttons[button]);

    if (val)
    {
        return true;
    }
    return false;
}

void board_update_button_states(void)
{
    button_state.left   = board_button_is_pressed(BUTTON_LEFT);
    button_state.middle = board_button_is_pressed(BUTTON_MIDDLE);
    button_state.right  = board_button_is_pressed(BUTTON_RIGHT);
}

void board_led_act_toggle(void)
{
    gpio_pin_toggle_dt(&led);
}

void board_gpio_reset(uint8_t state)
{
    gpio_pin_set_dt(&dev_eink_reset, state);
}
void board_gpio_data_cmd(uint8_t state)
{
    gpio_pin_set_dt(&dev_eink_dc, state);
}

bool board_gpio_busy(void)
{
    if (gpio_pin_get_dt(&dev_eink_busy)==1)
    {
        return true;
    }
    return false;
}



int spi_transfer(uint8_t *data, uint32_t nDataLen)
{
    int ret;
    struct spi_config config;

    config.frequency = 1000000;
    config.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8);
    config.slave = 0;
    config.cs = &cs_ctrl;

    struct spi_buf tx_buf[1] = {
        {.buf = data, .len = nDataLen},
    };

    struct spi_buf_set tx_set = { .buffers = tx_buf, .count = nDataLen };

    ret = spi_write(dev_spibb, &config, &tx_set);

    return ret;
}
