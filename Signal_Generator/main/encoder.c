#include "encoder.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "ENCODER";

static volatile int enc_count = 0;

static volatile uint8_t last_ab_state = 0;

static int last_count = 0;

static uint64_t btn_press_start = 0;

static int btn_pressed = 0;
static int raw_pressed_prev = 0;
static uint64_t raw_change_ms = 0;
static const int8_t qdec_table[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

void IRAM_ATTR encoder_isr(void *arg)
{
    (void)arg;

    uint8_t a = (uint8_t)gpio_get_level(PIN_ENC_A);
    uint8_t b = (uint8_t)gpio_get_level(PIN_ENC_B);

    uint8_t new_state = (a << 1) | b;

    uint8_t index = (last_ab_state << 2) | new_state;

    enc_count += qdec_table[index];

    last_ab_state = new_state;
}

void encoder_init(void)
{
    gpio_config_t encoder_config = {

        .pin_bit_mask = (1ULL << PIN_ENC_A) | (1ULL << PIN_ENC_B),

        .mode = GPIO_MODE_INPUT,

        .pull_up_en = GPIO_PULLUP_DISABLE,

        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&encoder_config));

    gpio_config_t button_config = {

        .pin_bit_mask =
            (1ULL << PIN_ENC_BTTN),

        .mode = GPIO_MODE_INPUT,

        .pull_up_en = GPIO_PULLUP_ENABLE,

        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&button_config));

    ESP_ERROR_CHECK(gpio_set_intr_type(PIN_ENC_A, GPIO_INTR_ANYEDGE));

    ESP_ERROR_CHECK(gpio_set_intr_type(PIN_ENC_B, GPIO_INTR_ANYEDGE));

    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_ENC_A, encoder_isr, NULL));

    ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_ENC_B, encoder_isr, NULL));

    last_ab_state = ((uint8_t)gpio_get_level(PIN_ENC_A) << 1) | (uint8_t)gpio_get_level(PIN_ENC_B);

    ESP_LOGI(TAG, "Encoder initialized (A=%d B=%d BTN=%d)", PIN_ENC_A, PIN_ENC_B, PIN_ENC_BTTN);
}

encoder_t encoder_get_event(void)
{
    encoder_t event = ENC_NONE;

    uint64_t now_ms = esp_timer_get_time() / 1000;

    int raw_pressed =
        (gpio_get_level(PIN_ENC_BTTN) == 0);

    if (raw_pressed != raw_pressed_prev) {

        raw_pressed_prev = raw_pressed;
        raw_change_ms = now_ms;
    }

    if ((raw_pressed != btn_pressed) &&
        ((now_ms - raw_change_ms) >= BUTTON_DEBOUNCE_MS)) {

        btn_pressed = raw_pressed;

        if (btn_pressed) {

            btn_press_start = now_ms;

        } else {

            uint64_t duration =
                now_ms - btn_press_start;


            if (duration >= BUTTON_LONG_PRESS_MS) {

                event = ENC_LONG_PRESS;

            } else {

                event = ENC_SHORT_PRESS;
            }

            ESP_LOGI(TAG, "Button event: %s (held %llu ms)", (event == ENC_LONG_PRESS) ? "LONG_PRESS" : "SHORT_PRESS", (unsigned long long)duration);
        }

        return event;
    }

    if (enc_count - last_count >= ENCODER_STEPS_PER_DETENT) {

        event = ENC_CW;

        last_count += ENCODER_STEPS_PER_DETENT;

        ESP_LOGD(TAG, "Rotation event: CW (count=%d)", enc_count);

    } else if (enc_count - last_count <= -ENCODER_STEPS_PER_DETENT) {

        event = ENC_CCW;

        last_count -= ENCODER_STEPS_PER_DETENT;

        ESP_LOGD(TAG, "Rotation event: CCW (count=%d)", enc_count);
    }

    return event;
}