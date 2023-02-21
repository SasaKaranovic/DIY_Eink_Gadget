#ifndef _SK_GFX_ICON_H_
#define _SK_GFX_ICON_H_

#include <stdint.h>

typedef struct {
  uint8_t *bitmap;       // Pointer to icon bitmap
  uint8_t width;         // bitmap size in X
  uint8_t height;        // bitmap siz in Y
} gfx_icon_typedef;

typedef enum gfx_mirror_t
{
    MIRROR_NONE,
    MIRROR_X,
    MIRROR_Y,
    MIRROR_XY,
} gfx_mirror_t;

typedef enum gfx_rotate_t
{
    ROTATE_NONE,
    ROTATE_CW,
    ROTATE_CCW
} gfx_rotate_t;

void gfx_draw_icon(const gfx_icon_typedef *icon, uint16_t start_x, uint16_t start_y, uint8_t color, gfx_mirror_t mirror, gfx_rotate_t rotate);

#endif
