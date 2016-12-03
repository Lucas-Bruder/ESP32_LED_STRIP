/*  ----------------------------------------------------------------------------
    File: main.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/26/2016

    ------------------------------------------------------------------------- */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "led_strip/led_strip.h"

#include <stdio.h>

#define LED_STRIP_LENGTH 300U
static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];

#define LED_STRIP_RMT_INTR_NUM 19

int app_main(void)
{
    nvs_flash_init();

    struct led_strip_t led_strip = {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_1,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = GPIO_NUM_21,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
        .led_strip_length = LED_STRIP_LENGTH
    };

    bool led_init_ok = led_strip_init(&led_strip);
    assert(led_init_ok);

    struct led_color_t led_color = {
        .red = 255,
        .green = 0,
        .blue = 0,
    };

    int count = 0;
    for (uint32_t index = 0; index < LED_STRIP_LENGTH; index++) {
            led_strip_set_pixel_color(&led_strip, index, &led_color);
    }   

    led_strip_show(&led_strip);

    while (true) {
        vTaskDelay(30 / portTICK_PERIOD_MS);
        count++;

        if (count > 30) {
            break;
        }
    }

    printf("Done\n");

    bool delete_ok = led_strip_deinit(&led_strip);
    assert(delete_ok);

    count = 0;
    for (uint32_t index = 0; index < LED_STRIP_LENGTH; index++) {
            led_strip_set_pixel_color(&led_strip, index, &led_color);
    }   

    led_strip_show(&led_strip);

    while (true) {
        vTaskDelay(30 / portTICK_PERIOD_MS);
        count++;

        if (count > 30) {
            break;
        }
    }

    printf("Done\n");

    delete_ok = led_strip_deinit(&led_strip);
    assert(delete_ok);

    while (true) {
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    return 0;
}

