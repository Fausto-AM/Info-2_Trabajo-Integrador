#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// GPIO definitions
#define PIN_SDA       21
#define PIN_SCL       22
#define PIN_DAC       25
#define PIN_ENC_A     27
#define PIN_ENC_B     14
#define PIN_ENC_BTTN  12


// OLED Display parameters
#define OLED_ADDR     0x3C
#define OLED_WIDTH    128
#define OLED_HEIGHT   64


// Waveform generation parameters
#define LUT_SIZE      256
#define SAMPLE_RATE   10000.0f
#define BASE_FREQ     1000.0f
#define BASE_AMP_VPP  1.65f
#define BASE_OFFSET   0.0f
#define BASE_DUTY     0.5f
#define MIN_FREQ      1.0f
#define MAX_FREQ      5000.0f
#define MIN_AMP_VPP   0.1f
#define MAX_AMP_VPP   3.3f
#define MIN_OFFSET   -3.0f
#define MAX_OFFSET    3.3f
#define MIN_DUTY      0.0f
#define MAX_DUTY      1.0f

// General Parameters
#define DAC_MAX_VALUE 255
#define BUTTON_LONG_PRESS_MS 500
#define BUTTON_DEBOUNCE_MS   25
#define ENCODER_DEBOUNCE_US  1500

#endif