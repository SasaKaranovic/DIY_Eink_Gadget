#ifndef _SK_GFX_H_
#define _SK_GFX_H_

#include <stdint.h>
#include "ssd1680.h"

#define gfx_swap(a, b) \
  {                    \
    int16_t t = a;     \
    a = b;             \
    b = t;             \
  } ///< simple swap function

typedef enum EPD_Color
{
  EPD_BLACK,   ///< black color
  EPD_WHITE,   ///< white color
  EPD_INVERSE, ///< invert color
  EPD_RED,     ///< red color
  EPD_RED_INVERSE,     ///< red color
  EPD_DARK,    ///< darker color
  EPD_LIGHT,   ///< lighter color
} EPD_Color;

void gfx_initialize(void);
void gfx_display(void);
void gfx_display_partial(uint32_t x, uint32_t y, uint32_t nColumn, uint32_t nLine);
void gfx_clear_buffer();
void gfx_draw_image(const char *img_bw, const char *img_red);
void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color);
void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void gfx_draw_v_line(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_draw_h_line(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_draw_helper_circle(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void gfx_draw_helper_circle_fill(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color);
void gfx_draw_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void gfx_draw_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void gfx_draw_fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

extern uint8_t img_bw_buff[EPD_BUF_LEN];
extern uint8_t img_red_buff[EPD_BUF_LEN];

#endif
