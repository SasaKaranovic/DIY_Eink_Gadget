#ifndef _SK_GFX_FONT_H_
#define _SK_GFX_FONT_H_

#include "gfx.h"

typedef struct {
  uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
  uint8_t width;         ///< Bitmap dimensions in pixels
  uint8_t height;        ///< Bitmap dimensions in pixels
  uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
  int8_t xOffset;        ///< X dist from cursor pos to UL corner
  int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
  uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
  GFXglyph *glyph;  ///< Glyph array
  uint16_t first;   ///< ASCII extents (first char)
  uint16_t last;    ///< ASCII extents (last char)
  uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;


typedef enum
{
    FREE_MONO_9PT = 0,
    FREE_MONO_12PT = 1,
    FREE_MONO_18PT = 2,
    FREE_MONO_24PT = 3,
    FREE_MONO_BOLD_9PT = 4,
    FREE_MONO_BOLD_12PT = 5,
    FREE_MONO_BOLD_18PT = 6,
    FREE_MONO_BOLD_24PT = 7,
    FREE_SANS_9PT = 8,
    FREE_SANS_12PT = 9,
    FREE_SANS_18PT = 10,
    FREE_SANS_24PT = 11,
    FREE_DEFAULT = 99
} gfx_font_t;

void gfx_set_font(uint8_t font);
void gfx_draw_char(int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y, unsigned char c);
void gfx_draw_text(int16_t x, int16_t y, int16_t color, int16_t bg, int16_t size_x, int16_t size_y, const char *str);
void gfx_draw_centered_text(int16_t x, int16_t y, int16_t color, int16_t bg, int16_t size_x, int16_t size_y, const char *str);
uint16_t gfx_font_get_str_width(const char *str);
#endif
