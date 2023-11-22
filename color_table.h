//-----------------------------------------------------------------------------
/**
 * @file color_table.h
 * @author charles-park (charles-park@hardkernel.com)
 * @brief framebuffer library header file.
 * @version 0.1
 * @date 2023-11-21
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef __COLOR_TABLE_H__
#define __COLOR_TABLE_H__

//-----------------------------------------------------------------------------
// Color table & convert macro
//-----------------------------------------------------------------------------
#define RGB_TO_UINT(r,g,b)  (((r << 16) | (g << 8) | b) & 0xFFFFFF)
#define UINT_TO_R(i)        ((i >> 16) & 0xFF)
#define UINT_TO_G(i)        ((i >>  8) & 0xFF)
#define UINT_TO_B(i)        ((i      ) & 0xFF)

/*
    https://www.rapidtables.com/web/color/RGB_Color.html
    http://mytreelove.com/bbs/board.php?bo_table=reference&wr_id=47
*/
// Red colors (9)
#define COLOR_INDIAN_RED                RGB_TO_UINT(205, 92, 92)
#define COLOR_LIGHT_CORAL               RGB_TO_UINT(240, 128, 128)
#define COLOR_SALMON                    RGB_TO_UINT(250, 128, 114)
#define COLOR_DARK_SALMON               RGB_TO_UINT(233, 150, 122)
#define COLOR_LIGHT_SALMON              RGB_TO_UINT(255, 150, 122)
#define COLOR_RED                       RGB_TO_UINT(255, 0, 0)
#define COLOR_CRIMSON                   RGB_TO_UINT(220, 20, 60)
#define COLOR_FIRE_BRICK                RGB_TO_UINT(178, 34, 34)
#define COLOR_DARK_RED                  RGB_TO_UINT(139, 0, 0)

// Pink colors (6)
#define COLOR_PINK                      RGB_TO_UINT(255, 192, 203)
#define COLOR_LIGHT_PINK                RGB_TO_UINT(255, 182, 193)
#define COLOR_HOT_PINK                  RGB_TO_UINT(255, 105, 180)
#define COLOR_DEEP_PINK                 RGB_TO_UINT(255, 20, 147)
#define COLOR_MEDIUM_VIOLET_RED         RGB_TO_UINT(199, 21, 133)
#define COLOR_PALE_VIOLET_RED           RGB_TO_UINT(219, 112, 147)

// Orange colors (5)
#define COLOR_CORAL                     RGB_TO_UINT(255, 192, 203)
#define COLOR_TOMATO                    RGB_TO_UINT(255, 192, 203)
#define COLOR_ORANGE_RED                RGB_TO_UINT(255, 192, 203)
#define COLOR_DARK_ORANGE               RGB_TO_UINT(255, 192, 203)
#define COLOR_ORANGE                    RGB_TO_UINT(255, 192, 203)

// Yellow colors (11)
#define COLOR_GOLD                      RGB_TO_UINT(255, 215, 0)
#define COLOR_YELLOW                    RGB_TO_UINT(255, 255, 0)
#define COLOR_LIGHT_YELLOW              RGB_TO_UINT(255, 255, 224)
#define COLOR_LEMON_CHIFFON             RGB_TO_UINT(255, 250, 205)
#define COLOR_LIGHT_GOLDENROD_YELLOW    RGB_TO_UINT(250, 250, 210)
#define COLOR_PAPAYA_WHIP               RGB_TO_UINT(255, 239, 213)
#define COLOR_MOCCASIN                  RGB_TO_UINT(255, 228, 181)
#define COLOR_PEACH_PUFF                RGB_TO_UINT(255, 218, 185)
#define COLOR_PALE_GOLDERNROD           RGB_TO_UINT(238, 232, 170)
#define COLOR_KHAKI                     RGB_TO_UINT(240, 230, 140)
#define COLOR_DARK_KHAKI                RGB_TO_UINT(189, 183, 107)

// Purple colors (18)
#define COLOR_LAVENDER                  RGB_TO_UINT(230, 230, 250)
#define COLOR_THISTLE                   RGB_TO_UINT(216, 191, 216)
#define COLOR_PLUM                      RGB_TO_UINT(221, 160, 221)
#define COLOR_VIOLET                    RGB_TO_UINT(238, 130, 238)
#define COLOR_ORCHID                    RGB_TO_UINT(218, 112, 214)
#define COLOR_FUCHSIA                   RGB_TO_UINT(255, 0, 255)
#define COLOR_MAGENTA                   RGB_TO_UINT(255, 0, 255)
#define COLOR_MEDIUM_ORCHID             RGB_TO_UINT(186, 85, 211)
#define COLOR_MEDIUM_PURPLE             RGB_TO_UINT(147, 112, 219)
#define COLOR_BLUE_VIOLET               RGB_TO_UINT(138, 43, 226)
#define COLOR_DARK_VIOLET               RGB_TO_UINT(148, 0, 211)
#define COLOR_DARK_ORCHID               RGB_TO_UINT(153, 50, 204)
#define COLOR_DARK_MAGENTA              RGB_TO_UINT(139, 0, 139)
#define COLOR_PURPLE                    RGB_TO_UINT(128, 0, 128)
#define COLOR_INDIGO                    RGB_TO_UINT(75, 0, 130)
#define COLOR_DARK_SLATE_BLUE           RGB_TO_UINT(72, 61, 139)
#define COLOR_SLATE_BLUE                RGB_TO_UINT(106, 90, 205)
#define COLOR_MEDIUM_SLATE_BLUE         RGB_TO_UINT(123, 104, 238)

// Green colors (23)
#define COLOR_GREEN_YELLOW              RGB_TO_UINT(173, 255, 47)
#define COLOR_CHARTREUSE                RGB_TO_UINT(127, 255, 0)
#define COLOR_LAWN_GREEN                RGB_TO_UINT(124, 252, 0)
#define COLOR_LIME                      RGB_TO_UINT(0, 255, 0)
#define COLOR_LIME_GREEN                RGB_TO_UINT(50, 205, 50)
#define COLOR_PALE_GREEN                RGB_TO_UINT(152, 251, 152)
#define COLOR_LIGHT_GREEN               RGB_TO_UINT(144, 238, 144)
#define COLOR_MEDIUM_SPRING_GREEN       RGB_TO_UINT(0, 250, 154)
#define COLOR_SPRING_GREEN              RGB_TO_UINT(0, 255, 127)
#define COLOR_MEDIUM_SEA_GREEN          RGB_TO_UINT(60, 179, 113)
#define COLOR_SEA_GREEN                 RGB_TO_UINT(46, 139, 87)
#define COLOR_FOREST_GREEN              RGB_TO_UINT(34, 139, 34)
#define COLOR_GREEN                     RGB_TO_UINT(0, 128, 0)
#define COLOR_DARK_GREEN                RGB_TO_UINT(0, 100, 0)
#define COLOR_YELLOW_GREEN              RGB_TO_UINT(154, 205, 50)
#define COLOR_OLIVE_DRAB                RGB_TO_UINT(107, 142, 35)
#define COLOR_OLIVE                     RGB_TO_UINT(128, 128, 0)
#define COLOR_DARK_OLIVE_GREEN          RGB_TO_UINT(85, 107, 47)
#define COLOR_MEDIUM_AQUAMARINE         RGB_TO_UINT(102, 205, 170)
#define COLOR_DARK_SEA_GREEN            RGB_TO_UINT(143, 188, 143)
#define COLOR_LIGHT_SEA_GREEN           RGB_TO_UINT(32, 178, 170)
#define COLOR_DARK_CYAN                 RGB_TO_UINT(0, 139, 139)
#define COLOR_TEAL                      RGB_TO_UINT(0, 128, 128)

// Blue / Cyan colors (24)
#define COLOR_AQUA                      RGB_TO_UINT(0, 255, 255)
#define COLOR_CYAN                      RGB_TO_UINT(0, 255, 255)
#define COLOR_LIGHT_CYAN                RGB_TO_UINT(224, 255, 255)
#define COLOR_PALE_TURQUOISE            RGB_TO_UINT(175, 238, 238)
#define COLOR_AQUAMARINE                RGB_TO_UINT(127, 255, 212)
#define COLOR_TURQUOISE                 RGB_TO_UINT(64, 224, 208)
#define COLOR_MEDIUM_TURQUOISE          RGB_TO_UINT(72, 209, 204)
#define COLOR_DARK_TURQUIUSE            RGB_TO_UINT(0, 206, 209)
#define COLOR_CADET_BLUE                RGB_TO_UINT(95, 158, 160)
#define COLOR_STEEL_BLUE                RGB_TO_UINT(70, 130, 180)
#define COLOR_LIGHT_STEEL_BLUE          RGB_TO_UINT(176, 196, 222)
#define COLOR_POWDER_BLUE               RGB_TO_UINT(176, 224, 230)
#define COLOR_LIGHT_BLUE                RGB_TO_UINT(173, 216, 230)
#define COLOR_SKY_BLUE                  RGB_TO_UINT(135, 206, 235)
#define COLOR_LIGHT_SKY_BLUE            RGB_TO_UINT(135, 206, 250)
#define COLOR_DEEP_SKY_BLUE             RGB_TO_UINT(0, 191, 255)
#define COLOR_DODGER_BLUE               RGB_TO_UINT(30, 144, 255)
#define COLOR_CORNFLOWER_BLUE           RGB_TO_UINT(100, 149, 237)
#define COLOR_ROYAL_BLUE                RGB_TO_UINT(65, 105, 225)
#define COLOR_BLUE                      RGB_TO_UINT(0, 0, 255)
#define COLOR_MEDIUM_BLUE               RGB_TO_UINT(0, 0, 205)
#define COLOR_DARK_BLUE                 RGB_TO_UINT(0, 0, 139)
#define COLOR_NAVY                      RGB_TO_UINT(0, 0, 128)
#define COLOR_MIDNIGHT_BLUE             RGB_TO_UINT(25, 25, 112)

// Brown colors (17)
#define COLOR_CORNSILK                  RGB_TO_UINT(255, 248, 220)
#define COLOR_BLANCHED_ALMOND           RGB_TO_UINT(255, 235, 205)
#define COLOR_BISQUE                    RGB_TO_UINT(255, 228, 196)
#define COLOR_NAVAJO_WHITE              RGB_TO_UINT(255, 222, 173)
#define COLOR_WHEAT                     RGB_TO_UINT(245, 222, 179)
#define COLOR_BURLY_WOOD                RGB_TO_UINT(222, 184, 135)
#define COLOR_TAN                       RGB_TO_UINT(210, 180, 140)
#define COLOR_ROSY_BROWN                RGB_TO_UINT(188, 143, 143)
#define COLOR_SANDY_BROWN               RGB_TO_UINT(244, 164, 96)
#define COLOR_GOLDENROD                 RGB_TO_UINT(218, 165, 32)
#define COLOR_DARK_GOLDENROD            RGB_TO_UINT(184, 134, 11)
#define COLOR_PERU                      RGB_TO_UINT(205, 133, 63)
#define COLOR_CHOCOLATE                 RGB_TO_UINT(210, 105, 30)
#define COLOR_SADDLE_BROWN              RGB_TO_UINT(139, 69, 19)
#define COLOR_SIENNA                    RGB_TO_UINT(160, 82, 45)
#define COLOR_BROWN                     RGB_TO_UINT(165, 42, 42)
#define COLOR_MAROON                    RGB_TO_UINT(128, 0, 0)

// White colors (17)
#define COLOR_WHITE                     RGB_TO_UINT(255, 255, 255)
#define COLOR_SNOW                      RGB_TO_UINT(255, 250, 250)
#define COLOR_HONEYDEW                  RGB_TO_UINT(240, 255, 240)
#define COLOR_MINT_CREAM                RGB_TO_UINT(245, 255, 250)
#define COLOR_AZURE                     RGB_TO_UINT(240, 255, 255)
#define COLOR_ALICE_BLUE                RGB_TO_UINT(240, 248, 255)
#define COLOR_GHOST_WHITE               RGB_TO_UINT(248, 248, 255)
#define COLOR_WHITE_SMOKE               RGB_TO_UINT(245, 245, 245)
#define COLOR_SEASHELL                  RGB_TO_UINT(255, 245, 238)
#define COLOR_BEIGE                     RGB_TO_UINT(245, 245, 220)
#define COLOR_OLD_LACE                  RGB_TO_UINT(254, 245, 230)
#define COLOR_FLORAL_WHITE              RGB_TO_UINT(255, 250, 240)
#define COLOR_IVORY                     RGB_TO_UINT(255, 255, 240)
#define COLOR_ANTIQUE_WHITE             RGB_TO_UINT(250, 235, 215)
#define COLOR_LINEN                     RGB_TO_UINT(250, 240, 230)
#define COLOR_LAVENDER_BLUSH            RGB_TO_UINT(255, 240, 245)
#define COLOR_MISTY_ROSE                RGB_TO_UINT(255, 228, 225)

// Gray colors (10)
#define COLOR_GAINSBORO                 RGB_TO_UINT(220, 220, 220)
#define COLOR_LIGHT_GRAY                RGB_TO_UINT(211, 211, 211)
#define COLOR_SILVER                    RGB_TO_UINT(192, 192, 192)
#define COLOR_DARK_SILVER               RGB_TO_UINT(169, 169, 169)
#define COLOR_GRAY                      RGB_TO_UINT(128, 128, 128)
#define COLOR_DIM_GRAY                  RGB_TO_UINT(105, 105, 105)
#define COLOR_LIGHT_SLATE_GRAY          RGB_TO_UINT(119, 136, 153)
#define COLOR_SLATE_GRAY                RGB_TO_UINT(112, 128, 144)
#define COLOR_DARK_SLATE_GRAY           RGB_TO_UINT(47, 79, 79)
#define COLOR_BLACK                     RGB_TO_UINT(0, 0, 0)

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
#endif  // #define __COLOR_TABLE_H__
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
