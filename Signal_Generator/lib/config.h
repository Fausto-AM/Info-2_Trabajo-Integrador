#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// GPIO pin definition
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_DAC 25
#define PIN_ENC_A 27
#define PIN_ENC_B 14
#define PIN_ENC_BTTN 12

// OLED initialization
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Waveform generation parameters
#define LUT_SIZE 256
#define SAMPLE_RATE 10000.0f
#define BASE_FREQ 1000.0f
#define BASE_AMP_VPP 1.65f
#define BASE_OFFSET 0.0f
#define BASE_DUTY 0.5f
#define MIN_FREQ 1.0f
#define MAX_FREQ 5000.0f
#define MAX_AMP_VPP 3.3f
#define MAX_OFFSET 3.3f

#endif