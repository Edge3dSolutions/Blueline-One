/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --no-compress --stride 1 --align 1 --font Xolonium-pn4D.ttf --symbols ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .:-/%+°_ --format lvgl -o Xolonium_pn4D12pt7b.c
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



#ifndef XOLONIUM_PN4D12PT7B
#define XOLONIUM_PN4D12PT7B 1
#endif

#if XOLONIUM_PN4D12PT7B

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0025 "%" */
    0xf0, 0x89, 0x10, 0x93, 0x9, 0x2f, 0xf4, 0x90,
    0xc9, 0x8, 0x91, 0xf,

    /* U+002B "+" */
    0x30, 0xcf, 0xcc, 0x30, 0xc0,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x8, 0x61, 0x8c, 0x31, 0x86, 0x10,

    /* U+0030 "0" */
    0xff, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xff,

    /* U+0031 "1" */
    0xf3, 0x33, 0x33, 0x33,

    /* U+0032 "2" */
    0x7e, 0xc, 0x1f, 0xfc, 0x18, 0x30, 0x7f,

    /* U+0033 "3" */
    0xfe, 0xc, 0x1b, 0xe0, 0x60, 0xc1, 0xff,

    /* U+0034 "4" */
    0x7, 0xf, 0x1b, 0x33, 0x63, 0xff, 0x3, 0x3,

    /* U+0035 "5" */
    0xfd, 0x83, 0x7, 0xf0, 0x60, 0xc1, 0xff,

    /* U+0036 "6" */
    0xfd, 0x83, 0x7, 0xfc, 0x78, 0xf1, 0xff,

    /* U+0037 "7" */
    0xfe, 0xc, 0x30, 0x61, 0x83, 0xc, 0x18,

    /* U+0038 "8" */
    0xff, 0x8f, 0x1b, 0xec, 0x78, 0xf1, 0xff,

    /* U+0039 "9" */
    0xff, 0x8f, 0x1f, 0xf0, 0x60, 0xc1, 0xbf,

    /* U+003A ":" */
    0xc0, 0x30,

    /* U+0041 "A" */
    0xc, 0xe, 0x5, 0x86, 0x42, 0x33, 0xf9, 0x87,
    0x81,

    /* U+0042 "B" */
    0xff, 0xc3, 0xc3, 0xfe, 0xc3, 0xc3, 0xc3, 0xff,

    /* U+0043 "C" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff,

    /* U+0044 "D" */
    0xfe, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xfe,

    /* U+0045 "E" */
    0xff, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0, 0xc0, 0xff,

    /* U+0046 "F" */
    0xff, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0xfe, 0xc0, 0xc0, 0xcf, 0xc3, 0xc3, 0xc3, 0xff,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0049 "I" */
    0xff, 0xff,

    /* U+004A "J" */
    0x6, 0xc, 0x18, 0x30, 0x78, 0xf1, 0xff,

    /* U+004B "K" */
    0xc6, 0xcc, 0xd8, 0xf8, 0xe8, 0xcc, 0xc6, 0xc3,

    /* U+004C "L" */
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30, 0x7f,

    /* U+004D "M" */
    0xe1, 0xf8, 0x7e, 0x1f, 0xcf, 0xd2, 0xf7, 0xbc,
    0xcf, 0x33,

    /* U+004E "N" */
    0xe3, 0xe3, 0xf3, 0xd3, 0xcb, 0xcf, 0xc7, 0xc7,

    /* U+004F "O" */
    0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,

    /* U+0050 "P" */
    0xff, 0xc3, 0xc3, 0xc3, 0xff, 0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,
    0x8, 0xc,

    /* U+0052 "R" */
    0xff, 0xc3, 0xc3, 0xc3, 0xff, 0xcc, 0xc6, 0xc3,

    /* U+0053 "S" */
    0xfe, 0xc0, 0xc0, 0xff, 0x3, 0x3, 0x3, 0xff,

    /* U+0054 "T" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,

    /* U+0056 "V" */
    0xc1, 0xb0, 0xd8, 0x46, 0x63, 0x20, 0xb0, 0x70,
    0x18,

    /* U+0057 "W" */
    0xc3, 0xd, 0x1c, 0x26, 0x79, 0x99, 0x66, 0x2c,
    0x90, 0xf3, 0xc3, 0x8f, 0x6, 0x18,

    /* U+0058 "X" */
    0x61, 0x99, 0x8d, 0x83, 0x81, 0xc1, 0xb1, 0x8c,
    0xc3,

    /* U+0059 "Y" */
    0x61, 0x98, 0x63, 0x30, 0x78, 0xc, 0x3, 0x0,
    0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0x7, 0xe, 0x1c, 0x38, 0x70, 0xe0, 0xff,

    /* U+005F "_" */
    0xfe,

    /* U+0061 "a" */
    0x7c, 0x3f, 0xf3, 0xcf, 0xf0,

    /* U+0062 "b" */
    0xc1, 0x83, 0xfe, 0x3c, 0x78, 0xf1, 0xff,

    /* U+0063 "c" */
    0xff, 0xc, 0x30, 0xc3, 0xf0,

    /* U+0064 "d" */
    0x6, 0xf, 0xfe, 0x3c, 0x78, 0xf1, 0xff,

    /* U+0065 "e" */
    0xff, 0x3f, 0xf0, 0xc3, 0xf0,

    /* U+0066 "f" */
    0xfc, 0xfc, 0xcc, 0xcc,

    /* U+0067 "g" */
    0xff, 0x8f, 0x1e, 0x3c, 0x7f, 0xc1, 0x83, 0xfe,

    /* U+0068 "h" */
    0xc1, 0x83, 0xfe, 0x3c, 0x78, 0xf1, 0xe3,

    /* U+0069 "i" */
    0xcf, 0xff,

    /* U+006A "j" */
    0x61, 0xb6, 0xdb, 0x6f, 0x80,

    /* U+006B "k" */
    0xc1, 0x83, 0x36, 0xcf, 0x1f, 0x33, 0x62,

    /* U+006C "l" */
    0xff, 0xff,

    /* U+006D "m" */
    0xff, 0xf3, 0x3c, 0xcf, 0x33, 0xcc, 0xf3, 0x30,

    /* U+006E "n" */
    0xff, 0x8f, 0x1e, 0x3c, 0x78, 0xc0,

    /* U+006F "o" */
    0xff, 0x8f, 0x1e, 0x3c, 0x7f, 0xc0,

    /* U+0070 "p" */
    0xff, 0x8f, 0x1e, 0x3c, 0x7f, 0xf0, 0x60, 0xc0,

    /* U+0071 "q" */
    0xff, 0x8f, 0x1e, 0x3c, 0x7f, 0xc1, 0x83, 0x6,

    /* U+0072 "r" */
    0xfc, 0xcc, 0xcc,

    /* U+0073 "s" */
    0xfb, 0xf, 0xc3, 0xf, 0xf0,

    /* U+0074 "t" */
    0xcc, 0xfc, 0xcc, 0xcf,

    /* U+0075 "u" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x7f, 0xc0,

    /* U+0076 "v" */
    0xc2, 0x46, 0x64, 0x2c, 0x38, 0x18,

    /* U+0077 "w" */
    0xcc, 0x49, 0xd9, 0xab, 0x3d, 0x43, 0x38, 0x63,
    0x0,

    /* U+0078 "x" */
    0x46, 0x58, 0xe1, 0xc6, 0xc8, 0xc0,

    /* U+0079 "y" */
    0xc2, 0x46, 0x64, 0x2c, 0x38, 0x18, 0x18, 0x10,
    0x30,

    /* U+007A "z" */
    0xfc, 0x31, 0x8, 0xc3, 0xf0,

    /* U+00B0 "°" */
    0xf9, 0x9f
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 61, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 204, .box_w = 12, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 136, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 18, .adv_w = 84, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 19, .adv_w = 58, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 20, .adv_w = 96, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 26, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 33, .adv_w = 136, .box_w = 4, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 44, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 51, .adv_w = 136, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 73, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 136, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 58, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 154, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 150, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 154, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 136, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 154, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 56, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 140, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 178, .adv_w = 131, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 194, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 157, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 146, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 156, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 229, .adv_w = 148, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 146, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 154, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 223, .box_w = 14, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 284, .adv_w = 148, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 150, .box_w = 10, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 140, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 119, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 312, .adv_w = 121, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 113, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 329, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 123, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 79, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 353, .adv_w = 127, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 50, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 362, .adv_w = 50, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 367, .adv_w = 115, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 50, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 180, .box_w = 10, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 127, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 390, .adv_w = 127, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 404, .adv_w = 127, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 412, .adv_w = 79, .box_w = 4, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 113, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 81, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 127, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 123, .box_w = 8, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 171, .box_w = 11, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 117, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 451, .adv_w = 123, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 460, .adv_w = 115, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 465, .adv_w = 90, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 4}
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
    0, 0, 0, 0, 0, 0, -8, 0,
    -4, 0, -10, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, -10,
    0, -17, 0, 0, 0, 0, 0, 0,
    0, -6, -4, 0, 0, 0, 0, 0,
    -15, -15, 0, -8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -23, -2, -17, 0, -27,
    0, 0, 0, 0, 0, 0, 0, -15,
    -8, 0, 0, -15, 0, 0, 0, 0,
    0, 0, -2, 0, -2, 0, -4, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    -6, 0, -4, -6, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -13, -13, -17, 0, -6, 2,
    0, 0, 0, 0, 0, -6, -4, 0,
    -4, -4, -2, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, -6, 0,
    -6, 0, -8, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, -4, 0, 0, 0, 0, 0, 0,
    4, -2, 0, -23, -2, -12, 0, -25,
    0, 0, 0, 0, 0, 0, 0, -10,
    -4, 0, 0, -10, 0, 0, 0, -2,
    0, 0, -2, 0, -2, -2, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -13, -13, -13, 0,
    -2, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, -6, 0, -8, 0, 0, 0,
    0, 0, 0, 0, -4, -2, -2, 0,
    0, -8, -15, -15, -23, -2, -15, 4,
    0, 2, 0, 0, 0, -12, -12, 0,
    -10, -12, -10, -8, -8, -6, -8, 0,
    -4, -10, -10, -17, -2, -6, 2, 0,
    0, 0, 0, 0, -8, -8, 0, -6,
    -8, -6, -4, -4, -2, -4, 0, -10,
    -17, -17, -27, -4, -19, 0, 0, 0,
    0, 0, 0, -13, -13, 0, -12, -13,
    -12, -8, -8, -6, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -12, 0, -8, 0, -13,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -12, 0, -8, 0, -13, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -2, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -6, -10, 0, -2,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -10,
    0, -6, 0, -12, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -2,
    0, -6, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -6, -10, 0, -2, -2, 0, 0, -2,
    -4, -4, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -12, 0, -8, 0, -13,
    0, 0, 0, 0, 0, 0, 0, -4,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -10, 0, -6, 0, -12, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, -6, -15, 0,
    -2, -8, 0, -4, -8, -8, -4, -4,
    -2, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, -4, -4, -8, 0, 0,
    -8, 0, -4, -4, -8, -2, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -15, 0, -8, 0,
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



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Xolonium_pn4D12pt7b = {
#else
lv_font_t Xolonium_pn4D12pt7b = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if XOLONIUM_PN4D12PT7B*/
