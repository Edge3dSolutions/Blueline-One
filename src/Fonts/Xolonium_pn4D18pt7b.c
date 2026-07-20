/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: --bpp 1 --size 18 --no-compress --stride 1 --align 1 --font Xolonium-pn4D.ttf --symbols ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .:-/%+°_ --format lvgl -o Xolonium_pn4D18pt7b.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef XOLONIUM_PN4D18PT7B
#define XOLONIUM_PN4D18PT7B 1
#endif

#if XOLONIUM_PN4D18PT7B

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0025 "%" */
    0xfe, 0x18, 0x31, 0x8e, 0xc, 0x63, 0x3, 0x19,
    0xc0, 0xc6, 0x60, 0x31, 0xb7, 0xff, 0xed, 0x8c,
    0x6, 0x63, 0x3, 0x98, 0xc0, 0xc6, 0x30, 0x71,
    0x8c, 0x18, 0x7f,

    /* U+002B "+" */
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xff, 0xc3, 0x0,
    0xc0, 0x30, 0xc, 0x0,

    /* U+002D "-" */
    0xfc,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x3, 0x3, 0x81, 0x81, 0xc0, 0xc0, 0x60, 0x60,
    0x30, 0x38, 0x18, 0x1c, 0xc, 0x0,

    /* U+0030 "0" */
    0x7f, 0xb0, 0x3c, 0xf, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0xfe,

    /* U+0031 "1" */
    0xf8, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x30,

    /* U+0032 "2" */
    0x7f, 0x80, 0x30, 0xc, 0x3, 0x0, 0xdf, 0xec,
    0x3, 0x0, 0xc0, 0x30, 0xc, 0x3, 0xff,

    /* U+0033 "3" */
    0xff, 0x80, 0x30, 0xc, 0x3, 0x0, 0xcf, 0xe0,
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xf, 0xfe,

    /* U+0034 "4" */
    0x1, 0xc0, 0xf0, 0x7c, 0x3b, 0x1c, 0xce, 0x37,
    0xf, 0x83, 0xff, 0xc0, 0x30, 0xc, 0x3,

    /* U+0035 "5" */
    0xff, 0xb0, 0xc, 0x3, 0x0, 0xc0, 0x3f, 0xe0,
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xf, 0xfe,

    /* U+0036 "6" */
    0x7f, 0xb0, 0xc, 0x3, 0x0, 0xc0, 0x3f, 0xec,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0xfe,

    /* U+0037 "7" */
    0xff, 0xe0, 0x1c, 0x7, 0x0, 0xe0, 0x38, 0x7,
    0x1, 0xc0, 0x38, 0xe, 0x1, 0x80, 0x70, 0x1c,
    0x0,

    /* U+0038 "8" */
    0x7f, 0xb0, 0x3c, 0xf, 0x3, 0xc0, 0xdf, 0xec,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0xfe,

    /* U+0039 "9" */
    0x7f, 0xb0, 0x3c, 0xf, 0x3, 0xc0, 0xdf, 0xf0,
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xd, 0xfe,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+0041 "A" */
    0x3, 0x80, 0x1e, 0x0, 0x7c, 0x3, 0xb0, 0xc,
    0xe0, 0x73, 0x81, 0xc7, 0xe, 0x1c, 0x3f, 0xf9,
    0xc0, 0x66, 0x1, 0xf8, 0x7,

    /* U+0042 "B" */
    0xff, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0xff,
    0xb0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x1, 0xff,
    0xe0,

    /* U+0043 "C" */
    0x7f, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x18, 0x3, 0x0, 0x3f,
    0xf0,

    /* U+0044 "D" */
    0xff, 0x8c, 0xe, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0xef, 0xf8,

    /* U+0045 "E" */
    0xff, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x1, 0xff,
    0xb0, 0x6, 0x0, 0xc0, 0x18, 0x3, 0x0, 0x7f,
    0xf0,

    /* U+0046 "F" */
    0xff, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x1, 0xff,
    0xb0, 0x6, 0x0, 0xc0, 0x18, 0x3, 0x0, 0x60,
    0x0,

    /* U+0047 "G" */
    0x7f, 0xd8, 0x3, 0x0, 0x60, 0xc, 0x1, 0x87,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x1, 0xbf,
    0xf0,

    /* U+0048 "H" */
    0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3f,
    0xff, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x3c, 0x3,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x0, 0x60, 0xc, 0x1, 0x80, 0x30, 0x6, 0x0,
    0xc0, 0x18, 0x3, 0xc0, 0x78, 0xf, 0x1, 0xbf,
    0xe0,

    /* U+004B "K" */
    0xc1, 0xd8, 0x73, 0x1c, 0x67, 0xd, 0xc1, 0xf8,
    0x3f, 0x87, 0x38, 0xc3, 0x98, 0x73, 0x7, 0x60,
    0x70,

    /* U+004C "L" */
    0xc0, 0x18, 0x3, 0x0, 0x60, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x18, 0x3, 0x0, 0x7f,
    0xf0,

    /* U+004D "M" */
    0xe0, 0xf, 0xe0, 0x3f, 0xc0, 0x7f, 0xc1, 0xff,
    0x83, 0xfb, 0x8e, 0xf7, 0x1d, 0xe6, 0x73, 0xce,
    0xe7, 0x8d, 0x8f, 0x1f, 0x1e, 0x1c, 0x30,

    /* U+004E "N" */
    0xe0, 0x3f, 0x3, 0xf8, 0x3f, 0x83, 0xdc, 0x3c,
    0xe3, 0xc7, 0x3c, 0x3b, 0xc1, 0xfc, 0x1f, 0xc0,
    0xfc, 0x7,

    /* U+004F "O" */
    0x7f, 0xec, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x37, 0xfe,

    /* U+0050 "P" */
    0xff, 0xd8, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xff, 0xf6, 0x0, 0xc0, 0x18, 0x3, 0x0, 0x60,
    0x0,

    /* U+0051 "Q" */
    0x7f, 0xec, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x37, 0xfe, 0x3, 0x80, 0x38, 0x1, 0xc0,

    /* U+0052 "R" */
    0xff, 0xcc, 0x6, 0xc0, 0x6c, 0x6, 0xc0, 0x6c,
    0x6, 0xff, 0xcc, 0x38, 0xc1, 0xcc, 0x1c, 0xc0,
    0xec, 0x6,

    /* U+0053 "S" */
    0x7f, 0xd8, 0x3, 0x0, 0x60, 0xc, 0x0, 0xff,
    0x80, 0x18, 0x3, 0x0, 0x60, 0xc, 0x1, 0xff,
    0xe0,

    /* U+0054 "T" */
    0xff, 0xf8, 0x30, 0x1, 0x80, 0xc, 0x0, 0x60,
    0x3, 0x0, 0x18, 0x0, 0xc0, 0x6, 0x0, 0x30,
    0x1, 0x80, 0xc, 0x0,

    /* U+0055 "U" */
    0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x3c, 0x3, 0xc0,
    0x37, 0xfe,

    /* U+0056 "V" */
    0xe0, 0x1d, 0xc0, 0x77, 0x3, 0x8c, 0xe, 0x38,
    0x70, 0x61, 0xc1, 0xc6, 0x3, 0x38, 0xe, 0xc0,
    0x1f, 0x0, 0x78, 0x0, 0xe0,

    /* U+0057 "W" */
    0x60, 0x70, 0x33, 0x7, 0x81, 0x9c, 0x3e, 0x1c,
    0xe1, 0xb0, 0xc3, 0x1d, 0x86, 0x1c, 0xee, 0x70,
    0xe6, 0x33, 0x83, 0x71, 0x98, 0x1b, 0x8f, 0xc0,
    0xf8, 0x3e, 0x3, 0xc1, 0xe0, 0x1e, 0xf, 0x0,

    /* U+0058 "X" */
    0x70, 0x38, 0xe1, 0xc1, 0x8e, 0x7, 0x30, 0xf,
    0xc0, 0x1e, 0x0, 0x78, 0x3, 0xf0, 0x1c, 0xe0,
    0xe1, 0x83, 0x7, 0x1c, 0xe,

    /* U+0059 "Y" */
    0x70, 0x38, 0xc0, 0xc3, 0x87, 0x7, 0x38, 0xc,
    0xc0, 0x1e, 0x0, 0x78, 0x0, 0xc0, 0x3, 0x0,
    0xc, 0x0, 0x30, 0x0, 0xc0,

    /* U+005A "Z" */
    0xff, 0xe0, 0x1c, 0x7, 0x1, 0xc0, 0x70, 0x1c,
    0x3, 0x80, 0xe0, 0x38, 0xe, 0x3, 0x80, 0x7f,
    0xf0,

    /* U+005F "_" */
    0xff, 0xe0,

    /* U+0061 "a" */
    0x7f, 0x0, 0xc0, 0x60, 0x37, 0xfe, 0xf, 0x7,
    0x83, 0x7f, 0x80,

    /* U+0062 "b" */
    0xc0, 0x60, 0x30, 0x1f, 0xec, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x7f, 0xe0,

    /* U+0063 "c" */
    0x7f, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0x7f,

    /* U+0064 "d" */
    0x1, 0x80, 0xc0, 0x6f, 0xfc, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x6f, 0xf0,

    /* U+0065 "e" */
    0x7f, 0x60, 0xf0, 0x78, 0x3f, 0xfe, 0x3, 0x1,
    0x80, 0x7f, 0x80,

    /* U+0066 "f" */
    0x7e, 0x31, 0xfc, 0x63, 0x18, 0xc6, 0x31, 0x80,

    /* U+0067 "g" */
    0x7f, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0x7f, 0x80, 0xc0, 0x60, 0x3f, 0xf0,

    /* U+0068 "h" */
    0xc0, 0x60, 0x30, 0x1f, 0xec, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0069 "i" */
    0xf3, 0xff, 0xff,

    /* U+006A "j" */
    0x33, 0x3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3e,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0x6c, 0x66, 0x63, 0x61,
    0xf8, 0xec, 0x63, 0x31, 0xd8, 0x70,

    /* U+006C "l" */
    0xff, 0xff, 0xff,

    /* U+006D "m" */
    0xff, 0xfb, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3,
    0xf, 0xc, 0x3c, 0x30, 0xf0, 0xc3, 0xc3, 0xc,

    /* U+006E "n" */
    0xff, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0x80,

    /* U+006F "o" */
    0x7f, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0x7f, 0x0,

    /* U+0070 "p" */
    0xff, 0x60, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xff, 0x60, 0x30, 0x18, 0xc, 0x0,

    /* U+0071 "q" */
    0x7f, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0x7f, 0x80, 0xc0, 0x60, 0x30, 0x18,

    /* U+0072 "r" */
    0x7e, 0x31, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x7e, 0xc0, 0xc0, 0xc0, 0x7e, 0x3, 0x3, 0x3,
    0xfe,

    /* U+0074 "t" */
    0xc6, 0x31, 0xfc, 0x63, 0x18, 0xc6, 0x30, 0xf0,

    /* U+0075 "u" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0x7f, 0x80,

    /* U+0076 "v" */
    0xe0, 0x6c, 0x1d, 0xc3, 0x18, 0xe3, 0x98, 0x37,
    0x7, 0xc0, 0x78, 0xe, 0x0,

    /* U+0077 "w" */
    0xe1, 0x87, 0x63, 0xc6, 0x63, 0xc6, 0x73, 0xce,
    0x36, 0x6c, 0x36, 0x6c, 0x3e, 0x7c, 0x1c, 0x38,
    0x1c, 0x38,

    /* U+0078 "x" */
    0x60, 0xc6, 0x30, 0xec, 0xf, 0x80, 0xe0, 0x3e,
    0xe, 0xe1, 0x8c, 0x60, 0xc0,

    /* U+0079 "y" */
    0xe0, 0x6c, 0x1d, 0xc3, 0x18, 0xe3, 0x98, 0x37,
    0x7, 0xc0, 0x78, 0xe, 0x1, 0xc0, 0x30, 0xe,
    0x1, 0x80,

    /* U+007A "z" */
    0xff, 0x81, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1,
    0xc0, 0xff, 0x80,

    /* U+00B0 "°" */
    0xff, 0x8f, 0x1e, 0x3c, 0x7f, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 92, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 305, .box_w = 18, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 28, .adv_w = 204, .box_w = 10, .box_h = 9, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 40, .adv_w = 127, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 41, .adv_w = 86, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 144, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 204, .box_w = 5, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 204, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 171, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 204, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 86, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 204, .adv_w = 230, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 225, .adv_w = 225, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 213, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 230, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 277, .adv_w = 213, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 204, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 230, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 233, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 346, .adv_w = 84, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 210, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 366, .adv_w = 213, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 196, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 291, .box_w = 15, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 423, .adv_w = 236, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 441, .adv_w = 233, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 219, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 476, .adv_w = 233, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 499, .adv_w = 222, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 219, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 216, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 233, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 230, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 593, .adv_w = 334, .box_w = 21, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 625, .adv_w = 222, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 646, .adv_w = 225, .box_w = 14, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 667, .adv_w = 210, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 684, .adv_w = 179, .box_w = 11, .box_h = 1, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 686, .adv_w = 181, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 697, .adv_w = 190, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 711, .adv_w = 170, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 720, .adv_w = 190, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 734, .adv_w = 184, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 745, .adv_w = 118, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 753, .adv_w = 190, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 768, .adv_w = 190, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 782, .adv_w = 75, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 785, .adv_w = 75, .box_w = 4, .box_h = 16, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 793, .adv_w = 173, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 807, .adv_w = 75, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 810, .adv_w = 271, .box_w = 14, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 826, .adv_w = 190, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 837, .adv_w = 190, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 848, .adv_w = 190, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 863, .adv_w = 190, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 878, .adv_w = 118, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 884, .adv_w = 170, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 893, .adv_w = 121, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 901, .adv_w = 190, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 912, .adv_w = 184, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 925, .adv_w = 256, .box_w = 16, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 943, .adv_w = 176, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 956, .adv_w = 184, .box_w = 11, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 974, .adv_w = 173, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 985, .adv_w = 135, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 7}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x5, 0xb
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 12, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 3, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 45, .range_length = 14, .glyph_id_start = 4,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 65, .range_length = 26, .glyph_id_start = 18,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 95, .range_length = 1, .glyph_id_start = 44,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 97, .range_length = 26, .glyph_id_start = 45,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 1, .glyph_id_start = 71,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 4, 5, 6, 7, 6, 8,
    9, 0, 0, 10, 11, 12, 0, 0,
    13, 14, 13, 0, 15, 16, 10, 16,
    17, 11, 18, 19, 0, 20, 21, 22,
    0, 21, 23, 24, 20, 0, 25, 26,
    0, 20, 20, 21, 21, 24, 27, 28,
    22, 29, 30, 31, 26, 30, 1, 32
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 4, 0, 5, 0, 0, 0,
    5, 0, 0, 6, 0, 0, 0, 0,
    5, 0, 5, 0, 0, 7, 8, 7,
    9, 10, 11, 12, 0, 13, 0, 14,
    14, 14, 0, 14, 0, 0, 15, 0,
    0, 16, 16, 14, 16, 14, 17, 18,
    0, 16, 19, 20, 21, 19, 22, 23
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, -12, 0,
    -6, 0, -14, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -23, 0, -14,
    0, -26, 0, 0, 0, 0, 0, 0,
    0, -9, -6, 0, 0, 0, 0, 0,
    -23, -23, 0, -12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -35, -3, -26, 0, -40,
    0, 0, 0, 0, 0, 0, 0, -23,
    -12, 0, 0, -23, 0, 0, 0, 0,
    0, 0, -3, 0, -3, 0, -6, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, -9, 0, 0,
    -9, 0, -6, -9, -12, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -20, -20, -26, 0, -9, 3,
    0, 0, 0, 0, 0, -9, -6, 0,
    -6, -6, -3, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, -9, 0,
    -9, 0, -12, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -12, -6, 0, 0, 0, 0, 0, 0,
    6, -3, 0, -35, -3, -17, 0, -37,
    0, 0, 0, 0, 0, 0, 0, -14,
    -6, 0, 0, -14, 0, 0, 0, -3,
    0, 0, -3, 0, -3, -3, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -20, -20, -20, 0,
    -3, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -9, 0, -9, 0, -12, 0, 0, 0,
    0, 0, 0, 0, -6, -3, -3, 0,
    0, -12, -23, -23, -35, -3, -23, 6,
    0, 3, 0, 0, 0, -17, -17, 0,
    -14, -17, -14, -12, -12, -9, -12, 0,
    -6, -14, -14, -26, -3, -9, 3, 0,
    0, 0, 0, 0, -12, -12, 0, -9,
    -12, -9, -6, -6, -3, -6, 0, -14,
    -26, -26, -40, -6, -29, 0, 0, 0,
    0, 0, 0, -20, -20, 0, -17, -20,
    -17, -12, -12, -9, -14, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -17, 0, -12, 0, -20,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -17, 0, -12, 0, -20, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, -3, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -9, -9, -14, 0, -3,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -14,
    0, -9, 0, -17, 0, 0, 0, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, -3,
    0, -9, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -9,
    -9, -14, 0, -3, -3, 0, 0, -3,
    -6, -6, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -17, 0, -12, 0, -20,
    0, 0, 0, 0, 0, 0, 0, -6,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -14, 0, -9, 0, -17, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -9, -9, -23, 0,
    -3, -12, 0, -6, -12, -12, -6, -6,
    -3, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, -6, -6, -12, 0, 0,
    -12, 0, -6, -6, -12, -3, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -23, 0, -12, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 32,
    .right_class_cnt     = 23,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 6,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};

extern const lv_font_t Xolonium_pn4D18pt7b;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Xolonium_pn4D18pt7b = {
#else
lv_font_t Xolonium_pn4D18pt7b = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &Xolonium_pn4D18pt7b,
#endif
    .user_data = NULL,
};



#endif /*#if XOLONIUM_PN4D18PT7B*/
