#include "generator.h"
#include "config.h"
#include "waveform.h"
#include "esp_log.h"

static const char *TAG = "GENERATOR";

static const char *hub_names[HUB_COUNT] = {
    "Parametros",
    "Funcion",
    "Run/Stop"
};

static const char *state_names[] = {
    "INIT",
    "STANDBY",
    "SELECT_PARAM",
    "EDIT_PARAM",
    "GENERATING",
    "SELECT_FUNC"
};

static state_t state = STATE_INIT;
static state_t return_state = STATE_STANDBY;
static int generator_running = 0;
static int param_idx = 0;
static int func_idx = 0;
static int hub_idx = 0;

static param_t params[] = {

    {
        "Frequency",
        &freq,
        MIN_FREQ,
        MAX_FREQ,
        10.0f,
        waveform_set_freq
    },

    {
        "Amplitude",
        &amp,
        MIN_AMP_VPP,
        MAX_AMP_VPP,
        0.1f,
        waveform_set_amp
    },

    {
        "Offset",
        &offset,
        MIN_OFFSET,
        MAX_OFFSET,
        0.1f,
        waveform_set_offset
    },

    {
        "Duty",
        &duty,
        MIN_DUTY,
        MAX_DUTY,
        0.05f,
        waveform_set_duty
    }

};

static void hub_next(void)
{
    hub_idx++;

    if (hub_idx >= HUB_COUNT) {
        hub_idx = 0;
    }

    ESP_LOGI(TAG, "Hub cursor -> %s", hub_names[hub_idx]);
}

static void hub_previous(void)
{
    if (hub_idx == 0) {
        hub_idx = HUB_COUNT - 1;
    } else {
        hub_idx--;
    }

    ESP_LOGI(TAG, "Hub cursor -> %s", hub_names[hub_idx]);
}

static void param_next(void)
{
    param_idx++;

    if (param_idx >= PARAM_COUNT) {
        param_idx = 0;
    }

    ESP_LOGI(TAG, "Param cursor -> %s", params[param_idx].name);
}

static void param_previous(void)
{
    if (param_idx == 0) {
        param_idx = PARAM_COUNT - 1;
    } else {
        param_idx--;
    }

    ESP_LOGI(TAG, "Param cursor -> %s", params[param_idx].name);
}

static void param_increase(void)
{
    float value;

    value = *params[param_idx].value;

    value += params[param_idx].step;

    if (value > params[param_idx].max) {
        value = params[param_idx].max;
    }

    params[param_idx].setter(value);

    ESP_LOGI(TAG, "%s += %.3f -> %.3f", params[param_idx].name, params[param_idx].step, *params[param_idx].value);
}

static void param_decrease(void)
{
    float value;

    value = *params[param_idx].value;

    value -= params[param_idx].step;

    if (value < params[param_idx].min) {
        value = params[param_idx].min;
    }

    params[param_idx].setter(value);

    ESP_LOGI(TAG, "%s -= %.3f -> %.3f", params[param_idx].name, params[param_idx].step, *params[param_idx].value);
}

static void function_next(void)
{
    func_idx++;

    if (func_idx >= FUNCTION_COUNT) {
        func_idx = 0;
    }

    ESP_LOGI(TAG, "Function cursor -> %s", waveform_get_name((wave_t)func_idx));
}

static void function_previous(void)
{
    if (func_idx == 0) {
        func_idx = FUNCTION_COUNT - 1;
    } else {
        func_idx--;
    }

    ESP_LOGI(TAG, "Function cursor -> %s", waveform_get_name((wave_t)func_idx));
}

static void function_select(void)
{
    waveform_set_type((wave_t)func_idx);

    ESP_LOGI(TAG, "Function applied: %s", waveform_get_name((wave_t)func_idx));
}

static void hub_commit(void)
{
    switch (hub_idx) {

        case HUB_PARAM:
            return_state = state;
            state = STATE_SELECT_PARAM;
            ESP_LOGI(TAG, "Enter SELECT_PARAM (return to %s)", state_names[return_state]);
            break;

        case HUB_FUNC:
            return_state = state;
            state = STATE_SELECT_FUNC;
            ESP_LOGI(TAG, "Enter SELECT_FUNC (return to %s)", state_names[return_state]);
            break;

        case HUB_RUN:

            if (state == STATE_STANDBY) {
                generator_running = 1;
                state = STATE_GENERATING;
                ESP_LOGI(TAG, "RUN pressed -> GENERATING (output enabled)");
            } else {
                generator_running = 0;
                state = STATE_STANDBY;
                ESP_LOGI(TAG, "STOP pressed -> STANDBY (output muted)");
            }

            break;

        default:
            break;
    }
}

void generator_init(void)
{
    waveform_init();

    param_idx = 0;
    func_idx = 0;
    hub_idx = 0;
    generator_running = 0;
    state = STATE_STANDBY;
    return_state = STATE_STANDBY;

    ESP_LOGI(TAG, "Init -> STANDBY");
}

void generator_process_event(encoder_t event)
{
    if (event == ENC_NONE) {
        return;
    }

    ESP_LOGD(TAG, "State=%s event=%d", state_names[state], (int)event);

    switch (state) {

        case STATE_STANDBY:
        case STATE_GENERATING:

            if (event == ENC_CW) {
                hub_next();
            }

            else if (event == ENC_CCW) {
                hub_previous();
            }

            else if (event == ENC_SHORT_PRESS) {
                hub_commit();
            }

            break;

        case STATE_SELECT_PARAM:

            if (event == ENC_CW) {
                param_next();
            }

            else if (event == ENC_CCW) {
                param_previous();
            }

            else if (event == ENC_SHORT_PRESS) {
                state = STATE_EDIT_PARAM;
                ESP_LOGI(TAG, "Enter EDIT_PARAM (%s)", params[param_idx].name);
            }

            else if (event == ENC_LONG_PRESS) {
                ESP_LOGI(TAG, "Exit SELECT_PARAM -> %s", state_names[return_state]);
                state = return_state;
            }

            break;

        case STATE_EDIT_PARAM:

            if (event == ENC_CW) {
                param_increase();
            }

            else if (event == ENC_CCW) {
                param_decrease();
            }

            else if (event == ENC_SHORT_PRESS || event == ENC_LONG_PRESS) {
                ESP_LOGI(TAG, "Exit EDIT_PARAM -> SELECT_PARAM");
                state = STATE_SELECT_PARAM;
            }

            break;

        case STATE_SELECT_FUNC:

            if (event == ENC_CW) {
                function_next();
            }

            else if (event == ENC_CCW) {
                function_previous();
            }

            else if (event == ENC_SHORT_PRESS) {
                function_select();
            }

            else if (event == ENC_LONG_PRESS) {
                ESP_LOGI(TAG, "Exit SELECT_FUNC -> %s", state_names[return_state]);
                state = return_state;
            }

            break;

        default:
            break;
    }
}

state_t generator_get_state(void)
{
    return state;
}

int generator_get_index(void)
{
    switch (state) {

        case STATE_SELECT_PARAM:
        case STATE_EDIT_PARAM:
            return param_idx;

        case STATE_SELECT_FUNC:
            return func_idx;

        case STATE_STANDBY:
        case STATE_GENERATING:
            return hub_idx;

        default:
            return 0;
    }
}

int generator_is_running(void)
{
    return generator_running;
}

float generator_get_sample(void)
{
    return waveform_get_sample();
}