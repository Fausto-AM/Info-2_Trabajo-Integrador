#ifndef ENCODER_H
#define ENCODER_H

typedef enum {
    ENC_NONE,
    ENC_CW,
    ENC_CCW,
    ENC_SHORT_PRESS,
    ENC_LONG_PRESS
} encoder_t;

void encoder_init(void);
encoder_t encoder_get_event(void);

#endif