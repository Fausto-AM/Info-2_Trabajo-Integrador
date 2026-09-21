#include "sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_log.h"

void app_main(void)
{
    esp_task_wdt_deinit();

    init_all();

    while (1) {

        system_process();

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}