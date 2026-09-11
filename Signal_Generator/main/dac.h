#ifndef DAC_H
#define DAC_H

#include "driver/dac_oneshot.h"

void dac_init(void);
void dac_output_sample(float sample);

#endif