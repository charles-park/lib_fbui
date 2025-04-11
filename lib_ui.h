//------------------------------------------------------------------------------
/**
 * @file lib_ui.h
 * @author charles-park (charles.park@hardkernel.com)
 * @brief User interface library (include parser)
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
#ifndef __LIB_UI_H__
#define __LIB_UI_H__

#include "lib_ts.h"

//------------------------------------------------------------------------------
#define	ITEM_COUNT_MAX  256
#define	ITEM_STR_MAX    256
#define	ITEM_SCALE_MAX  100

#define	STR_ALIGN_C     0
#define	STR_ALIGN_L     1
#define	STR_ALIGN_R     2

//------------------------------------------------------------------------------
typedef struct rect_item__t {
    int             x, y, w, h, lw;
    fb_color_u      bc, lc;
}   rect_item_t;

typedef struct string_item__t {
    int             x, y, scale, f_type, len;
    fb_color_u      fc, bc;
    char            str[ITEM_STR_MAX];
}   string_item_t;

// rect item
typedef struct r_item__t {
    int             id;
    rect_item_t     r;
}   r_item_t;

// string item
typedef struct s_item__t {
    int             id;
    string_item_t   s;
}   s_item_t;

// rect box + string item
typedef struct b_item__t {
    int             gid;
    int             id;
    rect_item_t     r;
    string_item_t   s;
    int             s_align;
    char            s_dfl[ITEM_STR_MAX];
}   b_item_t;

//------------------+-----------------------------------------------
// grp name(grp_id) | dev_name(dev_id)
//------------------+-----------------------------------------------
//    SYSTEM(0)     | MEM(0), FB_X(1), FB_Y(2)
//    STORAGE(1)    | eMMC(0), SD(1), SATA(2), NVME(3)
//    USB(2)        | USB-3.0(0), USB-2.0(1), USB-OTG(2), USB-H(3)
//    HDMI(3)       | EDID(0), HPD(1)
//    ADC(4)        | ADC_37(0), ADC_40(1)
//    ETHERNET(5)   | IP(0), MAC(1), IPERF(2), LINK_SPEED(3)
//    HEADER(6)     | H40-H14(0), GPIO(1 ~ 999)
//    AUDIO(7)      | LEFT(0), RIGHT(1)
//    LED(9)        | POWER(0), ALIVE(1), NVME(2)
//    PWM(9)        | PWM0(0), PWM1(1)
//------------------+-----------------------------------------------
typedef struct i_item__t {
    int             ui_id;
    char            grp_id;
    char            dev_id;
    char            is_info;
    char            name[ITEM_STR_MAX];
    // 0 : wait, 1 : pass, -1 : fail(cmd run status)
    int             complete;
    // 0 : fail, 1 : success (item check status)
    int             status;
}   i_item_t;

//------------------------------------------------------------------
// Touchscreen t_item
//------------------------------------------------------------------
typedef struct t_item__t {
    // bitem id
    int             ui_id;
    // press color, release color
    fb_color_u      pc, rc;
}   t_item_t;

//------------------------------------------------------------------
// Popup message item
//------------------------------------------------------------------
typedef struct p_item__t {
    // time out
    int             timeout;
    void            *vp_fb;
    rect_item_t     r;
    string_item_t   s;
}   p_item_t;

typedef struct ui_group__t {
    int             f_type;
    fb_color_u      fc, bc, lc;

    int             b_item_cnt;
    b_item_t        b_item[ITEM_COUNT_MAX];

    int             i_item_cnt;
    i_item_t        i_item[ITEM_COUNT_MAX];

    int             t_item_cnt;
    t_item_t        t_item[ITEM_COUNT_MAX];

    p_item_t        p_item;
}   ui_grp_t;

//------------------------------------------------------------------------------
extern int      ui_get_titem    (fb_info_t *fb, ui_grp_t *ui_grp, ts_event_t *event);
extern void     ui_set_ritem    (fb_info_t *fb, ui_grp_t *ui_grp, int f_id, int bc, int lc);
extern void     ui_set_sitem    (fb_info_t *fb, ui_grp_t *ui_grp, int f_1d, int fc, int bc, char *str);
extern void     ui_set_str      (fb_info_t *fb, ui_grp_t *ui_grp,
                                    int f_id, int x, int y, int scale, int font, char *fmt, ...);
extern void     ui_set_printf   (fb_info_t *fb, ui_grp_t *ui_grp, int id, char *fmt, ...);
extern void     ui_update       (fb_info_t *fb, ui_grp_t *ui_grp, int id);
extern void	    ui_update_group	(fb_info_t *fb, ui_grp_t *ui_grp, int gid);
extern void     ui_close        (ui_grp_t *ui_grp);
extern int      ui_set_popup (fb_info_t *fb, ui_grp_t *ui_grp,
                        int w, int h, int lw,       /* box width, box height, box outline width */
                        int fc, int bc, int lc,     /* color : font, background, outline */
                        int fs, int ts, char *fmt, ...); /* font scale, display time(sec), msg format */

extern int      ui_update_popup (fb_info_t *fb, ui_grp_t *ui_grp);
extern ui_grp_t *ui_init        (fb_info_t *fb, const char *cfg_filename);

//------------------------------------------------------------------------------

#endif  // #define __LIB_UI_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
