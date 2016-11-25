/*  ----------------------------------------------------------------------------
    File: main.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/25/2016

    ------------------------------------------------------------------------- */

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "led_strip/led_strip.h"

#define LED_STRIP_LENGTH 17

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

int app_main(void)
{
    nvs_flash_init();

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = GPIO_SEL_21,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t err = gpio_config(&gpio_cfg);
    assert(err == ESP_OK);

    struct led_strip_t led_strip = {
        .pin_num = GPIO_NUM_21,
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
        .led_strip_length = LED_STRIP_LENGTH
    };

    bool led_init_ok = led_strip_init(&led_strip);
    assert(led_init_ok);

    struct led_color_t led_color = {
        .red = 0,
        .green = 0,
        .blue = 255,
    };

    uint16_t index = 0;
    bool forward = true;
    while (true) {

        led_strip_set_pixel_color(&led_strip, index, &led_color);
        led_strip_show(&led_strip);

        if (forward) {
            if(index == LED_STRIP_LENGTH) {
                forward = !forward;
            } else {
                index++;
            }
        }

        if (!forward) {
            if(index == UINT16_MAX) {
                forward = !forward;
            } else {
                index--;
            }
        }

        vTaskDelay(25 / portTICK_PERIOD_MS);
    }

    return 0;
}

