#include "generator.h"
#include "display.h"
#include "config.h"

state_t state = STATE_STANDBY;
state_t prev_state = STATE_STANDBY;
int param_idx = 0;
int func_idx = 0;
float freq = BASE_FREQ;
float amp = BASE_AMP_VPP;
float offset = BASE_OFFSET;
float duty = BASE_DUTY;

param_t params[] = {
    {"Freq", &freq, MIN_FREQ, MAX_FREQ, 10.0f, waveform_set_freq},
    {"Amp", &amp, 0.1f, MAX_AMP_VPP, 0.5f, waveform_set_amp},
    {"Offset", &offset, -3.0f, MAX_OFFSET, 0.1f, waveform_set_offset},
    {"Duty", &duty, 0.0f, 1.0f, 0.05f, waveform_set_duty}
};

void act_none(void) {}
void act_back_prev(void) { state = prev_state; }
void act_enter_params(void) { prev_state = state; state = STATE_SELECT_PARAM; param_idx = 0; }
void act_enter_func(void) { prev_state = state; state = STATE_SELECT_FUNCTION; func_idx = 0; }
void act_start(void) { prev_state = STATE_RUN; state = STATE_RUN; }
void act_stop(void) { state = STATE_STANDBY; }
void act_inc_param(void) { if(param_idx < PARAM_COUNT){ params[param_idx].setter(*params[param_idx].value + params[param_idx].step); } }
void act_dec_param(void) { if(param_idx < PARAM_COUNT){ params[param_idx].setter(*params[param_idx].value - params[param_idx].step); } }
void act_inc_func(void) { func_idx = (func_idx + 1) % 4; } // Corregido a act_inc_func
void act_dec_func(void) { func_idx = (func_idx - 1 + 4) % 4; }
void act_select_param(void) { if (param_idx >= PARAM_COUNT) act_back_prev(); else state = STATE_EDIT_PARAM; }
void act_select_func(void) { if (func_idx >= 3) act_back_prev(); else waveform_set_type(func_idx); }

transition_t fsm[] = {
    {STATE_STANDBY, ENC_SHORT_PRESS, STATE_SELECT_PARAM, act_enter_params},
    {STATE_STANDBY, ENC_LONG_PRESS, STATE_SELECT_FUNCTION, act_enter_func},
    {STATE_STANDBY, ENC_CW, STATE_RUN, act_start},

    {STATE_SELECT_PARAM, ENC_CW, STATE_SELECT_PARAM, act_inc_param},
    {STATE_SELECT_PARAM, ENC_CCW, STATE_SELECT_PARAM, act_dec_param},
    {STATE_SELECT_PARAM, ENC_SHORT_PRESS, STATE_EDIT_PARAM, act_select_param},
    {STATE_SELECT_PARAM, ENC_LONG_PRESS, STATE_STANDBY, act_back_prev},

    {STATE_EDIT_PARAM, ENC_CW, STATE_EDIT_PARAM, act_inc_param},
    {STATE_EDIT_PARAM, ENC_CCW, STATE_EDIT_PARAM, act_dec_param},
    {STATE_EDIT_PARAM, ENC_SHORT_PRESS, STATE_SELECT_PARAM, act_back_prev},
    {STATE_EDIT_PARAM, ENC_LONG_PRESS, STATE_STANDBY, act_back_prev},

    {STATE_SELECT_FUNCTION, ENC_CW, STATE_SELECT_FUNCTION, act_inc_func},
    {STATE_SELECT_FUNCTION, ENC_CCW, STATE_SELECT_FUNCTION, act_dec_func},
    {STATE_SELECT_FUNCTION, ENC_SHORT_PRESS, STATE_SELECT_FUNCTION, act_select_func},
    {STATE_SELECT_FUNCTION, ENC_LONG_PRESS, STATE_STANDBY, act_back_prev},

    {STATE_RUN, ENC_SHORT_PRESS, STATE_SELECT_PARAM, act_enter_params},
    {STATE_RUN, ENC_LONG_PRESS, STATE_STANDBY, act_stop},
};

void generator_init(void) {
    waveform_init();
    display_update(state, param_idx, freq, amp, offset, duty);
}

void generator_process_event(encoder_t e) {
    if (e == ENC_NONE) return;
    for (int i = 0; i < sizeof(fsm) / sizeof(fsm[0]); i++) {
        if (fsm[i].from == state && fsm[i].on == e) {
            if (fsm[i].action) fsm[i].action();
            state = fsm[i].to;
            display_update(state, (state == STATE_SELECT_FUNCTION || state == STATE_RUN) ? func_idx : param_idx, freq, amp, offset, duty);
            return;
        }
    }
}

state_t generator_get_state(void) { return state; }
int generator_get_index(void) { return (state == STATE_SELECT_FUNCTION || state == STATE_RUN) ? func_idx : param_idx; }
float generator_get_sample(void) { return waveform_get_sample(); }