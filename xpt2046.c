#include "xpt2046.h"
#include "system.h"

int xptGetit(TFT_t * dev, int cmd){
    char rbuf[MAX_LEN];
    char wbuf[MAX_LEN];

    memset(wbuf, 0, sizeof(wbuf));
    memset(rbuf, 0, sizeof(rbuf));
    wbuf[0] = cmd;



    // 12bit Conversion
    //int pos = (rbuf[1]<<8)+rbuf[2];
    int pos = (((int)rbuf[1])<<4)+(rbuf[2]>>4);
    return(pos);
}

bool touch_getxy(TFT_t *dev, int *xp, int *yp) {
    int level = gpio_get_level(dev->_irq);
    ESP_LOGD(__FUNCTION__, "gpio_get_level=%d", level);
    if (level == 1) return false; // Not touched
    *xp = xptGetit(dev, (XPT_START | XPT_XPOS | XPT_SER) );
    *yp = xptGetit(dev, (XPT_START | XPT_YPOS | XPT_SER) );
    return true;
}