#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "config.h"
#include <stdint.h>

#define TWO_PI          6.28318530717958647692f
#define PHASE_SCALE     4294967296.0f
#define PHASE_SHIFT     24

typedef enum {
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAWTOOTH

} wave_t;

extern float freq;
extern float amp;
extern float offset;
extern float duty;

void gen_sine(void);
void gen_square(void);
void gen_triangle(void);
void gen_sawtooth(void);

void waveform_init(void);

void waveform_set_type(wave_t type);

void waveform_set_freq(float f);
void waveform_set_amp(float a);
void waveform_set_offset(float o);
void waveform_set_duty(float d);

float waveform_get_sample(void);

wave_t waveform_get_type(void);

const char *waveform_get_name(wave_t type);

#endif