/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: --bpp 1 --size 8 --no-compress --font Minecraftia-Regular_8.ttf --range 32-255 --format lvgl -o font_8.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONT_8
#define FONT_8 1
#endif

#if FONT_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfa,

    /* U+0022 "\"" */
    0xb4,

    /* U+0023 "#" */
    0x52, 0xbe, 0xaf, 0xa9, 0x40,

    /* U+0024 "$" */
    0x23, 0xe0, 0xe0, 0xf8, 0x80,

    /* U+0025 "%" */
    0x8c, 0x84, 0x44, 0x26, 0x20,

    /* U+0026 "&" */
    0x22, 0x88, 0xdb, 0x49, 0xa0,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0x34, 0x88, 0x84, 0x30,

    /* U+0029 ")" */
    0xc2, 0x11, 0x12, 0xc0,

    /* U+002A "*" */
    0x96, 0x90,

    /* U+002B "+" */
    0x21, 0x3e, 0x42, 0x0,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x8, 0x84, 0x44, 0x22, 0x0,

    /* U+0030 "0" */
    0x74, 0x67, 0x5c, 0xc5, 0xc0,

    /* U+0031 "1" */
    0x23, 0x8, 0x42, 0x13, 0xe0,

    /* U+0032 "2" */
    0x74, 0x42, 0x64, 0x43, 0xe0,

    /* U+0033 "3" */
    0x74, 0x42, 0x60, 0xc5, 0xc0,

    /* U+0034 "4" */
    0x19, 0x53, 0x1f, 0x84, 0x20,

    /* U+0035 "5" */
    0xfc, 0x3c, 0x10, 0xc5, 0xc0,

    /* U+0036 "6" */
    0x32, 0x21, 0xe8, 0xc5, 0xc0,

    /* U+0037 "7" */
    0xfc, 0x42, 0x22, 0x10, 0x80,

    /* U+0038 "8" */
    0x74, 0x62, 0xe8, 0xc5, 0xc0,

    /* U+0039 "9" */
    0x74, 0x62, 0xf0, 0x89, 0x80,

    /* U+003A ":" */
    0xcc,

    /* U+003B ";" */
    0xce,

    /* U+003C "<" */
    0x12, 0x48, 0x42, 0x10,

    /* U+003D "=" */
    0xf8, 0x1, 0xf0,

    /* U+003E ">" */
    0x84, 0x21, 0x24, 0x80,

    /* U+003F "?" */
    0x74, 0x42, 0x22, 0x0, 0x80,

    /* U+0040 "@" */
    0x7a, 0x1b, 0x6d, 0xbe, 0x7, 0xc0,

    /* U+0041 "A" */
    0x74, 0x7f, 0x18, 0xc6, 0x20,

    /* U+0042 "B" */
    0xf4, 0x7d, 0x18, 0xc7, 0xc0,

    /* U+0043 "C" */
    0x74, 0x61, 0x8, 0x45, 0xc0,

    /* U+0044 "D" */
    0xf4, 0x63, 0x18, 0xc7, 0xc0,

    /* U+0045 "E" */
    0xfc, 0x39, 0x8, 0x43, 0xe0,

    /* U+0046 "F" */
    0xfc, 0x39, 0x8, 0x42, 0x0,

    /* U+0047 "G" */
    0x7c, 0x2f, 0x18, 0xc5, 0xc0,

    /* U+0048 "H" */
    0x8c, 0x7f, 0x18, 0xc6, 0x20,

    /* U+0049 "I" */
    0xe9, 0x24, 0xb8,

    /* U+004A "J" */
    0x8, 0x42, 0x10, 0xc5, 0xc0,

    /* U+004B "K" */
    0x8c, 0xb9, 0x28, 0xc6, 0x20,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x43, 0xe0,

    /* U+004D "M" */
    0x8e, 0xeb, 0x18, 0xc6, 0x20,

    /* U+004E "N" */
    0x8e, 0x6b, 0x38, 0xc6, 0x20,

    /* U+004F "O" */
    0x74, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0050 "P" */
    0xf4, 0x7d, 0x8, 0x42, 0x0,

    /* U+0051 "Q" */
    0x74, 0x63, 0x18, 0xc9, 0xa0,

    /* U+0052 "R" */
    0xf4, 0x7d, 0x18, 0xc6, 0x20,

    /* U+0053 "S" */
    0x7c, 0x1c, 0x10, 0xc5, 0xc0,

    /* U+0054 "T" */
    0xf9, 0x8, 0x42, 0x10, 0x80,

    /* U+0055 "U" */
    0x8c, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0056 "V" */
    0x8c, 0x63, 0x15, 0x28, 0x80,

    /* U+0057 "W" */
    0x8c, 0x63, 0x1a, 0xee, 0x20,

    /* U+0058 "X" */
    0x8a, 0x88, 0xa8, 0xc6, 0x20,

    /* U+0059 "Y" */
    0x8a, 0x88, 0x42, 0x10, 0x80,

    /* U+005A "Z" */
    0xf8, 0x44, 0x44, 0x43, 0xe0,

    /* U+005B "[" */
    0xf2, 0x49, 0x38,

    /* U+005C "\\" */
    0x82, 0x10, 0x41, 0x8, 0x20,

    /* U+005D "]" */
    0xe4, 0x92, 0x78,

    /* U+005E "^" */
    0x22, 0xa2,

    /* U+005F "_" */
    0xf8,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x70, 0x5f, 0x17, 0x80,

    /* U+0062 "b" */
    0x84, 0x2d, 0x98, 0xc7, 0xc0,

    /* U+0063 "c" */
    0x74, 0x61, 0x17, 0x0,

    /* U+0064 "d" */
    0x8, 0x5b, 0x38, 0xc5, 0xe0,

    /* U+0065 "e" */
    0x74, 0x7f, 0x7, 0x80,

    /* U+0066 "f" */
    0x34, 0xf4, 0x44, 0x40,

    /* U+0067 "g" */
    0x7c, 0x62, 0xf0, 0xf8,

    /* U+0068 "h" */
    0x84, 0x2d, 0x98, 0xc6, 0x20,

    /* U+0069 "i" */
    0xbe,

    /* U+006A "j" */
    0x8, 0x2, 0x10, 0xc6, 0x2e,

    /* U+006B "k" */
    0x88, 0x9a, 0xca, 0x90,

    /* U+006C "l" */
    0xaa, 0xa4,

    /* U+006D "m" */
    0xd5, 0x6b, 0x18, 0x80,

    /* U+006E "n" */
    0xf4, 0x63, 0x18, 0x80,

    /* U+006F "o" */
    0x74, 0x63, 0x17, 0x0,

    /* U+0070 "p" */
    0xb6, 0x63, 0xe8, 0x40,

    /* U+0071 "q" */
    0x6c, 0xe2, 0xf0, 0x84,

    /* U+0072 "r" */
    0xb6, 0x61, 0x8, 0x0,

    /* U+0073 "s" */
    0x7c, 0x1c, 0x1f, 0x0,

    /* U+0074 "t" */
    0x5d, 0x24, 0x88,

    /* U+0075 "u" */
    0x8c, 0x63, 0x17, 0x80,

    /* U+0076 "v" */
    0x8c, 0x62, 0xa2, 0x0,

    /* U+0077 "w" */
    0x8c, 0x6b, 0x57, 0x80,

    /* U+0078 "x" */
    0x8a, 0x88, 0xa8, 0x80,

    /* U+0079 "y" */
    0x8c, 0x62, 0xf0, 0xf8,

    /* U+007A "z" */
    0xf8, 0x88, 0x8f, 0x80,

    /* U+007B "{" */
    0x34, 0x48, 0x44, 0x30,

    /* U+007C "|" */
    0xff,

    /* U+007D "}" */
    0xc2, 0x21, 0x22, 0xc0,

    /* U+007E "~" */
    0x66, 0x60,

    /* U+00A1 "¡" */
    0xbe,

    /* U+00A2 "¢" */
    0x27, 0x87, 0x20,

    /* U+00A3 "£" */
    0x3a, 0x11, 0xe4, 0x23, 0xe0,

    /* U+00A4 "¤" */
    0x8b, 0x94, 0xe8, 0x80,

    /* U+00A5 "¥" */
    0x8a, 0x88, 0xe2, 0x38, 0x80,

    /* U+00A6 "¦" */
    0xee,

    /* U+00A7 "§" */
    0x7c, 0x1d, 0x17, 0x7, 0xc0,

    /* U+00A8 "¨" */
    0xa0,

    /* U+00A9 "©" */
    0x7d, 0x6, 0x6d, 0x19, 0xb0, 0x5f, 0x0,

    /* U+00AA "ª" */
    0xce, 0xb1, 0xc0,

    /* U+00AB "«" */
    0x5a, 0xa5,

    /* U+00AC "¬" */
    0xf1, 0x10,

    /* U+00AE "®" */
    0x7d, 0x6, 0xed, 0x9a, 0xb0, 0x5f, 0x0,

    /* U+00AF "¯" */
    0xf8,

    /* U+00B0 "°" */
    0x55, 0x0,

    /* U+00B1 "±" */
    0x21, 0x3e, 0x42, 0x3, 0xe0,

    /* U+00B2 "²" */
    0xc5, 0x70,

    /* U+00B3 "³" */
    0xcc, 0xe0,

    /* U+00B4 "´" */
    0x60,

    /* U+00B5 "µ" */
    0x8c, 0x63, 0x3e, 0xc0,

    /* U+00B6 "¶" */
    0x7f, 0x7a, 0xd0, 0x84, 0x20,

    /* U+00B7 "·" */
    0x80,

    /* U+00B8 "¸" */
    0x70,

    /* U+00B9 "¹" */
    0x59, 0x70,

    /* U+00BA "º" */
    0x56, 0xa1, 0xc0,

    /* U+00BB "»" */
    0xa5, 0x5a,

    /* U+00BC "¼" */
    0x42, 0xc4, 0x44, 0xeb, 0x15, 0x17, 0x21,

    /* U+00BD "½" */
    0x42, 0xc4, 0x44, 0xee, 0x11, 0x12, 0x27,

    /* U+00BE "¾" */
    0xc2, 0x64, 0x24, 0xcb, 0x15, 0x17, 0x21,

    /* U+00BF "¿" */
    0x20, 0x8, 0x88, 0x45, 0xc0,

    /* U+00C0 "À" */
    0x41, 0x1d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C1 "Á" */
    0x11, 0x1d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C2 "Â" */
    0x22, 0x9d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C3 "Ã" */
    0x6d, 0x9d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C4 "Ä" */
    0x50, 0x1d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C5 "Å" */
    0x72, 0x9d, 0x1f, 0xc6, 0x31, 0x88,

    /* U+00C6 "Æ" */
    0x7f, 0xc4, 0x3f, 0x91, 0x8, 0x84, 0x42, 0x3e,

    /* U+00C7 "Ç" */
    0x74, 0x61, 0x8, 0x45, 0xc4, 0x60,

    /* U+00C8 "È" */
    0x41, 0x3f, 0xe, 0x42, 0x10, 0xf8,

    /* U+00C9 "É" */
    0x11, 0x3f, 0xe, 0x42, 0x10, 0xf8,

    /* U+00CA "Ê" */
    0x22, 0xbf, 0xe, 0x42, 0x10, 0xf8,

    /* U+00CB "Ë" */
    0x50, 0x3f, 0xe, 0x42, 0x10, 0xf8,

    /* U+00CC "Ì" */
    0x8b, 0xa4, 0x92, 0xe0,

    /* U+00CD "Í" */
    0x2b, 0xa4, 0x92, 0xe0,

    /* U+00CE "Î" */
    0x57, 0xa4, 0x92, 0xe0,

    /* U+00CF "Ï" */
    0xa3, 0xa4, 0x92, 0xe0,

    /* U+00D0 "Ð" */
    0x79, 0x1e, 0x51, 0x45, 0x17, 0x80,

    /* U+00D1 "Ñ" */
    0x6d, 0xa3, 0x9a, 0xce, 0x31, 0x88,

    /* U+00D2 "Ò" */
    0x41, 0x1d, 0x18, 0xc6, 0x31, 0x70,

    /* U+00D3 "Ó" */
    0x11, 0x1d, 0x18, 0xc6, 0x31, 0x70,

    /* U+00D4 "Ô" */
    0x22, 0x9d, 0x18, 0xc6, 0x31, 0x70,

    /* U+00D5 "Õ" */
    0x6d, 0x9d, 0x18, 0xc6, 0x31, 0x70,

    /* U+00D6 "Ö" */
    0x50, 0x1d, 0x18, 0xc6, 0x31, 0x70,

    /* U+00D7 "×" */
    0x96, 0x69,

    /* U+00D8 "Ø" */
    0x6c, 0xa7, 0x5c, 0xa6, 0xc0,

    /* U+00D9 "Ù" */
    0x41, 0x23, 0x18, 0xc6, 0x31, 0x70,

    /* U+00DA "Ú" */
    0x11, 0x23, 0x18, 0xc6, 0x31, 0x70,

    /* U+00DB "Û" */
    0x22, 0xa3, 0x18, 0xc6, 0x31, 0x70,

    /* U+00DC "Ü" */
    0x50, 0x23, 0x18, 0xc6, 0x31, 0x70,

    /* U+00DD "Ý" */
    0x11, 0x22, 0xa2, 0x10, 0x84, 0x20,

    /* U+00DE "Þ" */
    0x87, 0xa3, 0xe8, 0x42, 0x0,

    /* U+00DF "ß" */
    0x64, 0xad, 0x18, 0xc6, 0xc0,

    /* U+00E0 "à" */
    0x41, 0x0, 0xe0, 0xbe, 0x2f,

    /* U+00E1 "á" */
    0x11, 0x0, 0xe0, 0xbe, 0x2f,

    /* U+00E2 "â" */
    0x22, 0x80, 0xe0, 0xbe, 0x2f,

    /* U+00E3 "ã" */
    0x6d, 0x80, 0xe0, 0xbe, 0x2f,

    /* U+00E4 "ä" */
    0x50, 0x1c, 0x17, 0xc5, 0xe0,

    /* U+00E5 "å" */
    0x72, 0x9c, 0x7, 0x5, 0xf1, 0x78,

    /* U+00E6 "æ" */
    0x77, 0x4, 0x5f, 0xf1, 0x7, 0x70,

    /* U+00E7 "ç" */
    0x74, 0x61, 0x17, 0x11, 0x80,

    /* U+00E8 "è" */
    0x41, 0x0, 0xe8, 0xfe, 0xf,

    /* U+00E9 "é" */
    0x11, 0x0, 0xe8, 0xfe, 0xf,

    /* U+00EA "ê" */
    0x22, 0x80, 0xe8, 0xfe, 0xf,

    /* U+00EB "ë" */
    0x50, 0x1d, 0x1f, 0xc1, 0xe0,

    /* U+00EC "ì" */
    0x91, 0x55,

    /* U+00ED "í" */
    0x62, 0xaa,

    /* U+00EE "î" */
    0x54, 0x24, 0x92,

    /* U+00EF "ï" */
    0xa1, 0x24, 0x90,

    /* U+00F0 "ð" */
    0x28, 0x8a, 0x17, 0xc5, 0xc0,

    /* U+00F1 "ñ" */
    0x6d, 0x81, 0xe8, 0xc6, 0x31,

    /* U+00F2 "ò" */
    0x41, 0x0, 0xe8, 0xc6, 0x2e,

    /* U+00F3 "ó" */
    0x11, 0x0, 0xe8, 0xc6, 0x2e,

    /* U+00F4 "ô" */
    0x22, 0x80, 0xe8, 0xc6, 0x2e,

    /* U+00F5 "õ" */
    0x6d, 0x80, 0xe8, 0xc6, 0x2e,

    /* U+00F6 "ö" */
    0x50, 0x1d, 0x18, 0xc5, 0xc0,

    /* U+00F7 "÷" */
    0x20, 0x3e, 0x2, 0x0,

    /* U+00F8 "ø" */
    0x6c, 0xaa, 0x9b, 0x0,

    /* U+00F9 "ù" */
    0x41, 0x1, 0x18, 0xc6, 0x2f,

    /* U+00FA "ú" */
    0x11, 0x1, 0x18, 0xc6, 0x2f,

    /* U+00FB "û" */
    0x22, 0x81, 0x18, 0xc6, 0x2f,

    /* U+00FC "ü" */
    0x50, 0x23, 0x18, 0xc5, 0xe0,

    /* U+00FD "ý" */
    0x11, 0x1, 0x18, 0xc5, 0xe1, 0xf0,

    /* U+00FE "þ" */
    0x84, 0x2d, 0x98, 0xfa, 0x10,

    /* U+00FF "ÿ" */
    0x8c, 0x62, 0xf0, 0xf8
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 32, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 32, .box_w = 1, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 2, .adv_w = 64, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 3, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 8, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 13, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 18, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 23, .adv_w = 32, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 24, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 28, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 32, .adv_w = 80, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 9},
    {.bitmap_index = 34, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 38, .adv_w = 32, .box_w = 1, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 39, .adv_w = 96, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 40, .adv_w = 32, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 41, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 46, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 51, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 56, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 61, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 66, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 71, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 76, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 81, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 86, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 91, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 96, .adv_w = 32, .box_w = 1, .box_h = 6, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 97, .adv_w = 32, .box_w = 1, .box_h = 7, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 98, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 102, .adv_w = 96, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 105, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 109, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 114, .adv_w = 112, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 120, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 125, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 130, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 135, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 140, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 145, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 150, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 155, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 160, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 163, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 168, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 173, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 178, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 183, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 188, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 193, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 198, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 203, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 208, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 213, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 218, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 223, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 228, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 233, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 238, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 243, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 248, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 251, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 256, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 259, .adv_w = 96, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 9},
    {.bitmap_index = 261, .adv_w = 96, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 262, .adv_w = 48, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 11},
    {.bitmap_index = 263, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 267, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 272, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 276, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 281, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 285, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 289, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 293, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 298, .adv_w = 32, .box_w = 1, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 299, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 304, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 308, .adv_w = 48, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 310, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 314, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 318, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 322, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 326, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 330, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 334, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 338, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 341, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 345, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 349, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 353, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 357, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 361, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 365, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 369, .adv_w = 32, .box_w = 1, .box_h = 8, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 370, .adv_w = 80, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 374, .adv_w = 112, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 376, .adv_w = 32, .box_w = 1, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 377, .adv_w = 80, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 380, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 385, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 389, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 394, .adv_w = 32, .box_w = 1, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 395, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 400, .adv_w = 80, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 11},
    {.bitmap_index = 401, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 408, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 411, .adv_w = 80, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 413, .adv_w = 80, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 415, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 422, .adv_w = 96, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 11},
    {.bitmap_index = 423, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 9},
    {.bitmap_index = 425, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 430, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 432, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 434, .adv_w = 48, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 11},
    {.bitmap_index = 435, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 439, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 444, .adv_w = 32, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 445, .adv_w = 64, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 446, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 448, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 451, .adv_w = 80, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 453, .adv_w = 144, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 460, .adv_w = 144, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 467, .adv_w = 144, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 474, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 479, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 485, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 491, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 497, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 503, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 509, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 515, .adv_w = 160, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 523, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 529, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 535, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 541, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 547, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 553, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 557, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 561, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 565, .adv_w = 64, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 569, .adv_w = 112, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 575, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 581, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 587, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 593, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 599, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 605, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 611, .adv_w = 80, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 613, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 618, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 624, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 630, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 636, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 642, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 648, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 653, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 658, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 663, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 668, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 673, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 678, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 683, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 689, .adv_w = 160, .box_w = 9, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 695, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 700, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 705, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 710, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 715, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 720, .adv_w = 32, .box_w = 2, .box_h = 8, .ofs_x = -1, .ofs_y = 5},
    {.bitmap_index = 722, .adv_w = 48, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 724, .adv_w = 48, .box_w = 3, .box_h = 8, .ofs_x = -1, .ofs_y = 5},
    {.bitmap_index = 727, .adv_w = 48, .box_w = 3, .box_h = 7, .ofs_x = -1, .ofs_y = 5},
    {.bitmap_index = 730, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 735, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 740, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 745, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 750, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 755, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 760, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 765, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 769, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 773, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 778, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 783, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 788, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 793, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 799, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 804, .adv_w = 96, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 4}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 161, .range_length = 12, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 174, .range_length = 82, .glyph_id_start = 108,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
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
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 3,
    .bpp = 1,
    .kern_classes = 0,
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
const lv_font_t font_8 = {
#else
lv_font_t font_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if FONT_8*/

