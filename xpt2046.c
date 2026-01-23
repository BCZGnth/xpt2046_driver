#include "xpt2046.h"
#include "ili_SoftSPI.h"
#include "system.h"
#include <string.h>

uint16_t xpt_getit(uint8_t cmd){
    uint8_t rbuf[2];
    // uint8_t wbuf;

    memset(rbuf, 0, sizeof(rbuf));

    T_CS_SetLow();
    __delay_us(2); // Give the chip time to "See the CS line"
    // fast_spi_transfer_buffer(&wbuf, &rbuf, MAX_LEN);
    fast_spi_write_byte(cmd);
    // fast_spi_write_byte(0); // Send 8 dummy bits
 
    fast_spi_read_buffer(&rbuf, 2);
    __delay_us(2);

    T_CS_SetHigh();

    // 12bit Conversion
    // int pos = (rbuf[1]<<8)+rbuf[2];
    uint16_t pos = (((uint16_t)rbuf[0]) << 4) | (rbuf[1] >> 4);
    // uint8_t pos = rbuf;
    // return rbuf;
    return pos;
}

/* Screen Calibration data */
// Pre-calculated constants (use integers!)
#define _X_INITIAL 136
#define _X_FINAL 1919
#define _Y_INITIAL 112
#define _Y_FINAL 1959
#define _X_RES (_X_FINAL - _X_INITIAL)  // 1783
#define _Y_RES (_Y_FINAL - _Y_INITIAL)  // 1847
#define _SCREEN_X_LENGTH 320
#define _SCREEN_Y_LENGTH 480

#define _REVERSE_X 0
#define _REVERSE_Y 1

// Fixed-point scaling factors (multiply by 256 for precision)
// X: (320 << 8) / 1783 = 46
// Y: (480 << 8) / 1847 = 66
#define _X_SCALE_FACTOR 46
#define _Y_SCALE_FACTOR 66

Touch scale_touch_to_screen(uint16_t raw_x, uint16_t raw_y) {
    Touch result;
    int16_t x_adjusted, y_adjusted;
    
    // Subtract offset
    x_adjusted = (int16_t)raw_x - _X_INITIAL;
    y_adjusted = (int16_t)raw_y - _Y_INITIAL;
    
    // Clamp to valid range before scaling
    if(x_adjusted < 0) x_adjusted = 0;
    if(x_adjusted > _X_RES) x_adjusted = _X_RES;
    if(y_adjusted < 0) y_adjusted = 0;
    if(y_adjusted > _Y_RES) y_adjusted = _Y_RES;
    
    // Fixed-point multiply and divide by 256 (right shift 8)
    result.x = ((uint32_t)x_adjusted * _X_SCALE_FACTOR) >> 8;
    result.y = ((uint32_t)y_adjusted * _Y_SCALE_FACTOR) >> 8;
    
    // Final bounds check (should rarely trigger)
    if(result.x > _SCREEN_X_LENGTH) result.x = _SCREEN_X_LENGTH;
    if(result.y > _SCREEN_Y_LENGTH) result.y = _SCREEN_Y_LENGTH;
    
    // Handle reversal
    if(_REVERSE_X) result.x = _SCREEN_X_LENGTH - result.x;
    if(_REVERSE_Y) result.y = _SCREEN_Y_LENGTH - result.y;
    
    return result;
}

/** 
 * Returns TRUE on a valid read
 */
bool xpt_touch_getxy(Touch* t) {
    // int level = gpio_get_level(dev->_irq);
    // ESP_LOGD(__FUNCTION__, "gpio_get_level=%d", level);
    if (TOUCH_IRQ_GetValue() == 1) return false; // Not touched
    uint16_t x_raw = xpt_getit( (XPT_START | XPT_XPOS | XPT_SER) );
    uint16_t y_raw = xpt_getit( (XPT_START | XPT_YPOS | XPT_SER) );

    *(t) = scale_touch_to_screen(x_raw, y_raw);
    // t->x = x_raw;
    // t->y = y_raw;

    return true;
}


bool xpt_is_touch_in_box(Touch t, TouchBox tb) {
    if(tb.xs <=  t.x  && \
        t.x  <= tb.xe && \
       tb.ys <=  t.y  && \
        t.y  <= tb.ye ) {

        return true;
    }
    return false;
}

/**
 * @author: Ben Good
 * 
 * @param t: the coordinates of the touch 
 * @param p_tb: The pointer to the list of touchboxes
 * @param tb_len: the length of the touchbox list
 * 
 * @return Key of the touchbox unless no key is found.
 * @return Returns 255 if no key is found.
 */
uint8_t xpt_is_touch_in_box_list(Touch t, TouchBox* p_tb, size_t tb_len) {

    TouchBox test_tb;
    
    for(uint8_t i = 0; i < tb_len; i++) {

        test_tb = *(p_tb + i);

        if( xpt_is_touch_in_box(t, test_tb) ) {
            return (p_tb + i)->key;
        }
    }

    /* Error Case Returns 255*/
    return 255;
}