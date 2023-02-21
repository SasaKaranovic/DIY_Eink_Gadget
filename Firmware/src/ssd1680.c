#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ssd1680, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include "board.h"
#include "ssd1680.h"

void eink_initialize(bool reset)
{

    if (reset)
    {
        // Setup reset pin direction
        // VDD (3.3V) goes high at start, lets just chill for a ms
        board_gpio_reset(0);
        BOARD_DELAY(100);
        // bring reset low
        board_gpio_reset(1);
        // wait 10ms
        BOARD_DELAY(100);
        // bring out of reset
        board_gpio_reset(0);
        BOARD_DELAY(100);
    }

    eink_readBusy();
    eink_sendCmd(0x12); //SWRESET
    eink_readBusy();

    eink_sendCmd(0x01); //Driver output control
    // eink_sendData(0xF9);
    eink_sendData(0xC7);
    eink_sendData(0x00);
    eink_sendData(0x00);

    eink_sendCmd(0x11); //data entry mode
    eink_sendData(0x01);

    eink_sendCmd(0x44); //set Ram-X address start/end position
    eink_sendData(0x00);
    eink_sendData(0x18); //0x18-->(24+1)*8=200

    eink_sendCmd(0x45);  //set Ram-Y address start/end position
    eink_sendData(0xC7); //0xC7-->(199+1)=200
    eink_sendData(0x00);
    eink_sendData(0x00);
    eink_sendData(0x00);

    eink_sendCmd(0x3C); //BorderWavefrom
    // eink_sendData(0x01);
    eink_sendData(0x05);

    // eink_sendCmd(0x21); //  Display update control
    // eink_sendData(0x00);
    // eink_sendData(0x00);

    eink_sendCmd(0x18); //Read built-in temperature sensor
    eink_sendData(0x80);

    eink_sendCmd(0x4E); // set RAM x address count to 0;
    eink_sendData(0x00);
    eink_sendCmd(0x4F); // set RAM y address count to 0X199;
    // eink_sendData(0xF9);
    eink_sendData(0xC7);
    eink_sendData(0x00);
    eink_readBusy();
}

void eink_display(uint8_t *bw, uint8_t *red)
{
    bool display_update = false;

    eink_set_cursor(0,0);

    if(bw != NULL)
    {
        eink_sendCmd(0x24); //write RAM for black(0)/white (1)
        for (uint16_t i = 0; i < EPD_BUF_LEN; i++)
        {
            eink_sendData(bw[i]);
        }
        display_update = true;
    }

    if(red != NULL)
    {
        eink_sendCmd(0x26); //write RAM for red(1)/white (0)
        for (uint16_t i = 0; i < EPD_BUF_LEN; i++)
        {
            eink_sendData(red[i]);
        }
        display_update = true;
    }

    if (display_update)
    {
        eink_update();
    }
}

void eink_display_partial(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
    unsigned int i;
    unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
    x_start=x_start/8;
    x_end=x_start+PART_LINE/8-1;

    y_start1=0;
    y_start2=y_start;
    if(y_start>=256)
    {
        y_start1=y_start2/256;
        y_start2=y_start2%256;
    }
    y_end1=0;
    y_end2=y_start+PART_COLUMN-1;
    if(y_end2>=256)
    {
        y_end1=y_end2/256;
        y_end2=y_end2%256;
    }

    eink_sendCmd(0x44);       // set RAM x address start/end, in page 35
    eink_sendData(x_start);    // RAM x address start at 00h;
    eink_sendData(x_end);    // RAM x address end at 0fh(15+1)*8->128
    eink_sendCmd(0x45);       // set RAM y address start/end, in page 35
    eink_sendData(y_start2);    // RAM y address start at 0127h;
    eink_sendData(y_start1);    // RAM y address start at 0127h;
    eink_sendData(y_end2);    // RAM y address end at 00h;
    eink_sendData(y_end1);    // ????=0


    eink_sendCmd(0x4E);   // set RAM x address count to 0;
    eink_sendData(x_start);
    eink_sendCmd(0x4F);   // set RAM y address count to 0X127;
    eink_sendData(y_start2);
    eink_sendData(y_start1);


     eink_sendCmd(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {
     eink_sendData(*datas);
            datas++;
   }
     eink_update_partial();

}

void eink_update(void)
{
    eink_sendCmd(0x22); //Display Update Control
    eink_sendData(0xF7);
    eink_sendCmd(0x20); //Activate Display Update Sequence
    eink_readBusy();
}

void eink_update_partial(void)
{
    eink_sendCmd(0x22); //Display Update Control
    eink_sendData(0xFF);
    eink_sendCmd(0x20); //Activate Display Update Sequence
    eink_readBusy();
    LOG_INF("eink_update_partial");
}

void eink_set_cursor(uint16_t Xstart, uint16_t Ystart)
{
    eink_sendCmd(0x4E);
    eink_sendData((Xstart >> 3) & 0xFF);

    eink_sendCmd(0x4F);
    eink_sendData(Ystart & 0xFF);
    eink_sendData((Ystart >> 8) & 0xFF);

    eink_readBusy();
}


void eink_deep_sleep(void)
{
    eink_readBusy();
    eink_sendCmd(0x10); //enter deep sleep
    eink_sendData(0x01);
}


void eink_sendCmd(uint8_t cmd)
{
    eink_readBusy();
    board_gpio_data_cmd(0);
    spi_transfer(&cmd, 1);
    board_gpio_data_cmd(1);
}

void eink_sendData(uint8_t dat)
{
    eink_readBusy();
    spi_transfer(&dat, 1);
}

void eink_readBusy(void)
{
    #if 0
    uint32_t timeout = EPD_GET_TICKS() + 1000;
    while (EPD_GET_TICKS() < timeout)
    #else
    while (1)
    #endif
    {
        if (!board_gpio_busy())
        {
            return;
        }
    }

}

