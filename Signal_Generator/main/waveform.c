#include "waveform.h"

#include <math.h>


static float lut[LUT_SIZE];


wave_t type = WAVE_SINE;


float freq = BASE_FREQ;
float amp = BASE_AMP_VPP;
float offset = BASE_OFFSET;
float duty = BASE_DUTY;


static uint32_t phase_acc = 0;
static uint32_t phase_inc = 0;


void gen_sine(void)
{
    for (int i = 0; i < LUT_SIZE; i++) {

        lut[i] =
            sinf(
                TWO_PI *
                (float)i /
                (float)LUT_SIZE
            );
    }
}


void gen_square(void)
{
    int high_samples =
        (int)(
            (float)LUT_SIZE *
            duty
        );


    if (high_samples <= 0) {
        high_samples = 1;
    }


    if (high_samples >= LUT_SIZE) {
        high_samples = LUT_SIZE - 1;
    }


    for (int i = 0; i < LUT_SIZE; i++) {

        if (i < high_samples) {

            lut[i] = 1.0f;

        } else {

            lut[i] = -1.0f;
        }
    }
}


void gen_triangle(void)
{
    for (int i = 0; i < LUT_SIZE; i++) {

        if (i < LUT_SIZE / 2) {

            lut[i] =
                (4.0f *
                 (float)i /
                 (float)LUT_SIZE)
                - 1.0f;

        } else {

            lut[i] =
                3.0f -
                (4.0f *
                 (float)i /
                 (float)LUT_SIZE);
        }
    }
}


static void (*gen_funcs[])(void) = {

    gen_sine,
    gen_square,
    gen_triangle

};


void waveform_init(void)
{
    gen_funcs[type]();

    phase_acc = 0;


    phase_inc =
        (uint32_t)(
            (freq / SAMPLE_RATE) *
            PHASE_SCALE
        );
}


void waveform_set_type(wave_t t)
{
    if (t > WAVE_TRIANGLE) {
        return;
    }


    type = t;


    gen_funcs[type]();
}


void waveform_set_freq(float f)
{
    if (f < MIN_FREQ) {
        f = MIN_FREQ;
    }


    if (f > MAX_FREQ) {
        f = MAX_FREQ;
    }


    freq = f;


    phase_inc =
        (uint32_t)(
            (freq / SAMPLE_RATE) *
            PHASE_SCALE
        );
}


void waveform_set_amp(float a)
{
    if (a < MIN_AMP_VPP) {
        a = MIN_AMP_VPP;
    }


    if (a > MAX_AMP_VPP) {
        a = MAX_AMP_VPP;
    }


    amp = a;
}


void waveform_set_offset(float o)
{
    if (o < MIN_OFFSET) {
        o = MIN_OFFSET;
    }


    if (o > MAX_OFFSET) {
        o = MAX_OFFSET;
    }


    offset = o;
}


void waveform_set_duty(float d)
{
    if (d < MIN_DUTY) {
        d = MIN_DUTY;
    }


    if (d > MAX_DUTY) {
        d = MAX_DUTY;
    }


    duty = d;


    if (type == WAVE_SQUARE) {
        gen_square();
    }
}


float waveform_get_sample(void)
{
    phase_acc += phase_inc;


    uint8_t index =
        (uint8_t)(
            phase_acc >>
            PHASE_SHIFT
        );


    return
        (lut[index] * (amp / 2.0f))
        + offset;
}