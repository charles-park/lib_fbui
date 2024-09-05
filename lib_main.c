//------------------------------------------------------------------------------
/**
 * @file lib_main.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief user interface with framebuffer test app.
 * @version 0.1
 * @date 2022-12-04
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <getopt.h>

#include "lib_fb.h"
#include "lib_ui.h"

//------------------------------------------------------------------------------
#if defined (__LIB_FBUI_APP__)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char *OPT_DEVICE_NAME = "/dev/fb0";
const char *OPT_TS_DEVICE_NAME = "/dev/input/event0";
const char *OPT_FBUI_CFG = "fbui.cfg";
const char *OPT_TEXT_STR = "FrameBuffer 테스트 프로그램입니다.";
unsigned int opt_x = 0, opt_y = 0, opt_width = 0, opt_height = 0, opt_color = 0, opt_fb_rotate = 0;
unsigned char opt_red = 0, opt_green = 0, opt_blue = 0, opt_thckness = 1, opt_scale = 1;
unsigned char opt_clear = 0, opt_fill = 0, opt_info = 0, opt_font = 0, opt_ui_cfg = 0;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void print_usage(const char *prog)
{
    printf("Usage: %s [-DrgbxywhfntscCi]\n", prog);
    puts("  -D --device    device to use (default /dev/fb0)\n"
         "  -T --device    device to use (default /dev/input/event0)\n"
         "  -R --rotate    fb rotate display (0, 90, 180, 270. default = 0)\n"
         "  -r --red       pixel red hex value.(default = 0)\n"
         "  -g --green     pixel green hex value.(default = 0)\n"
         "  -b --blue      pixel blue hex value.(default = 0)\n"
         "  -x --x_pos     framebuffer memory x position.(default = 0)\n"
         "  -y --y_pos     framebuffer memory y position.(default = 0)\n"
         "  -w --width     reference width for drawing.\n"
         "  -h --height    reference height for drawing.\n"
         "  -f --fill      drawing fill box.(default empty box)\n"
         "  -n --thickness drawing line thickness.(default = 1)\n"
         "  -t --text      drawing text string.(default str = \"text\"\n"
         "  -s --scale     scale of text.\n"
         "  -c --color     background rgb(hex) color.(ARGB)\n"
         "  -C --clear     clear framebuffer(r = g = b = 0)\n"
         "  -i --info      framebuffer info display.\n"
         "  -F --font      Hangul font select\n"
         "                 0 MYEONGJO\n"
         "                 1 HANBOOT\n"
         "                 2 HANGODIC\n"
         "                 3 HANPIL\n"
         "                 4 HANSOFT\n"
         "  Useage : ./lib_fbui -I fbui.cfg -s 3 -F 2\n"
    );
    exit(1);
}

//------------------------------------------------------------------------------
static void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "fb_device",  1, 0, 'D' },
            { "ts_device",  1, 0, 'T' },
            { "rotate",  	1, 0, 'R' },
            { "red",		1, 0, 'r' },
            { "green",		1, 0, 'g' },
            { "blue",		1, 0, 'b' },
            { "x_pos",		1, 0, 'x' },
            { "y_pos",		1, 0, 'y' },
            { "width",		1, 0, 'w' },
            { "height",		1, 0, 'h' },
            { "fill",		0, 0, 'f' },
            { "thickness",	1, 0, 'n' },
            { "text",		1, 0, 't' },
            { "scale",		1, 0, 's' },
            { "color",		1, 0, 'c' },
            { "clear",		0, 0, 'C' },
            { "info",		0, 0, 'i' },
            { "font",		1, 0, 'F' },
            { "ui_cfg",		1, 0, 'I' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "D:T:R:r:g:b:x:y:w:h:fn:t:s:c:CiF:I:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 'D':
            OPT_DEVICE_NAME = optarg;
            break;
        case 'T':
            OPT_TS_DEVICE_NAME = optarg;
            break;
        case 'R':
            opt_fb_rotate = atoi(optarg);
            break;
        case 'r':
            opt_red = strtol(optarg, NULL, 16);
            opt_red = opt_red < 255 ? opt_red : 255;
            break;
        case 'g':
            opt_green = strtol(optarg, NULL, 16);
            opt_green = opt_green < 255 ? opt_green : 255;
            break;
        case 'b':
            opt_blue = strtol(optarg, NULL, 16);
            opt_blue = opt_blue < 255 ? opt_blue : 255;
            break;
        case 'x':
            opt_x = abs(atoi(optarg));
            break;
        case 'y':
            opt_y = abs(atoi(optarg));
            break;
        case 'w':
            opt_width = abs(atoi(optarg));
            break;
        case 'h':
            opt_height = abs(atoi(optarg));
            break;
        case 'f':
            opt_fill = 1;
            break;
        case 'n':
            opt_thckness = abs(atoi(optarg));
            opt_thckness = opt_thckness > 0? opt_thckness : 1;
            break;
        case 't':
            OPT_TEXT_STR = optarg;
            break;
        case 's':
            opt_scale = abs(atoi(optarg));
            opt_scale = opt_scale ? opt_scale : 1;
            break;
        case 'c':
            opt_color = strtol(optarg, NULL, 16) & 0xFFFFFF;
            break;
        case 'C':
            opt_clear = 1;
            break;
        case 'i':
            opt_info = 1;
            break;
        case 'F':
            opt_font = abs(atoi(optarg));
            break;
        case 'I':
            opt_ui_cfg = 1;
            OPT_FBUI_CFG = optarg;
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }
}

//------------------------------------------------------------------------------
void dump_fb_info (fb_info_t *fb)
{
    printf("========== FB SCREENINFO ==========\n");
    printf("xres   : %d\n", fb->w);
    printf("yres   : %d\n", fb->h);
    printf("bpp    : %d\n", fb->bpp);
    printf("stride : %d\n", fb->stride);
    printf("bgr    : %d\n", fb->is_bgr);
    printf("fb_base     : %p\n", fb->base);
    printf("fb_data     : %p\n", fb->data);
    printf("==================================\n");
}

//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    fb_info_t	*pfb;
    int f_color, b_color;
    ui_grp_t 	*ui_grp;

    parse_opts(argc, argv);

    if ((pfb = fb_init (OPT_DEVICE_NAME)) == NULL) {
        fprintf(stdout, "ERROR: frame buffer init fail!\n");
        exit(1);
    }
    fb_cursor (0);
    fb_set_rotate (pfb, opt_fb_rotate);

    if (opt_ui_cfg) {
        if ((ui_grp = ui_init (pfb, OPT_FBUI_CFG)) == NULL) {
            fprintf(stdout, "ERROR: User interface create fail!\n");
            exit(1);
        }
        ui_update(pfb, ui_grp, -1);
    }

    f_color = RGB_TO_UINT(opt_red, opt_green, opt_blue);
    b_color = COLOR_WHITE;

    if (opt_color)
        b_color = opt_color & 0x00FFFFFF;

    if (opt_clear)
        fb_clear(pfb);

    if (opt_info)
        dump_fb_info(pfb);

    if (OPT_TEXT_STR) {
        set_font(opt_font);
        switch(opt_font) {
            default :
            case eFONT_HAN_DEFAULT:
                draw_text(pfb, 0, pfb->h / 2, f_color, b_color, opt_scale,
                    "한글폰트는 명조체 이며, Font Scale은 %d배 입니다.", opt_scale);
            break;
            case eFONT_HANBOOT:
                draw_text(pfb, 0, pfb->h / 2, f_color, b_color, opt_scale,
                    "한글폰트는 붓글씨체 이며, Font Scale은 %d배 입니다.", opt_scale);
            break;
            case eFONT_HANGODIC:
                draw_text(pfb, 0, pfb->h / 2, f_color, b_color, opt_scale,
                    "한글폰트는 고딕체 이며, Font Scale은 %d배 입니다.", opt_scale);
            break;
            case eFONT_HANPIL:
                draw_text(pfb, 0, pfb->h / 2, f_color, b_color, opt_scale,
                    "한글폰트는 필기체 이며, Font Scale은 %d배 입니다.", opt_scale);
            break;
            case eFONT_HANSOFT:
                draw_text(pfb, 0, pfb->h / 2, f_color, b_color, opt_scale,
                    "한글폰트는 한소프트체 이며, Font Scale은 %d배 입니다.", opt_scale);
            break;
        }
        draw_text(pfb, opt_x, opt_y, f_color, b_color, opt_scale, "%s", OPT_TEXT_STR);
    }

    if (opt_width) {
        if (opt_height) {
            if (opt_fill)
                draw_fill_rect(pfb, opt_x, opt_y, opt_width, opt_height, f_color);
            else
                draw_rect(pfb, opt_x, opt_y, opt_width, opt_height, opt_thckness, f_color);
        }
        else
            draw_line(pfb, opt_x, opt_y, opt_width, f_color);
    }

    // ts input test
    {
        ts_t *p_ts;
        ts_event_t event;

        p_ts = ts_init (OPT_TS_DEVICE_NAME);
        while (p_ts != NULL) {
            usleep (10000);
            if (ts_get_event (pfb, p_ts, &event)) {
                printf ("status = %d, x = %d, y = %d, ui_id = %d\n",
                        event.status, event.x, event.y, ui_get_titem (pfb, ui_grp, &event));

            }
        }
    }

    sleep(1);
    fb_close (pfb);
    if (opt_ui_cfg)
        ui_close(ui_grp);

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif	// #if defined (__LIB_FBUI_APP__)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
