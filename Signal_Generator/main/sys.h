#ifndef SYSTEM_H
#define SYSTEM_H

#include "encoder.h"
#include "display.h"
#include "generator.h"
#include "dac.h"
#include "sample_timer.h"
#include "waveform.h"

void init_all(void);
void system_process(void);

#endif