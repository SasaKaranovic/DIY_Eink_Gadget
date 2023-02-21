#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gfx, LOG_LEVEL_INF);

#include <stdlib.h>
#include "gfx.h"
#include "ssd1680.h"

#define BYTES_PER_Y (EPD_HEIGHT/8)

uint8_t display_rotation = 3;
uint8_t img_bw_buff[EPD_BUF_LEN] = {0xFF};
uint8_t img_red_buff[EPD_BUF_LEN] = {0};


void gfx_initialize(void)
{
    gfx_clear_buffer();
    LOG_DBG("Setting up EINK");
    eink_initialize(true);
}

void gfx_display(void)
{
    eink_display(img_bw_buff, img_red_buff);
}

void gfx_display_partial(uint32_t x, uint32_t y, uint32_t nColumn, uint32_t nLine)
{
    eink_display_partial(x, y, &img_bw_buff[(x/8)+y], nColumn, nLine);
}

void gfx_clear_buffer()
{
    memset(img_bw_buff, 0xFF, EPD_BUF_LEN);
    memset(img_red_buff, 0x00, EPD_BUF_LEN);
}

void gfx_set_buffer(uint8_t buff, uint8_t val)
{
    if (buff == 0)
    {
        memset(img_bw_buff, val, EPD_BUF_LEN);
    }
    else
    {
        memset(img_red_buff, val, EPD_BUF_LEN);
    }
}

void gfx_draw_image(const char *img_bw, const char *img_red)
{
    for (int i = 0; i < EPD_BUF_LEN; i++)
    {
        img_bw_buff[i] = img_bw[i];
    }

    for (int i = 0; i < EPD_BUF_LEN; i++)
    {
        img_red_buff[i] = img_red[i];
    }
}

void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x < 0) || (x >= EPD_WIDTH) || (y < 0) || (y >= EPD_HEIGHT))
        return;

    uint8_t *pBuf;
    uint16_t addr;

    addr = x*BYTES_PER_Y + y/8;

    if (color == EPD_RED || color == EPD_RED_INVERSE)
    {
        pBuf = img_red_buff + addr;
    }
    else
    {
        pBuf = img_bw_buff + addr;
    }

    // x is which column
    switch (color)
    {
        case EPD_WHITE:
            *pBuf |= (1 << (7 - y % 8));
            break;
        case EPD_RED:
            *pBuf |= (1 << (7 - y % 8));
            break;
        case EPD_BLACK:
            *pBuf &= ~(1 << (7 - y % 8));
            break;
        case EPD_INVERSE:
        case EPD_RED_INVERSE:
            *pBuf ^= (1 << (7 - y % 8));
            break;
        default:
            LOG_ERR("Invalid color %d", color);
            break;
    }
}



void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    gfx_swap(x0, y0);
    gfx_swap(x1, y1);
  }

  if (x0 > x1)
  {
    gfx_swap(x0, x1);
    gfx_swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      gfx_draw_pixel(y0, x0, color);
    }
    else
    {
      gfx_draw_pixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void gfx_draw_v_line(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    gfx_draw_line(x, y, x, y + h - 1, color);
}

void gfx_draw_h_line(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  gfx_draw_line(x, y, x + w - 1, y, color);
}

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  gfx_draw_pixel(x0, y0 + r, color);
  gfx_draw_pixel(x0, y0 - r, color);
  gfx_draw_pixel(x0 + r, y0, color);
  gfx_draw_pixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    gfx_draw_pixel(x0 + x, y0 + y, color);
    gfx_draw_pixel(x0 - x, y0 + y, color);
    gfx_draw_pixel(x0 + x, y0 - y, color);
    gfx_draw_pixel(x0 - x, y0 - y, color);
    gfx_draw_pixel(x0 + y, y0 + x, color);
    gfx_draw_pixel(x0 - y, y0 + x, color);
    gfx_draw_pixel(x0 + y, y0 - x, color);
    gfx_draw_pixel(x0 - y, y0 - x, color);
  }
}

void gfx_draw_helper_circle(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      gfx_draw_pixel(x0 + x, y0 + y, color);
      gfx_draw_pixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      gfx_draw_pixel(x0 + x, y0 - y, color);
      gfx_draw_pixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      gfx_draw_pixel(x0 - y, y0 + x, color);
      gfx_draw_pixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      gfx_draw_pixel(x0 - y, y0 - x, color);
      gfx_draw_pixel(x0 - x, y0 - y, color);
    }
  }
}

void gfx_draw_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  gfx_draw_v_line(x0, y0 - r, 2 * r + 1, color);
  gfx_draw_helper_circle_fill(x0, y0, r, 3, 0, color);
}

void gfx_draw_helper_circle_fill(int16_t x0, int16_t y0, int16_t r,
                                    uint8_t corners, int16_t delta,
                                    uint16_t color)
{

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        gfx_draw_v_line(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        gfx_draw_v_line(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        gfx_draw_v_line(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        gfx_draw_v_line(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  gfx_draw_h_line(x, y, w, color);
  gfx_draw_h_line(x, y + h - 1, w, color);
  gfx_draw_v_line(x, y, h, color);
  gfx_draw_v_line(x + w - 1, y, h, color);
}

void gfx_draw_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i = x; i < x + w; i++)
  {
    gfx_draw_v_line(i, y, h, color);
  }
}


void gfx_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  gfx_draw_h_line(x + r, y, w - 2 * r, color);         // Top
  gfx_draw_h_line(x + r, y + h - 1, w - 2 * r, color); // Bottom
  gfx_draw_v_line(x, y + r, h - 2 * r, color);         // Left
  gfx_draw_v_line(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  gfx_draw_helper_circle(x + r, y + r, r, 1, color);
  gfx_draw_helper_circle(x + w - r - 1, y + r, r, 2, color);
  gfx_draw_helper_circle(x + w - r - 1, y + h - r - 1, r, 4, color);
  gfx_draw_helper_circle(x + r, y + h - r - 1, r, 8, color);
}

void gfx_draw_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  gfx_draw_fill_rect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  gfx_draw_helper_circle_fill(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  gfx_draw_helper_circle_fill(x + r, y + r, r, 2, h - 2 * r - 1, color);
}


void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  gfx_draw_line(x0, y0, x1, y1, color);
  gfx_draw_line(x1, y1, x2, y2, color);
  gfx_draw_line(x2, y2, x0, y0, color);
}


void gfx_draw_fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    gfx_swap(y0, y1);
    gfx_swap(x0, x1);
  }
  if (y1 > y2) {
    gfx_swap(y2, y1);
    gfx_swap(x2, x1);
  }
  if (y0 > y1) {
    gfx_swap(y0, y1);
    gfx_swap(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    gfx_draw_h_line(a, y0, b - a + 1, color);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      gfx_swap(a, b);
    gfx_draw_h_line(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      gfx_swap(a, b);
    gfx_draw_h_line(a, y, b - a + 1, color);
  }
}







