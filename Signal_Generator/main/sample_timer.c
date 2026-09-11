#include "sample_timer.h"
#include "config.h"
#include "generator.h"
#include "dac.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "SAMPLE_TIMER";

static esp_timer_handle_t sample_timer;

static void sample_timer_callback(void *arg)
{
    (void)arg;

    if (generator_is_running()) {

        float sample = generator_get_sample();

        dac_output_sample(sample);
    }
}


void sample_timer_init(void)
{
    const esp_timer_create_args_t timer_config = {

        .callback = sample_timer_callback,

        .arg = NULL,

        .dispatch_method = ESP_TIMER_TASK,

        .name = "sample_timer"
    };


    ESP_ERROR_CHECK(
        esp_timer_create(
            &timer_config,
            &sample_timer
        )
    );

    ESP_LOGI(TAG, "Sample timer created (%.0f Hz)", SAMPLE_RATE);
}


void sample_timer_start(void)
{
    ESP_ERROR_CHECK(
        esp_timer_start_periodic(
            sample_timer,
            (uint64_t)(
                1000000.0f / SAMPLE_RATE
            )
        )
    );

    ESP_LOGI(TAG, "Sample timer started");
}


void sample_timer_stop(void)
{
    ESP_ERROR_CHECK(
        esp_timer_stop(
            sample_timer
        )
    );

    ESP_LOGI(TAG, "Sample timer stopped");
}