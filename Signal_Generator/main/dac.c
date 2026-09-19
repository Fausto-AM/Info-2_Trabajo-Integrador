#include "dac.h"
#include "config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"

static const char *TAG = "DAC";

static dac_oneshot_handle_t dac_handle;

void dac_init(void)
{
    dac_oneshot_config_t dac_config = {
        .chan_id = DAC_CHAN_0,
    };

    ESP_ERROR_CHECK(
        dac_oneshot_new_channel(&dac_config, &dac_handle)
    );

    ESP_LOGI(TAG, "DAC initialized on channel %d", DAC_CHAN_1);
}

void dac_output_sample(float sample)
{
    float dac_voltage = sample + (MAX_AMP_VPP / 2.0f);

    float normalized = dac_voltage / MAX_AMP_VPP;

    int dac_value =
        (int)(normalized * DAC_MAX_VALUE);

    if (dac_value < 0)
        dac_value = 0;

    if (dac_value > DAC_MAX_VALUE)
        dac_value = DAC_MAX_VALUE;

    ESP_ERROR_CHECK(
        dac_oneshot_output_voltage(
            dac_handle,
            dac_value
        )
    );
}