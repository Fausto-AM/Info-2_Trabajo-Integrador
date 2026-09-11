#ifndef GENERATOR_H
#define GENERATOR_H

#include "encoder.h"

typedef enum {
    STATE_INIT,
    STATE_STANDBY,
    STATE_SELECT_PARAM,
    STATE_EDIT_PARAM,
    STATE_GENERATING,
    STATE_SELECT_FUNC
} state_t;


typedef struct {
    const char *name;
    float *value;
    float min;
    float max;
    float step;
    void (*setter)(float);
} param_t;

#define PARAM_COUNT     4
#define FUNCTION_COUNT  3

void generator_init(void);
void generator_process_event(encoder_t event);
state_t generator_get_state(void);
int generator_get_index(void);
int generator_is_running(void);
float generator_get_sample(void);

#endif