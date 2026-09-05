#ifndef CONFIG_H
#define CONFIG_H

// GPIO pin definition
#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_DAC 25
#define PIN_ENC_A 27
#define PIN_ENC_B 14
#define PIN_ENC_BTTN 12

// Waveform generation parameters
#define LUT_SIZE 256
#define SAMPLE_RATE 10000.0f
#define BASE_FREQ 1000.0f
#define BASE_AMP_VPP 2.5f
#define BASE_OFFSET 0.0f
#define BASE_DUTY 0.5f

#endif