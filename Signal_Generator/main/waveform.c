#include "waveform.h"
#include <math.h>
#include "esp_log.h"

static const char *TAG = "WAVEFORM";

static float lut[LUT_SIZE];

wave_t type = WAVE_SINE;

float freq = BASE_FREQ;
float amp = BASE_AMP_VPP;
float offset = BASE_OFFSET;
float duty = BASE_DUTY;

static uint32_t phase_acc = 0;
static uint32_t phase_inc = 0;

static const char *wave_names[] = {
    "Seno",
    "Cuadrada",
    "Triangular",
    "Sierra"
};

void gen_sine(void)
{
    for (int i = 0; i < LUT_SIZE; i++) {

        lut[i] = sinf(TWO_PI * (float)i / (float)LUT_SIZE);
    }
}


void gen_square(void)
{
    int high_samples = (int)((float)LUT_SIZE * duty);

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

            lut[i] = (4.0f * (float)i / (float)LUT_SIZE) - 1.0f;

        } else {

            lut[i] = 3.0f - (4.0f * (float)i / (float)LUT_SIZE);
        }
    }
}

void gen_sawtooth(void)
{
    for (int i = 0; i < LUT_SIZE; i++) {

        lut[i] = (2.0f * (float)i / (float)LUT_SIZE) - 1.0f;
    }
}

static void (*gen_funcs[])(void) = {
    gen_sine,
    gen_square,
    gen_triangle,
    gen_sawtooth
};

void waveform_init(void)
{
    gen_funcs[type]();

    phase_acc = 0;

    phase_inc = (uint32_t)((freq / SAMPLE_RATE) * PHASE_SCALE);

    ESP_LOGI(TAG, "Waveform initialized: type=%d freq=%.1f amp=%.2f offset=%.2f duty=%.2f", type, freq, amp, offset, duty);
}

void waveform_set_type(wave_t t)
{
    if (t > WAVE_SAWTOOTH) {
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

    phase_inc = (uint32_t)((freq / SAMPLE_RATE) * PHASE_SCALE);
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

    uint8_t index = (uint8_t)(phase_acc >> PHASE_SHIFT);

    return (lut[index] * (amp / 2.0f)) + offset;
}

wave_t waveform_get_type(void)
{
    return type;
}

const char *waveform_get_name(wave_t t)
{
    if (t > WAVE_SAWTOOTH) {
        return "?";
    }

    return wave_names[t];
}