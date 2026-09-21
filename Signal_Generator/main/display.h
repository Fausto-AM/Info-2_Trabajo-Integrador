#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "u8g2.h"
#include "driver/i2c.h"

void display_init(void);

void display_update(int state, int index, float freq, float amp, float offset, float duty, int wave_type);

#endif