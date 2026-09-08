#include "encoder.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"

volatile int enc_count = 0;
int last_count = 0;
uint64_t btn_press_start = 0;
int btn_pressed = 0;

void IRM_ATTR encoder_isr(void* arg) {
    if (gpio_get_level(PIN_ENC_B) == 1) enc_count++;
    else enc_count--;
}

void encoder_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_ENC_A) | (1ULL << PIN_ENC_B),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en =  GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_intr_type(PIN_ENC_A,GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_ENC_A, encoder_isr, NULL);
}

encoder_t encoder_get_event(void){
    encoder_t ev = ENC_NONE;
    if (enc_count != last_count){
        ev = (enc_count > last_count) ? ENC_CW : ENC_CCW;
        last_count = enc_count;
    }
    uint64_t now_ms = esp_timer_get_time() / 1000;
    int is_pressed = (gpio_get_level(PIN_ENC_BTTN) == 0);

    if (is_pressed != btn_pressed) {
        btn_pressed = is_pressed;

        if (is_pressed) {
            btn_press_start = now_ms;
        } else {
            uint64_t duration = now_ms - btn_press_start;
            if (duration > 500) ev = ENC_LONG_PRESS;
            else ev = ENC_SHORT_PRESS;
        }
    }
}