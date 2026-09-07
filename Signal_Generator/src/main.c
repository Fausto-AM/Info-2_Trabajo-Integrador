#include "config.h"
#include "encoder.h"
#include "generator.h"
#include "display.h"
#include "dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

void app_main(void) {
    esp_task_wdt_deinit();

    encoder_init();
    display_init();
    generator_init();
    dac_init();

    while (1) {
        generator_process_event(encoder_get_event());

        if (generator_get_state() == STATE_RUN) {
            dac_output_sample(generator_get_sample());
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}