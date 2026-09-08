#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "u8g2.h"
#include "driver/i2c.h"
#include "generator.h"
#include <stdio.h>

void display_init(void);
void display_update(int state, int index, float freq, float amp, float offset, float duty);
void display_standby(void);
void display_draw_menu(char** items, int count, int selected);
void display_draw_running(void);

#endif