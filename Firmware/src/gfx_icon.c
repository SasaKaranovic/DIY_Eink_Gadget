#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(gfx_icon, LOG_LEVEL_INF);

#include "gfx.h"
#include "gfx_icon.h"

// To save space, icons are "compressed" where each 1-byte represents 8 pixels
// For simplicity, we will loop trough Y (height) then X (width) and individually set/clear
// each pixel based on it's "bitmap" value.
// This is very naive and slow approach but it's quick, dirty and it works. :)
// Also we are using our gfx_draw_pixel which will abstract this code from the actual
// display type, orientation etc.


static inline bool is_pixel_set(uint8_t *bitmap, uint8_t img_w, uint8_t img_h, uint16_t x, uint16_t y)
{
    uint8_t data = bitmap[ y*img_w/8 + x/8 ];
    if ( data & (0x80>>(x%8)) )
    {
        return true;
    }
    return false;
}

// Draw icon bitmap starting at (x,y) location using color
// Limitation: We expect image height to be divisible by 8
void gfx_draw_icon(const gfx_icon_typedef *icon, uint16_t start_x, uint16_t start_y, uint8_t color, gfx_mirror_t mirror, gfx_rotate_t rotate)
{
    uint8_t *bitmap = icon->bitmap;
    uint8_t width = icon->width;
    uint8_t height = icon->height;
    uint8_t check_x = 0;
    uint8_t check_y = 0;
    uint8_t draw_x = 0;
    uint8_t draw_y = 0;

    for (uint8_t curs_y=0; curs_y<height; curs_y++)
    {
        for (uint8_t curs_x=0; curs_x<width; curs_x++)
        {
            if (mirror == MIRROR_NONE)
            {
                check_x = curs_x;
                check_y = curs_y;
            }
            else if (mirror == MIRROR_X)
            {
                check_x = width-curs_x-1;
                check_y = curs_y;
            }
            else if (mirror == MIRROR_Y)
            {
                check_x = curs_x;
                check_y = height-curs_y-1;
            }
            else if (mirror == MIRROR_XY)
            {
                check_x = width-curs_x-1;
                check_y = height-curs_y-1;
            }

            if (is_pixel_set(bitmap, width, height, check_x, check_y))
            {
                if (rotate == ROTATE_NONE)
                {
                    draw_x = start_x+curs_x;
                    draw_y = start_y+curs_y;
                }
                else if (rotate == ROTATE_CW)
                {
                    draw_x = start_x+curs_y;
                    draw_y = start_y+width-curs_x-1;
                }
                else if (rotate == ROTATE_CCW)
                {
                    draw_x = start_x+curs_y;
                    draw_y = start_y+curs_x;
                }

                gfx_draw_pixel(draw_x, draw_y, color);
            }
        }
    }
}
