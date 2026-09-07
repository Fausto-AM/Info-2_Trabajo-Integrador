#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "config.h"
#include <math.h>

typedef enum {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_TRIANGLE
} wave_t;

void gen_sine(void);
void gen_square(void);
void gen_triange(void);

void waveform_init(void);
void waveform_set_type(wave_t t);
void waveform_set_freq(float f);
void waveform_set_amp(float v);
void waveform_set_offset(float o);
void waveform_set_duty(float d);
void waveform_output_dac(void);
float waveform_get_sample(void);

#endif