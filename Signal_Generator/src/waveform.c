#include "waveform.h"

float lut[LUT_SIZE];
WaveType_t type = WAVE_SINE;
float freq = BASE_FREQ;
float amp = BASE_AMP_VPP;
float offset = BASE_OFFSET;
float duty = BASE_DUTY;
uint32_t phase_acc = 0;
uint32_t phase_inc = 0;

void (*gen_funcs[])(void) = { 
    gen_sine, 
    gen_square, 
    gen_triangle
};

void gen_sine(void) {
    for(int i=0; i<LUT_SIZE; i++) lut[i] = sinf(2.0f * M_PI * i / LUT_SIZE);
}

void gen_square(void) {
    int h = (int)(LUT_SIZE * duty);
    if(h<=0) h=1;
    if(h>=LUT_SIZE) h=LUT_SIZE-1;
    for(int i=0;i<LUT_SIZE;i++) lut[i]=(i<h)?1.0f:-1.0f;
}

void gen_triangle(void) {
    for(int i=0;i<LUT_SIZE;i++){
        if(i<LUT_SIZE/2) lut[i]=(4.0f*i/LUT_SIZE)-1.0f;
        else lut[i]=3.0f-(4.0f*i/LUT_SIZE);
    }
}

void waveform_init(void) {
    gen_funcs[type]();
    phase_inc = (uint32_t)((freq / SAMPLE_RATE) * 4294967296.0f);
}

void waveform_set_type(WaveType_t t) {
    type = t;
    gen_funcs[type]();
}

void waveform_set_freq(float f) {
    if(f<MIN_FREQ) f=MIN_FREQ;
    if(f>MAX_FREQ) f=MAX_FREQ;
    freq=f;
    phase_inc=(uint32_t)((freq/SAMPLE_RATE)*4294967296.0f);
}

void waveform_set_amp(float v) {
    if(v>MAX_AMP_VPP) v=MAX_AMP_VPP;
    amp=v;
}

void waveform_set_offset(float o) {
    offset=o;
}

void waveform_set_duty(float d) {
    if(d>1.0f) d=1.0f;
    duty=d;
    if(type==WAVE_SQUARE) gen_square();
}

float waveform_get_sample(void) {
    phase_acc += phase_inc;
    return (lut[phase_acc >> 24] * (amp / 2.0f)) + offset;
}