#ifndef ENCODER_H
#define ENCODER_H

#include "config.h"
#include "driver/gpio.h"
#include "esp_attr.h"


typedef enum {

    ENC_NONE,
    ENC_CW,
    ENC_CCW,
    ENC_SHORT_PRESS,
    ENC_LONG_PRESS

} encoder_t;


void encoder_init(void);

encoder_t encoder_get_event(void);

void encoder_isr(void *arg);


#endif