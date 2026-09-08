#include "sys.h"
#include "encoder.h"
#include "display.h"
#include "waveform.h"
#include "generator.h"
#include "dac.h"

void init_all(void) {
    encoder_init();
    display_init();
    generator_init();
    dac_init();
}