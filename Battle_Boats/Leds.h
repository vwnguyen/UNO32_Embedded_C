
#ifndef LED_H
#define	LED_H

#include "BOARD.h"
#include <xc.h>

#define LEDS_INIT() do{ \
    TRISE = 0x00; \
    LATE = 0x00; \
}while(0)

#define LEDS_GET() (LATE & 0x0F) 

#define LEDS_SET(x) do{ \
    LATE = x; \
}while(0)


#endif