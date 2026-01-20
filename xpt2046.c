#include "xpt2046.h"
#include "ili_SoftSPI.h"
#include "system.h"
#include <string.h>

uint16_t xpt_getit(uint8_t cmd){
    uint8_t rbuf[2];
    // uint8_t wbuf;

    memset(rbuf, 0, sizeof(rbuf));

    T_CS_SetLow();
    // fast_spi_transfer_buffer(&wbuf, &rbuf, MAX_LEN);
    fast_spi_write_byte(cmd);
    fast_spi_read_buffer(&rbuf, 2);
    T_CS_SetHigh();

    // 12bit Conversion
    //int pos = (rbuf[1]<<8)+rbuf[2];
    uint16_t pos = (((uint16_t)rbuf[0]) << 4) + (rbuf[1] >> 4);
    return pos;
}

bool xpt_touch_getxy(Touch * t) {
    // int level = gpio_get_level(dev->_irq);
    // ESP_LOGD(__FUNCTION__, "gpio_get_level=%d", level);
    if (TOUCH_IRQ_GetValue() == 1) return false; // Not touched
    t->x = xpt_getit( (XPT_START | XPT_XPOS | XPT_SER) );
    t->y = xpt_getit( (XPT_START | XPT_YPOS | XPT_SER) );
    return true;
}


bool xpt_is_touch_in_box(Touch t, TouchBox tb) {
    if(tb.xs <= t.x && \
         t.x <= tb.xe && \
       tb.ys <= t.y && \
         t.y <= tb.ye ) {

        return true;
    }
    return false;
}
