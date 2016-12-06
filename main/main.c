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

