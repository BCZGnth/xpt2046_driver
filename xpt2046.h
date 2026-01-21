#ifndef _XPS2046
#define _XPS2046

#include "system.h"

#define MAX_LEN     3
#define	XPT_START   0x80
#define XPT_XPOS    0x50
#define XPT_YPOS    0x10
#define XPT_8BIT    0x08
#define XPT_SER	    0x04
#define XPT_DEF	    0x03

typedef struct {
    uint16_t x;
    uint16_t y;
} Touch;


typedef struct {
    uint16_t xs;
    uint16_t xe;
    uint16_t ys;
    uint16_t ye;
} TouchBox;

uint16_t xpt_getit(uint8_t cmd);
bool xpt_touch_getxy(Touch * t);
bool xpt_is_touch_in_box(Touch t, TouchBox tb);
uint8_t xpt_is_touch_in_box_list(Touch t, TouchBox* p_tb, size_t tb_len);

#endif