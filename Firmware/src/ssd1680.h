#ifndef __SK_SSD1680_H
#define __SK_SSD1680_H

#define EPD_WIDTH       200
#define EPD_HEIGHT      200
#define EPD_BUF_LEN     EPD_WIDTH*EPD_HEIGHT/8

#define EPD_GET_TICKS()    k_uptime_get_32()

void eink_initialize(bool reset);
void eink_display(uint8_t *bw, uint8_t *red);
void eink_display_partial(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void eink_update(void);
void eink_update_partial(void);
void eink_set_cursor(uint16_t Xstart, uint16_t Ystart);
void eink_deepSleep(void);
void eink_sendCmd(uint8_t cmd);
void eink_sendData(uint8_t dat);
void eink_readBusy(void);
void eink_deep_sleep(void);

#endif
