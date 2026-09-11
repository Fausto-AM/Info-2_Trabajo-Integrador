idf_component_register(
    SRCS
        "main.c"
        "encoder.c"
        "display.c"
        "waveform.c"
        "generator.c"
        "dac.c"
        "sample_timer.c"
        "sys.c"
        "sample_timer.c"

    INCLUDE_DIRS "."

    PRIV_REQUIRES
        driver
        esp_driver_dac
        esp_driver_gpio
        esp_driver_i2c
        esp_timer
        u8g2
)