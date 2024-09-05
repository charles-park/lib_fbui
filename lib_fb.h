//-----------------------------------------------------------------------------
/**
 * @file lib_fb.h
 * @author charles-park (charles-park@hardkernel.com)
 * @brief framebuffer library header file.
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef __LIB_FB_H__
#define __LIB_FB_H__

//-----------------------------------------------------------------------------
// Color table & convert macro
//-----------------------------------------------------------------------------
#include "color_table.h"

//-----------------------------------------------------------------------------
// Framebuffer blink control
//-----------------------------------------------------------------------------
#define FB_CURSOR_CONTROL   "/sys/class/graphics/fbcon/cursor_blink"

//-----------------------------------------------------------------------------
enum eFB_ROTATE {
    eFB_ROTATE_0   = 0,
    eFB_ROTATE_90  = 90,
    eFB_ROTATE_180 = 180,
    eFB_ROTATE_270 = 270,
    eROTATE_END,
};

//-----------------------------------------------------------------------------
// Frame buffer struct
//-----------------------------------------------------------------------------
typedef union fb_color__u {
    struct {
        unsigned int    b:8;    // lsb
        unsigned int    g:8;
        unsigned int    r:8;
        unsigned int    a:8;
    } bits;
    unsigned int uint;
}	fb_color_u;

typedef struct fb_info__t {
    int     fd;
    int     rotate;
    int     w;
    int     h;
    int     stride;
    int     bpp;
    char    is_bgr;
    char    *base;
    char    *data;
}	fb_info_t;

//-----------------------------------------------------------------------------
#define FONT_HANGUL_WIDTH   16
#define FONT_ASCII_WIDTH    8
#define FONT_HEIGHT         16

enum eFONTS_HANGUL {
    eFONT_HAN_DEFAULT = 0,
    eFONT_HANBOOT,
    eFONT_HANGODIC,
    eFONT_HANPIL,
    eFONT_HANSOFT,
    eFONT_END
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern void         put_pixel   (fb_info_t *fb, int x, int y, int color);
extern void         draw_text   (fb_info_t *fb, int x, int y,
                                    int f_color, int b_color, int scale, char *fmt, ...);
extern void         draw_line   (fb_info_t *fb, int x, int y, int w, int color);
extern void         draw_rect   (fb_info_t *fb, int x, int y, int w, int h, int lw, int color);
extern void         draw_fill_rect (fb_info_t *fb, int x, int y, int w, int h, int color);
extern void         set_font    (enum eFONTS_HANGUL s_font);
extern void         fb_clear    (fb_info_t *fb);
extern void         fb_close    (fb_info_t *fb);
extern void         fb_cursor   (char status);
extern int          fb_get_rotate (fb_info_t *fb);
extern void         fb_set_rotate (fb_info_t *fb, int rotate);
extern fb_info_t    *fb_init    (const char *DEVICE_NAME);

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
#endif  // #define __LIB_FB_H__
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
