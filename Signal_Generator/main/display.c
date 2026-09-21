#include "display.h"
#include "generator.h"
#include "waveform.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "DISPLAY";

static u8g2_t u8g2;

static const char *param_names[] = {
    "Frecuencia",
    "Amplitud",
    "Offset",
    "Duty"
};

static char hub_func_row[24];

static i2c_cmd_handle_t oled_txn = NULL;

static uint8_t oled_i2c_byte(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;

    switch (msg) {

        case U8X8_MSG_BYTE_START_TRANSFER:

            oled_txn = i2c_cmd_link_create();

            i2c_master_start(oled_txn);

            i2c_master_write_byte(oled_txn, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);

            break;

        case U8X8_MSG_BYTE_SEND:

            i2c_master_write(oled_txn, (uint8_t *)arg_ptr, arg_int, true);

            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
        {
            i2c_master_stop(oled_txn);

            esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, oled_txn, 1000 / portTICK_PERIOD_MS);

            i2c_cmd_link_delete(oled_txn);
            oled_txn = NULL;

            if (ret != ESP_OK) {

                ESP_LOGE(TAG, "I2C transfer FAILED: err=%s (%d)", esp_err_to_name(ret), (int)ret);
            }

            break;
        }

        default:
            break;
    }

    return 1;
}

static uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;
    (void)arg_ptr;

    switch (msg) {

        case U8X8_MSG_DELAY_MILLI:

            vTaskDelay(pdMS_TO_TICKS(arg_int));

            break;

        default:
            break;
    }

    return 1;
}

void display_init(void)
{
    i2c_config_t conf = {

        .mode = I2C_MODE_MASTER,

        .sda_io_num = PIN_SDA,

        .scl_io_num = PIN_SCL,

        .sda_pullup_en = GPIO_PULLUP_ENABLE,

        .scl_pullup_en = GPIO_PULLUP_ENABLE,

        .master.clk_speed = 400000

    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));


    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));


    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, oled_i2c_byte, u8x8_gpio_and_delay);


    u8g2_InitDisplay(&u8g2);

    u8g2_SetPowerSave(&u8g2, 0);

    ESP_LOGI(TAG, "Display initialized (SSD1306 128x64 @ I2C)");
}

static void draw_menu(const char *title, const char **items, int count, int selected, int active)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);

    u8g2_DrawStr(&u8g2, 0, 10, title);

    u8g2_DrawLine(&u8g2,0, 12, 128, 12);


    for (int i = 0; i < count; i++) {

        int y = 24 + i * 10;

        if (i == selected) {
            u8g2_DrawStr(&u8g2, 0, y, ">");
        }

        if (i == active) {
            u8g2_DrawStr(&u8g2, 6, y, "*");
        }

        u8g2_DrawStr(&u8g2, 14, y, items[i]);
    }


    u8g2_SendBuffer(&u8g2);
}

static void draw_hub_menu(int selected, int generating, int wave_type)
{
    snprintf(hub_func_row, sizeof(hub_func_row), "Funcion: %s", waveform_get_name((wave_t)wave_type));

    const char *items[3];

    items[0] = "Parametros";
    items[1] = hub_func_row;
    items[2] = generating ? "STOP" : "RUN";

    draw_menu(generating ? "Generando..." : "Standby", items, 3, selected, -1);
}

static void draw_param(int index, float freq, float amp, float offset, float duty)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);

    u8g2_DrawStr(&u8g2, 0, 12, param_names[index]);

    u8g2_DrawLine(&u8g2, 0, 14, 128, 14);


    char value[20];


    switch (index) {

        case 0:

            snprintf(value, sizeof(value), "%.0f Hz", freq);

            break;

        case 1:

            snprintf(value, sizeof(value), "%.2f Vpp", amp);

            break;

        case 2:

            snprintf(value, sizeof(value), "%.2f V", offset);

            break;

        case 3:

            snprintf(value, sizeof(value), "%.0f %%", duty * 100.0f);

            break;

        default:

            value[0] = '\0';

            break;
    }

    u8g2_SetFont(&u8g2, u8g2_font_10x20_tf);

    u8g2_DrawStr(&u8g2, 0, 42, value);

    u8g2_SendBuffer(&u8g2);
}

void display_update(int state, int index, float freq, float amp, float offset, float duty, int wave_type)
{
    ESP_LOGD(TAG, "Redraw: state=%d index=%d", state, index);

    switch (state) {

        case STATE_STANDBY:

            draw_hub_menu(index, 0, wave_type);

            break;

        case STATE_GENERATING:

            draw_hub_menu(index, 1, wave_type);

            break;

        case STATE_SELECT_PARAM:

            draw_menu("Parametros", param_names, PARAM_COUNT, index, -1);

            break;

        case STATE_EDIT_PARAM:

            draw_param(index, freq, amp, offset, duty);

            break;

        case STATE_SELECT_FUNC:
        {
            const char *func_names[FUNCTION_COUNT];

            for (int i = 0; i < FUNCTION_COUNT; i++) {
                func_names[i] = waveform_get_name((wave_t)i);
            }

            draw_menu("Funciones", func_names, FUNCTION_COUNT, index, wave_type);

            break;
        }

        default:
            break;
    }
}