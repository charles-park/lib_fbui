//------------------------------------------------------------------------------
/**
 * @file lib_ui.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief User interface library (include parser)
 * @version 0.1
 * @date 2022-09-30
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

//------------------------------------------------------------------------------
#include "lib_fb.h"
#include "lib_ui.h"

//------------------------------------------------------------------------------
// Function prototype.
//------------------------------------------------------------------------------
static   int  _my_strlen         (char *str);
static   int  _ui_str_scale      (int w, int h, int lw, int slen);
static   void _ui_clr_str        (fb_info_t *fb, rect_item_t *r_item, string_item_t *s_item);
static   void _ui_update_r       (fb_info_t *fb, rect_item_t *r_item);
static   void _ui_update_s       (fb_info_t *fb, string_item_t *s_item, int x, int y);
static   void _ui_parser_cmd_C   (char *buf, fb_info_t *fb, ui_grp_t *ui_grp);
static   void _ui_parser_cmd_R   (char *buf, fb_info_t *fb, ui_grp_t *ui_grp);
static   void _ui_parser_cmd_S   (char *buf, ui_grp_t *ui_grp);
static   void _ui_parser_cmd_B   (char *buf, fb_info_t *fb, ui_grp_t *ui_grp);
static   void _ui_parser_cmd_I   (char *buf, ui_grp_t *ui_grp);
static   void _ui_parser_cmd_T   (char *buf, ui_grp_t *ui_grp);
static   void _ui_str_pos_xy     (rect_item_t *r_item, string_item_t *s_item);
static   void *_ui_find_item     (ui_grp_t *ui_grp, int fid);
static   void _ui_update         (fb_info_t *fb, ui_grp_t *ui_grp, int id);

         int ui_get_titem        (fb_info_t *fb, ui_grp_t *ui_grp, ts_event_t *event);
         void ui_set_ritem       (fb_info_t *fb, ui_grp_t *ui_grp, int f_id, int bc, int lc);
         void ui_set_sitem       (fb_info_t *fb, ui_grp_t *ui_grp, int f_1d, int fc, int bc, char *str);
         void ui_set_str         (fb_info_t *fb, ui_grp_t *ui_grp,
                                    int f_id, int x, int y, int scale, int font, char *fmt, ...);
         void ui_set_printf      (fb_info_t *fb, ui_grp_t *ui_grp, int id, char *fmt, ...);
         void ui_update          (fb_info_t *fb, ui_grp_t *ui_grp, int id);
         void ui_update_group    (fb_info_t *fb, ui_grp_t *ui_grp, int gid);
         void ui_close           (ui_grp_t *ui_grp);
         ui_grp_t *ui_init       (fb_info_t *fb, const char *cfg_filename);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
   UI Config file 형식

   [ type ]
   '#' : commant
   'C' : default config data
   'R' : Rect data
   'S' : string data
   'I' : Init item data
   'T' : Touch box data

   Rect data x, y, w, h는 fb의 비율값 (0%~100%), 모든 컬러값은 32bits rgb data.

   fbui_v20.cfg file 참조
*/

//------------------------------------------------------------------------------
static int _my_strlen(char *str)
{
   int cnt = 0, err = 512;

   /* utf-8 에서 한글표현은 3바이트 */
   while ((*str != 0x00) && err--) {
      if (*str & 0x80) {
         str += 3;   cnt += 2;
      } else {
         str += 1;   cnt++;
      }
   }
   return err ? cnt : 0;
}

//------------------------------------------------------------------------------
static int _ui_str_scale (int w, int h, int lw, int slen)
{
   int as, w_len, h_len;

   /* auto scaling */
   /* 배율이 설정되어진 최대치 보다 큰 경우 종료한다. */
   for (as = 1; as < ITEM_SCALE_MAX; as++) {
      w_len = FONT_ASCII_WIDTH * as * slen + lw * 2;
      h_len = FONT_HEIGHT      * as        + lw * 2;
      /*
         만약 배율이 1인 경우에도 화면에 표시되지 않는 경우 scale은 0값이 되고
         문자열은 화면상의 표시가 되지 않는다.
      */
      if ((w_len > w) || (h_len > h)) {
         if (as == 1)
            fprintf(stdout, "ERROR: String length too big. String can't display(scale = 0).\n");
         return (as -1);
      }
   }
   return ITEM_SCALE_MAX;
}

//------------------------------------------------------------------------------
static void _ui_clr_str (fb_info_t *fb, rect_item_t *r_item, string_item_t *s_item)
{
   int color = s_item->fc.uint;

   /* 기존 String을 배경색으로 다시 그림(텍스트 지움) */
   /* string x, y 좌표 연산 */
   s_item->fc.uint = s_item->bc.uint;
   _ui_str_pos_xy(r_item, s_item);
   _ui_update_s (fb, s_item, r_item->x, r_item->y);
   s_item->fc.uint = color;
   memset (s_item->str, 0x00, ITEM_STR_MAX);
}

//------------------------------------------------------------------------------
static void _ui_update_r (fb_info_t *fb, rect_item_t *r_item)
{
   draw_fill_rect (fb, r_item->x, r_item->y, r_item->w, r_item->h,
                     r_item->bc.uint);
   if (r_item->lw)
      draw_rect (fb, r_item->x, r_item->y, r_item->w, r_item->h, r_item->lw,
                     r_item->lc.uint);
}

//------------------------------------------------------------------------------
static void _ui_update_s (fb_info_t *fb, string_item_t *s_item, int x, int y)
{
   draw_text (fb, x + s_item->x, y + s_item->y, s_item->fc.uint, s_item->bc.uint,
               s_item->scale, s_item->str);
}

//------------------------------------------------------------------------------
// C(cmd), LCD RGB배열(0 = RGB, 1 = BGR), 기본문자색상(fc), 기본박스색상(rc), 기본외곽색상(lc), 한글폰트(fn:0~4)
//------------------------------------------------------------------------------
static void _ui_parser_cmd_C (char *buf, fb_info_t *fb, ui_grp_t *ui_grp)
{
   char *ptr = strtok (buf, ",");

   ptr = strtok (NULL, ",");     fb->is_bgr        = (atoi(ptr) != 0) ? 1: 0;
   ptr = strtok (NULL, ",");     ui_grp->fc.uint   = strtol(ptr, NULL, 16);
   ptr = strtok (NULL, ",");     ui_grp->bc.uint   = strtol(ptr, NULL, 16);
   ptr = strtok (NULL, ",");     ui_grp->lc.uint   = strtol(ptr, NULL, 16);
   ptr = strtok (NULL, ",");     ui_grp->f_type    = atoi(ptr);

   set_font(ui_grp->f_type);
}

//------------------------------------------------------------------------------
// R(cmd), ID(uid), 시작x좌표(x%), 시작y좌표(y%), 넓이(w%), 높이(h%), 외곽두께(lw), color, line_color, GroupID,
//------------------------------------------------------------------------------
static void _ui_parser_cmd_R (char *buf, fb_info_t *fb, ui_grp_t *ui_grp)
{
   int item_cnt = ui_grp->b_item_cnt, id, item_pos, color;
   char *ptr = strtok (buf, ",");
   rect_item_t *r;

   ptr = strtok (NULL, ",");  id = atoi(ptr);

   /* 설정되어진 ID가 있는지 찾는다. (기존 설정되어진 값 변경시) */
   for (item_pos = 0; item_pos < item_cnt; item_pos++) {
      if (ui_grp->b_item[item_pos].id == id)
         break;
   }
   ui_grp->b_item[item_pos].id = id;
   r = &ui_grp->b_item[item_pos].r;

   ptr = strtok (NULL, ",");     r->x  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->y  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->w  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->h  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->lw = atoi(ptr);

   r->x = (r->x * fb->w / 100);  r->y = (r->y * fb->h / 100);
   r->w = (r->w * fb->w / 100);  r->h = (r->h * fb->h / 100);

   r->bc.uint = ui_grp->bc.uint; r->lc.uint = ui_grp->lc.uint;

   ptr = strtok (NULL, ",");
   if ((color = strtol(ptr, NULL, 16)) >= 0)
      r->bc.uint = color;

   ptr = strtok (NULL, ",");
   if ((color = strtol(ptr, NULL, 16)) >= 0)
      r->lc.uint = color;

   ptr = strtok (NULL, ",");
   ui_grp->b_item[item_pos].gid = atoi(ptr);

   if (item_cnt == item_pos)
      ui_grp->b_item_cnt++;
}

//------------------------------------------------------------------------------
// S(cmd), ID(uid), 폰트크기(scale), 문자정렬(align), color, back_color, 문자열(str)
//------------------------------------------------------------------------------
static void _ui_parser_cmd_S (char *buf, ui_grp_t *ui_grp)
{
   int item_cnt = ui_grp->b_item_cnt, id, item_pos, color;
   char *ptr = strtok (buf, ",");
   string_item_t *s;

   ptr = strtok (NULL, ",");  id = atoi(ptr);

   /* 설정되어진 ID가 있는지 찾는다. (기존 설정되어진 값 변경시) */
   for (item_pos = 0; item_pos < item_cnt; item_pos++) {
      if (ui_grp->b_item[item_pos].id == id)
         break;
   }
   ui_grp->b_item[item_pos].id = id;
   s = &ui_grp->b_item[item_pos].s;
   ptr = strtok (NULL, ",");     s->scale = atoi(ptr);
   ptr = strtok (NULL, ",");     ui_grp->b_item[item_pos].s_align = atoi(ptr);

   s->f_type  = ui_grp->f_type;  s->fc.uint = ui_grp->fc.uint;
   s->bc.uint = ui_grp->bc.uint;

   ptr = strtok (NULL, ",");
   if ((color = strtol(ptr, NULL, 16)) >= 0)
      s->fc.uint = color;

   ptr = strtok (NULL, ",");
   if ((color = strtol(ptr, NULL, 16)) >= 0)
      s->bc.uint = color;
   else
      s->bc.uint = ui_grp->b_item[item_pos].r.bc.uint;

   /* 문자열이 없거나 앞부분의 공백이 있는 경우 제거 */
   if ((ptr = strtok (NULL, ",")) != NULL) {
      int slen = strlen(ptr);

      while ((*ptr == 0x20) && slen--)
         ptr++;

      s->len = slen;
      strncpy(s->str, ptr, s->len);
      // default string for ui_reset
      strncpy(ui_grp->b_item[item_pos].s_dfl, ptr, s->len);
   }

   switch (ui_grp->b_item[item_pos].s_align) {
      case STR_ALIGN_L: case STR_ALIGN_R: case STR_ALIGN_C:
      default :
         s->x = -1;  s->y = -1;
      break;
   }

   if (item_cnt == item_pos)
      ui_grp->b_item_cnt++;
}

//------------------------------------------------------------------------------
// B(cmd), ID(id), 시작x좌표(x%), 시작y좌표(y%), 넓이(w%), 높이(h%), 외곽두께(lw),
//          폰트크기(scale), 문자정렬(align), GroupID, 문자열(str)
//------------------------------------------------------------------------------
static void _ui_parser_cmd_B (char *buf, fb_info_t *fb, ui_grp_t *ui_grp)
{
   int item_cnt = ui_grp->b_item_cnt;
   char *ptr = strtok (buf, ",");
   rect_item_t   *r = &ui_grp->b_item[item_cnt].r;
   string_item_t *s = &ui_grp->b_item[item_cnt].s;

   ptr = strtok (NULL, ",");     ui_grp->b_item[item_cnt].id = atoi(ptr);
   ptr = strtok (NULL, ",");     r->x  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->y  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->w  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->h  = atoi(ptr);
   ptr = strtok (NULL, ",");     r->lw = atoi(ptr);

   r->x = (r->x * fb->w / 100);  r->y = (r->y * fb->h / 100);
   r->w = (r->w * fb->w / 100);  r->h = (r->h * fb->h / 100);

   r->bc.uint = ui_grp->bc.uint;
   r->lc.uint = ui_grp->lc.uint;

   ptr = strtok (NULL, ",");     s->scale = atoi(ptr);
   ptr = strtok (NULL, ",");     ui_grp->b_item[item_cnt].s_align = atoi(ptr);

   ptr = strtok (NULL, ",");
   ui_grp->b_item[item_cnt].gid = atoi(ptr);

   /* 문자열이 없거나 앞부분의 공백이 있는 경우 제거 */
   if ((ptr = strtok (NULL, ",")) != NULL) {
      int slen = strlen(ptr);

      while ((*ptr == 0x20) && slen--)
         ptr++;

      s->len = slen;
      strncpy(s->str, ptr, s->len);
      // default string for ui_reset
      strncpy(ui_grp->b_item[item_cnt].s_dfl, ptr, s->len);
   }

   s->f_type  = ui_grp->f_type;   s->fc.uint = ui_grp->fc.uint;
   s->bc.uint = ui_grp->bc.uint;

   switch (ui_grp->b_item[item_cnt].s_align) {
      case STR_ALIGN_L: case STR_ALIGN_R: case STR_ALIGN_C:
      default :
         s->x = -1;  s->y = -1;
      break;
   }

   item_cnt++;
   ui_grp->b_item_cnt = item_cnt;
}

//------------------------------------------------------------------------------
static void _ui_parser_cmd_I (char *buf, ui_grp_t *ui_grp)
{
   int item_cnt = ui_grp->i_item_cnt;
   char *ptr = strtok (buf, ",");

   if ((ptr = strtok (NULL, ",")) != NULL)
      ui_grp->i_item[item_cnt].ui_id = atoi(ptr);

   if ((ptr = strtok (NULL, ",")) != NULL)
      ui_grp->i_item[item_cnt].grp_id = atoi(ptr);

   if ((ptr = strtok (NULL, ",")) != NULL)
      ui_grp->i_item[item_cnt].dev_id = atoi(ptr);

   ui_grp->i_item[item_cnt].is_info = 1;
   if ((ptr = strtok (NULL, ",")) != NULL)
      ui_grp->i_item[item_cnt].is_info = (atoi(ptr) == 1) ? 1 : 0;

   /* 문자열이 없거나 앞부분의 공백이 있는 경우 제거 */
   if ((ptr = strtok (NULL, ",")) != NULL) {
      int slen = strlen(ptr);

      while ((*ptr == 0x20) && slen--)
         ptr++;

      strncpy(ui_grp->i_item[item_cnt].name, ptr, slen);
   }
   item_cnt++;
   ui_grp->i_item_cnt = item_cnt;
}

//------------------------------------------------------------------------------
// T(cmd), ID(uid), Press Color, Release Color
//------------------------------------------------------------------------------
static void _ui_parser_cmd_T (char *buf, ui_grp_t *ui_grp)
{
   int item_cnt = ui_grp->t_item_cnt, color, id, item_pos;
   char *ptr = strtok (buf, ",");

   ui_grp->t_item[item_cnt].ui_id   = 0;
   ui_grp->t_item[item_cnt].pc.uint = ui_grp->bc.uint;
   ui_grp->t_item[item_cnt].rc.uint = ui_grp->bc.uint;

   if ((ptr = strtok (NULL, ",")) != NULL)
      ui_grp->t_item[item_cnt].ui_id = atoi(ptr);

   if ((ptr = strtok (NULL, ",")) != NULL) {
      if ((color = strtol (ptr, NULL, 16)) >= 0)
         ui_grp->t_item[item_cnt].pc.uint = color;
   }

   if ((ptr = strtok (NULL, ",")) != NULL) {
      if ((color = strtol (ptr, NULL, 16)) >= 0)
         ui_grp->t_item[item_cnt].rc.uint = color;
      else {
         /* 설정되어진 ID가 있는지 찾는다. (기존 설정되어진 값 변경시) */
         id = ui_grp->t_item[item_cnt].ui_id;
         for (item_pos = 0; item_pos < ui_grp->b_item_cnt; item_pos++) {
            if (ui_grp->b_item[item_pos].id == id) {
               rect_item_t *r = &ui_grp->b_item[item_pos].r;
               ui_grp->t_item[item_cnt].rc.uint = r->bc.uint;
               break;
            }
         }
      }
   }
   item_cnt++;
   ui_grp->t_item_cnt = item_cnt;
}

//------------------------------------------------------------------------------
static void _ui_str_pos_xy (rect_item_t *r_item, string_item_t *s_item)
{
   int slen = _my_strlen(s_item->str);

   if (s_item->x < 0) {
      slen = slen * FONT_ASCII_WIDTH * s_item->scale;
      s_item->x = ((r_item->w - slen) / 2);
   }
   if (s_item->y < 0)
      s_item->y = ((r_item->h - FONT_HEIGHT * s_item->scale)) / 2;
}

//------------------------------------------------------------------------------
static void *_ui_find_item (ui_grp_t *ui_grp, int fid)
{
   int i;
   for (i = 0; i < ui_grp->b_item_cnt; i++) {
      if (fid == ui_grp->b_item[i].id)
         return &ui_grp->b_item[i];
   }
   return NULL;
}

//------------------------------------------------------------------------------
int ui_get_titem (fb_info_t *fb, ui_grp_t *ui_grp, ts_event_t *event)
{
   int ui_id, x_s, x_e, y_s, y_e, i, p_color, r_color;
   b_item_t *pitem;

   for (i = 0; i < ui_grp->t_item_cnt; i++) {
      ui_id    = ui_grp->t_item[i].ui_id;
      p_color  = ui_grp->t_item[i].pc.uint;
      r_color  = ui_grp->t_item[i].rc.uint;

      if ((pitem = (b_item_t *)_ui_find_item(ui_grp, ui_id)) == NULL)
         continue;
      x_s = pitem->r.x;
      x_e = pitem->r.x + pitem->r.w;
      y_s = pitem->r.y;
      y_e = pitem->r.y + pitem->r.h;

      if ((x_s <= event->x) && (x_e > event->x)) {
         if ((y_s <= event->y) && (y_e > event->y)) {
            if (event->status == eTS_STATUS_PRESS)
               ui_set_ritem (fb, ui_grp, ui_id, p_color, -1);
            else
               ui_set_ritem (fb, ui_grp, ui_id, r_color, -1);

            return ui_id;
         }
      }
   }
   return -1;
}

//------------------------------------------------------------------------------
void ui_set_ritem (fb_info_t *fb, ui_grp_t *ui_grp, int f_id, int bc, int lc)
{
    b_item_t *pitem = _ui_find_item(ui_grp, f_id);

    /* popup message */
    if (ui_grp->p_item.timeout) return;

    if ((f_id < ITEM_COUNT_MAX) && (pitem != NULL)) {
        if (bc != -1)  pitem->r.bc.uint = bc;
        if (lc != -1)  pitem->r.lc.uint = lc;
        ui_set_sitem (fb, ui_grp, f_id, -1, bc, NULL);
        ui_update (fb, ui_grp, f_id);
    }
}

//------------------------------------------------------------------------------
void ui_set_sitem (fb_info_t *fb, ui_grp_t *ui_grp, int f_id, int fc, int bc, char *str)
{
    b_item_t *pitem = _ui_find_item(ui_grp, f_id);

    /* popup message */
    if (ui_grp->p_item.timeout) return;

    if ((f_id < ITEM_COUNT_MAX) && (pitem != NULL)) {
        /* font color 변경 */
        if (fc != -1)
            pitem->s.fc.uint = fc;
        if (bc != -1)
            pitem->s.bc.uint = bc;

        /* 받아온 string을 buf에 저장 */
        if (str != NULL)  {
            char buf[ITEM_STR_MAX];

            memset (buf, 0x00, sizeof(buf));
            sprintf(buf, "%s", str);
            /*
                기존 문자열 보다 새로운 문자열이 더 작은 경우
                기존 문자열을 배경색으로 덮어 씌운다.
            */
            if ((strlen(pitem->s.str) > strlen(buf)))
                _ui_clr_str (fb, &pitem->r, &pitem->s);

            /* 새로운 string 복사 */
            strncpy(pitem->s.str, buf, strlen(buf));
            switch (pitem->s_align) {
                default :
                case STR_ALIGN_C:
                    pitem->s.x = -1, pitem->s.y = -1;
                    break;
            }
        }
        _ui_str_pos_xy(&pitem->r, &pitem->s);
        _ui_update_s (fb, &pitem->s, pitem->r.x, pitem->r.y);
    }
}

//------------------------------------------------------------------------------
void ui_set_str (fb_info_t *fb, ui_grp_t *ui_grp,
                  int f_id, int x, int y, int scale, int font, char *fmt, ...)
{
   b_item_t *pitem = _ui_find_item (ui_grp, f_id);

   if ((f_id < ITEM_COUNT_MAX) && (pitem != NULL)) {

      va_list va;
      char buf[ITEM_STR_MAX];
      int n_scale = pitem->s.scale;

      /* 받아온 가변인자를 string 형태로 변환 하여 buf에 저장 */
      memset(buf, 0x00, sizeof(buf));
      va_start(va, fmt);   vsprintf(buf, fmt, va); va_end(va);

      if (scale) {
         /* scale = -1 이면 최대 스케일을 구하여 표시한다 */
         if (scale < 0)
            n_scale = _ui_str_scale (pitem->r.w, pitem->r.h, pitem->r.lw,
                                       _my_strlen(buf));
         else
            n_scale = scale;

         if (pitem->s.scale > n_scale)
            _ui_clr_str (fb, &pitem->r, &pitem->s);
      }

      if (font) {
         pitem->s.f_type = (font < 0) ? ui_grp->f_type : font;
         set_font(pitem->s.f_type);
      }

      /*
      기존 문자열 보다 새로운 문자열이 더 작은 경우
      기존 문자열을 배경색으로 덮어 씌운다.
      */
      if ((strlen(pitem->s.str) > strlen(buf)) || n_scale != pitem->s.scale) {
         _ui_clr_str (fb, &pitem->r, &pitem->s);
         pitem->s.scale = n_scale;
      }
      pitem->s.x = (x != 0) ? x : pitem->s.x;
      pitem->s.y = (y != 0) ? y : pitem->s.y;

      /* 새로운 string 복사 */
      strncpy(pitem->s.str, buf, strlen(buf));

      _ui_str_pos_xy(&pitem->r, &pitem->s);
      _ui_update_s (fb, &pitem->s, pitem->r.x, pitem->r.y);
   }
}

//------------------------------------------------------------------------------
static void _ui_update (fb_info_t *fb, ui_grp_t *ui_grp, int id)
{
    b_item_t *pitem = _ui_find_item(ui_grp, id);

    /* popup message */
    if (ui_grp->p_item.timeout) return;

    if ((id < ITEM_COUNT_MAX) && (pitem != NULL)) {
        pitem->s.f_type = ui_grp->f_type;

        if ((signed)pitem->s.bc.uint < 0)
            pitem->s.bc.uint = pitem->r.bc.uint;

        set_font(pitem->s.f_type);

        if (pitem->s.scale < 0)
            pitem->s.scale = _ui_str_scale (pitem->r.w, pitem->r.h, pitem->r.lw,
                                               _my_strlen(pitem->s.str));

        _ui_str_pos_xy(&pitem->r, &pitem->s);
        _ui_update_r (fb, &pitem->r);
        _ui_update_s (fb, &pitem->s, pitem->r.x, pitem->r.y);
    }
}

//------------------------------------------------------------------------------
void ui_set_printf (fb_info_t *fb, ui_grp_t *ui_grp, int id, char *fmt, ...)
{
   va_list va;
   char buf[ITEM_STR_MAX];

   /* 받아온 가변인자를 string 형태로 변환 하여 buf에 저장 */
   memset(buf, 0x00, sizeof(buf));
   va_start(va, fmt);   vsprintf(buf, fmt, va); va_end(va);

   ui_set_str (fb, ui_grp, id, -1, -1, -1, -1, buf);
}

//------------------------------------------------------------------------------
void ui_update (fb_info_t *fb, ui_grp_t *ui_grp, int id)
{
    int i;

    /* popup message */
    if (ui_grp->p_item.timeout) return;

    /* ui_grp에 등록되어있는 모든 item에 대하여 화면 업데이트 함 */
    if (id < 0) {
        /* 모든 item에 대한 화면 업데이트 */
        for (i = 0; i < ITEM_COUNT_MAX; i++)
            _ui_update (fb, ui_grp, i);
    }
    else
        /* id값으로 설정된 1 개의 item에 대한 화면 업데이트 */
        _ui_update (fb, ui_grp, id);
}

//------------------------------------------------------------------------------
void ui_update_group (fb_info_t *fb, ui_grp_t *ui_grp, int gid)
{
   int i;

    /* popup message */
    if (ui_grp->p_item.timeout) return;

    for (i = 0; i < ui_grp->b_item_cnt; i++) {
        if (ui_grp->b_item[i].gid == gid) {
            ui_grp->b_item[i].r.bc.uint = ui_grp->bc.uint;
            ui_grp->b_item[i].r.lc.uint = ui_grp->lc.uint;
            ui_grp->b_item[i].s.bc.uint = ui_grp->bc.uint;
            ui_grp->b_item[i].s.fc.uint = ui_grp->fc.uint;
            ui_grp->b_item[i].s.x = ui_grp->b_item[i].s.y = -1;
            memset  (ui_grp->b_item[i].s.str, 0x00, ITEM_STR_MAX);
            strncpy (ui_grp->b_item[i].s.str, ui_grp->b_item[i].s_dfl, strlen (ui_grp->b_item[i].s_dfl));
            _ui_update (fb, ui_grp, ui_grp->b_item[i].id);
        }
    }
}

//------------------------------------------------------------------------------
void ui_close (ui_grp_t *ui_grp)
{
   /* 할당받은 메모리가 있다면 시스템으로 반환한다. */
   if (ui_grp)
      free (ui_grp);
}

//------------------------------------------------------------------------------
int ui_update_popup (fb_info_t *fb, ui_grp_t *ui_grp);
int ui_update_popup (fb_info_t *fb, ui_grp_t *ui_grp)
{
    p_item_t *p = &ui_grp->p_item;
    _ui_update_r (fb, &p->r);
    _ui_update_s (fb, &p->s, p->r.x, p->r.y);
    return 1;
}

//------------------------------------------------------------------------------
int ui_set_popup (fb_info_t *fb, ui_grp_t *ui_grp,
                    int w, int h, int lw,       /* box width, box height, box outline width */
                    int fc, int bc, int lc,     /* color : font, background, outline */
                    int fs, int ts, char *fmt, ...); /* font scale, display time(sec), msg format */


int ui_set_popup (fb_info_t *fb, ui_grp_t *ui_grp,
                    int w, int h, int lw,       /* width, height, line width */
                    int fc, int bc, int lc,     /* color : font, background, line */
                    int fs, int ts, char *fmt, ...)  /* font scale, display time(sec), msg format */
{
    va_list va;
    char buf[ITEM_STR_MAX];
    p_item_t *p = &ui_grp->p_item;

    /* thread busy */
    if (p->timeout)     return 0;

    // popup message display time setting
    p->timeout = ts;

    if ((w < fb->w) && (w > 0)) {   p->r.w = w;     p->r.x = (fb->w - w) / 2;   }
    else                        {   p->r.w = fb->w; p->r.x = 0;                 }

    if ((h < fb->h) && (h > 0)) {   p->r.h = h;     p->r.y = (fb->h - h) / 2;   }
    else                        {   p->r.h = fb->h; p->r.y = 0;                 }

    p->r.lw = lw < 0 ? 0 : lw;

    p->s.fc.uint = fc < 0 ? ui_grp->fc.uint : (unsigned int)fc;
    p->r.lc.uint = lc < 0 ? ui_grp->lc.uint : (unsigned int)lc;
    p->s.bc.uint = p->r.bc.uint = bc < 0 ? ui_grp->bc.uint : (unsigned int)bc;

    /* 받아온 가변인자를 string 형태로 변환 하여 buf에 저장 */
    memset (buf, 0x00, sizeof(buf));
    va_start(va, fmt);  vsprintf(buf, fmt, va); va_end(va);

    /* 새로운 string 복사 */
    p->s.len = _my_strlen(buf);
    memset (p->s.str, 0, sizeof(p->s.str));
    strncpy(p->s.str, buf, p->s.len);

    p->s.scale = fs < 0 ?
        _ui_str_scale (p->r.w, p->r.h, p->r.lw, p->s.len) : fs;

    p->s.x = -1;    p->s.y = -1;
    _ui_str_pos_xy(&p->r, &p->s);

    return  ui_update_popup(fb, ui_grp);
}

//------------------------------------------------------------------------------
ui_grp_t *ui_init (fb_info_t *fb, const char *cfg_filename)
{
    ui_grp_t *ui_grp;
    FILE *pfd;
    char buf[256], is_cfg_file = 0;

    if ((pfd = fopen(cfg_filename, "r")) == NULL)
        return   NULL;

    if ((ui_grp = (ui_grp_t *)malloc(sizeof(ui_grp_t))) == NULL)
        return   NULL;

    memset (ui_grp, 0x00, sizeof(ui_grp_t));
    memset (buf,    0x00, sizeof(buf));

    while(fgets(buf, sizeof(buf), pfd) != NULL) {
        if (!is_cfg_file) {
            is_cfg_file = strncmp ("ODROID-UI-CONFIG", buf, strlen(buf)-1) == 0 ? 1 : 0;
            memset (buf, 0x00, sizeof(buf));
            continue;
        }
        switch(buf[0]) {
            case  'C':  _ui_parser_cmd_C (buf, fb, ui_grp); break;
            case  'B':  _ui_parser_cmd_B (buf, fb, ui_grp); break;
            case  'I':  _ui_parser_cmd_I (buf, ui_grp);     break;
            case  'T':  _ui_parser_cmd_T (buf, ui_grp);     break;
            case  'R':  _ui_parser_cmd_R (buf, fb, ui_grp); break;
            case  'S':  _ui_parser_cmd_S (buf, ui_grp);     break;
            default :
                fprintf(stdout, "ERROR: Unknown parser command! cmd = %c\n", buf[0]);
            case  '#':  case  '\n':
                break;
        }
        memset (buf, 0x00, sizeof(buf));
    }
    if (!is_cfg_file) {
        fprintf(stdout, "ERROR: UI Config File not found! (filename = %s)\n", cfg_filename);
        free (ui_grp);
        return NULL;
    }
    /* all item update */
    if (ui_grp->b_item_cnt)
        ui_update (fb, ui_grp, -1);
    // cfg file close
    if (pfd)
        fclose (pfd);
    // file parser
    return   ui_grp;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
