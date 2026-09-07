#include "dac.h"
#include "config.h"
#include "driver/dac.h"

void dac_init(void) {
    dac_output_enable(DAC_CHANNEL_1);
}

void dac_output_sample(float sample) {
    int dac_val = (int)(((sample / MAX_AMP_VPP) + 1.0f) * 127.5f);
    if (dac_val < 0) dac_val = 0;
    if (dac_val > 255) dac_val = 255;

    dac_output_voltage(DAC_CHANNEL_1, dac_val);
}