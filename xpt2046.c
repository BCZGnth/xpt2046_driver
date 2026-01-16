#include "xpt2046.h"
#include "ili_SoftSPI.h"
#include "system.h"

int xptGetit(int cmd){
    char rbuf[MAX_LEN];
    char wbuf[MAX_LEN];

    memset(wbuf, 0, sizeof(wbuf));
    memset(rbuf, 0, sizeof(rbuf));
    wbuf[0] = cmd;


    fast_spi_transfer_buffer(&wbuf, &rbuf, MAX_LEN);

    // 12bit Conversion
    //int pos = (rbuf[1]<<8)+rbuf[2];
    int pos = (((int)rbuf[1])<<4)+(rbuf[2]>>4);
    return pos;
}

bool touch_getxy(Touch * t) {
    // int level = gpio_get_level(dev->_irq);
    // ESP_LOGD(__FUNCTION__, "gpio_get_level=%d", level);
    if (TOUCH_IRQ_GetValue() == 1) return false; // Not touched
    t->x = xptGetit( (XPT_START | XPT_XPOS | XPT_SER) );
    t->y = xptGetit( (XPT_START | XPT_YPOS | XPT_SER) );
    return true;
}

bool is_touch_in_box(Touch t, TouchBox tb) {
    if(tb.xs <= t.x && \
         t.x <= tb.xe && \
       tb.ys <= t.y && \
         t.y <= tb.ye ) {

        return true;
    }
    return false;
}