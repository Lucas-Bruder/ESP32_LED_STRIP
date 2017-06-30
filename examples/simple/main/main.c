/*  ----------------------------------------------------------------------------
    File: main.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/26/2016
    ------------------------------------------------------------------------- */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_task.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "led_strip/led_strip.h"

#include <stdio.h>

extern void main_led_task(void *args);

void app_main(void)
{
    nvs_flash_init();
    TaskHandle_t main_task_handle;
    BaseType_t task_created = xTaskCreate(main_led_task,
                                            "main_led_task",
                                            ESP_TASK_MAIN_STACK,
                                            NULL,
                                            ESP_TASK_MAIN_PRIO,
                                            &main_task_handle);
    (void)task_created;
    vTaskDelete(NULL);
}

#define LED_STRIP_LENGTH 477U
#define LED_STRIP_RMT_INTR_NUM 19U
#define DELAY_MS 400
#define WIDTH 5
#define MAX_BRIGHTNESS 2

static struct led_color_t led_strip_buf_1[LED_STRIP_LENGTH];
static struct led_color_t led_strip_buf_2[LED_STRIP_LENGTH];


void main_led_task(void *args)
{
    struct led_strip_t led_strip = {
        .rgb_led_type = RGB_LED_TYPE_WS2812,
        .rmt_channel = RMT_CHANNEL_1,
        .rmt_interrupt_num = LED_STRIP_RMT_INTR_NUM,
        .gpio = GPIO_NUM_21,
        .led_strip_buf_1 = led_strip_buf_1,
        .led_strip_buf_2 = led_strip_buf_2,
        .led_strip_length = LED_STRIP_LENGTH
    };
    led_strip.access_semaphore = xSemaphoreCreateBinary();

    bool led_init_ok = led_strip_init(&led_strip);
    assert(led_init_ok);
}
