#ifndef GENERATOR_H
#define GENERATOR_H

#include "encoder.h"
#include "waveform.h"

typedef enum {
    STATE_STANDBY,
    STATE_SELECT_PARAM,
    STATE_EDIT_PARAM,
    STATE_SELECT_FUNCTION,
    STATE_RUN
} state_t;

typedef struct {
    const char* name;
    float* value;
    float min, max, step;
    void (*setter)(float);
} param_t;

typedef struct {
    state_t from;
    encoder_t on;
    state_t to;
    void (*action)(void);
} transition_t;

#define PARAM_COUNT 4

void act_none(void);
void act_back_prev(void);
void act_enter_params(void);
void act_enter_func(void);
void act_start(void);
void act_stop(void);
void act_inc_param(void);
void act_dec_param(void);
void act_int_func(void);
void act_dec_func(void);
void act_select_param(void);
void act_select_func(void);

void generator_init(void);
void generator_process_event(encoder_t e);
state_t generator_get_state(void);
int generator_get_index(void);
float generator_get_sample(void);

#endif