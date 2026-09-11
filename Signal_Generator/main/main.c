#include "sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_task_wdt.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_task_wdt_deinit();

    init_all();

    ESP_LOGI(TAG, "Entering main loop");

    while (1) {

        encoder_t event = encoder_get_event();

        generator_process_event(event);

        if (event != ENC_NONE) {

            ESP_LOGD(TAG, "Loop: event=%d -> state=%d",
                     (int)event, (int)generator_get_state());

            display_update(
                generator_get_state(),
                generator_get_index(),
                freq,
                amp,
                offset,
                duty
            );
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}