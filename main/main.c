/*  ----------------------------------------------------------------------------
    File: main.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/23/2016

    ------------------------------------------------------------------------- */

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "led_strip/led_strip.h"

#define LED_STRIP_LENGTH 17

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

int app_main(void)
{
    nvs_flash_init();

    struct led_strip_t led_strip = {
        .pin_num = GPIO_NUM_21,
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
        .led_strip_length = LED_STRIP_LENGTH
    };

    bool led_init_ok = led_strip_init(&led_strip);
    assert(led_init_ok);

    while (true) {

    }

    return 0;
}

