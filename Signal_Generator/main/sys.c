#include "sys.h"
#include "esp_log.h"

static const char *TAG = "SYS";

void init_all(void)
{
    ESP_LOGI(TAG, "init_all: starting");

    encoder_init();
    display_init();
    dac_init();
    generator_init();
    sample_timer_init();
    sample_timer_start();

    display_update(generator_get_state(), generator_get_index(), freq, amp, offset, duty, (int)waveform_get_type());

    ESP_LOGI(TAG, "init_all: done");
}

void system_process(void)
{
    encoder_t event = encoder_get_event();

    if (event == ENC_NONE) {
        return;
    }

    ESP_LOGD(TAG, "system_process: event=%d", (int)event);

    generator_process_event(event);

    display_update(generator_get_state(), generator_get_index(), freq, amp, offset, duty, (int)waveform_get_type());
}