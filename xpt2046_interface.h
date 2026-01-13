#ifndef _XPT2046_INTERFACE
#define _XPT2046_INTERFACE

#include "system.h"

void xpt_transaction(uint8_t * tx, uint8_t * rx, size_t len);

#endif