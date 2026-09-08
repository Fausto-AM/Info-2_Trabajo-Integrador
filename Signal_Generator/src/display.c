#include "display.h"

u8g2_t u8g2;
int cur_state, cur_idx;
float f, a, o, d;

const char* param_names[] = {"Freq", "Amp", "Offset", "Duty", "Exit"};
const char* func_names[]  = {"Seno", "Cuadrada", "Triangular", "Exit"};

uint8_t u8x8_byte_sw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_BYTE_SEND: {
            uint8_t *data = (uint8_t *)arg_ptr;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd, 0x40, true);
            i2c_master_write(cmd, data, arg_int, true);
            i2c_master_stop(cmd);
            i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
            i2c_cmd_link_delete(cmd);
            break;
        }
        default: break;
    }
    return 1;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_DELAY_MILLI: vTaskDelay(pdMS_TO_TICKS(arg_int)); break;
    }
    return 1;
}

void display_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    u8g2_Setup_ssd1306_i2c_128x64_noname(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

void draw_standby(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(&u8g2, 0, 20, "STANDBY");
    u8g2_SendBuffer(&u8g2);
}

void draw_menu(const char** items, int count, int selected) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(&u8g2, 0, 10, "MENU");
    u8g2_DrawLine(&u8g2, 0, 12, 128, 12);
    for (int i = 0; i < count; i++) {
        int y = 30 + i * 12;
        u8g2_DrawStr(&u8g2, 10, y, (i == selected) ? ">" : " ");
        u8g2_DrawStr(&u8g2, 20, y, items[i]);
    }
    u8g2_SendBuffer(&u8g2);
}

void draw_running(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(&u8g2, 0, 10, "GENERANDO");
    u8g2_DrawStr(&u8g2, 0, 30, func_names[cur_idx]);
    u8g2_SendBuffer(&u8g2);
}

void display_update(int state, int index, float freq, float amp, float offset, float duty) {
    cur_state = state; cur_idx = index; f = freq; a = amp; o = offset; d = duty;
    switch(state) {
        case STATE_STANDBY: draw_standby(); break;
        case STATE_SELECT_PARAM: draw_menu(param_names, 5, cur_idx); break;
        case STATE_EDIT_PARAM: draw_menu(param_names, 5, cur_idx); break;
        case STATE_SELECT_FUNCTION: draw_menu(func_names, 4, cur_idx); break;
        case STATE_RUN: draw_running(); break;
    }
}