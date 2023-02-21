#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gfx_font, LOG_LEVEL_INF);

#include <stdlib.h>
#include <stdio.h>

#include "ssd1680.h"
#include "gfx_font.h"
#include "fonts/FreeMono9pt7b.h"
#include "fonts/FreeSerifBold24pt7b.h"
#include "fonts/FreeMono12pt7b.h"
#include "fonts/FreeMono18pt7b.h"
#include "fonts/FreeMono24pt7b.h"
#include "fonts/FreeMonoBold9pt7b.h"
#include "fonts/FreeMonoBold12pt7b.h"
#include "fonts/FreeMonoBold18pt7b.h"
#include "fonts/FreeMonoBold24pt7b.h"
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans24pt7b.h"

extern const unsigned char font[];
bool _cp437 = false;
bool wrap = false;
uint16_t cursor_x = 0;
uint16_t cursor_y = 0;
uint16_t textsize_y = 1;
uint16_t textsize_x = 1;

GFXfont *gfxFont = (GFXfont *)&FreeSerifBold24pt7b;


// Private function forward declaration
static uint16_t gfx_font_get_char_width(unsigned char c);

void gfx_set_font(uint8_t font)
{
    switch(font)
    {
        case FREE_MONO_9PT:
            gfxFont = (GFXfont *)&FreeMono9pt7b;
            break;
        case FREE_MONO_12PT:
            gfxFont = (GFXfont *)&FreeMono12pt7b;
            break;
        case FREE_MONO_18PT:
            gfxFont = (GFXfont *)&FreeMono18pt7b;
            break;
        case FREE_MONO_24PT:
            gfxFont = (GFXfont *)&FreeMono24pt7b;
            break;
        case FREE_MONO_BOLD_9PT:
            gfxFont = (GFXfont *)&FreeMonoBold9pt7b;
            break;
        case FREE_MONO_BOLD_12PT:
            gfxFont = (GFXfont *)&FreeMonoBold12pt7b;
            break;
        case FREE_MONO_BOLD_18PT:
            gfxFont = (GFXfont *)&FreeMonoBold18pt7b;
            break;
        case FREE_MONO_BOLD_24PT:
            gfxFont = (GFXfont *)&FreeMonoBold24pt7b;
            break;
        case FREE_SANS_9PT:
            gfxFont = (GFXfont *)&FreeSans9pt7b;
            break;
        case FREE_SANS_12PT:
            gfxFont = (GFXfont *)&FreeSans12pt7b;
            break;
        case FREE_SANS_18PT:
            gfxFont = (GFXfont *)&FreeSans18pt7b;
            break;
        case FREE_SANS_24PT:
            gfxFont = (GFXfont *)&FreeSans24pt7b;
            break;

        default:
            gfxFont = NULL;
            break;
    }
}


void gfx_draw_char(int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y, unsigned char c)
{
  if (gfxFont == NULL) { // 'Classic' built-in font

    if ((x >= EPD_WIDTH) ||              // Clip right
        (y >= EPD_HEIGHT) ||             // Clip bottom
        ((x + 6 * size_x - 1) < 0) || // Clip left
        ((y + 8 * size_y - 1) < 0))   // Clip top
      return;

    if (!_cp437 && (c >= 176))
      c++; // Handle 'classic' charset behavior

    for (int8_t i = 0; i < 5; i++)
    { // Char bitmap = 5 columns
      uint8_t line = font[c * 5 + i];
      for (int8_t j = 0; j < 8; j++, line >>= 1)
      {
        if (line & 1)
        {
          if (size_x == 1 && size_y == 1)
          {
            gfx_draw_pixel(x + i, y + j, color);
          }
          else
          {
            gfx_draw_fill_rect(x + i * size_x, y + j * size_y, size_x, size_y,
                          color);
          }
        }
        else if (bg != color)
        {
          if (size_x == 1 && size_y == 1)
          {
            gfx_draw_pixel(x + i, y + j, bg);
          }
          else
          {
            gfx_draw_fill_rect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
          }
        }
      }
    }
    if (bg != color)
    { // If opaque, draw vertical line for last column
      if (size_x == 1 && size_y == 1)
      {
        gfx_draw_v_line(x + 5, y, 8, bg);
      }
      else
      {
        gfx_draw_fill_rect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
      }
    }

  }
  else
  { // Custom font

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= gfxFont->first;
    GFXglyph *glyph = gfxFont->glyph + c;
    uint8_t *bitmap = gfxFont->bitmap;


    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->xOffset;
    int8_t yo = glyph->yOffset;
    uint8_t xx;
    uint8_t yy;
    uint8_t bits = 0;
    uint8_t bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1)
    {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    for (yy = 0; yy < h; yy++)
    {
      for (xx = 0; xx < w; xx++)
      {
        if (!(bit++ & 7))
        {
          bits = bitmap[bo++];
        }
        if (bits & 0x80)
        {
          if (size_x == 1 && size_y == 1)
          {
            gfx_draw_pixel(x + xo + xx, y + yo + yy, color);
          }
          else
          {
            gfx_draw_fill_rect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                          size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }

  } // End classic vs custom font
}


void gfx_draw_text(int16_t x, int16_t y, int16_t color, int16_t bg, int16_t size_x, int16_t size_y, const char *str)
{
    int len = strlen(str);
    char c;

    for (uint8_t i=0; i<len; i++)
    {
        gfx_draw_char(x, y, color, bg, size_x, size_y, str[i]);

        if (gfxFont == NULL)
        {
            x += size_x*6;
        }
        else
        {
            c = str[i];
            c -= gfxFont->first;
            GFXglyph *glyph = gfxFont->glyph + c;
            x += glyph->xAdvance * (int16_t)size_x;
        }
    }
}

void gfx_draw_centered_text(int16_t x, int16_t y, int16_t color, int16_t bg, int16_t size_x, int16_t size_y, const char *str)
{
    uint16_t w;
    w = gfx_font_get_str_width(str);

    x = x - w/2;
    if(x<0)
    {
        x = 0;
    }
    gfx_draw_text(x, y, color, bg, size_x, size_y, str);
}

static uint16_t gfx_font_get_char_width(unsigned char c)
{

  if (gfxFont != NULL)
  {
      if ((c >= gfxFont->first) && (c <= gfxFont->last)) // Char present in this font?
      {
            c -= gfxFont->first;
            GFXglyph *glyph = gfxFont->glyph + c;
        return glyph->xAdvance * textsize_x;
      }
      else
      {
        return 0;
      }
  }
  else
  {
     return textsize_x * 6;
  }
}


uint16_t gfx_font_get_str_width(const char *str)
{
  uint8_t c; // Current character
  uint16_t width = 0; // Initial size is zero

  while ((c = *str++)) {
    width += gfx_font_get_char_width(c);
  }

  return width;
}
