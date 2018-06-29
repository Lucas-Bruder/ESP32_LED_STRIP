/*  ----------------------------------------------------------------------------
    File: main.c
    Author(s):  Tiago Medicci Serrano <tiago.medicci@gmail.com>
    Date Created: 06/02/2018
    Last modified: 06/02/2018

    ------------------------------------------------------------------------- */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_task.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "led_strip.h"
#include "sdkconfig.h"

#include <stdio.h>

#define LED_STRIP_LENGTH 6U

struct led_strip_t led_strip;
struct led_strip_effect_t led_strip_effect;
struct led_color_t led_effect_color;

extern void main_led_task(void *args);

/**
  * @brief     	Initialize WS2812 LED Interface
  *
  * @param		None
  *
  * @return
  *      -ESP_OK 	On success
  *      -ESP_FAIL 	Generic code indicating failure
  *
  **/
esp_err_t initialize_LED(void)
{

	static struct led_color_t led_strip_buf_1[CONFIG_LED_STRIP_LENGTH];
	static struct led_color_t led_strip_buf_2[CONFIG_LED_STRIP_LENGTH];

	led_strip.rgb_led_type = CONFIG_RGB_LED_TYPE;
	led_strip.rmt_channel = CONFIG_RMT_CHANNEL;
	led_strip.gpio = CONFIG_GPIO_NUM;
	led_strip.led_strip_buf_1 = led_strip_buf_1;
	led_strip.led_strip_buf_2 = led_strip_buf_2;
	led_strip.led_strip_length = LED_STRIP_LENGTH;
	led_strip.access_semaphore = xSemaphoreCreateBinary();

	led_strip_effect.led_strip = &led_strip;

	//TODO: turn led_strip_init return into a esp_err_t indicating ESP-IDF error code
	if(!led_strip_init(led_strip_effect.led_strip)){
		return ESP_FAIL;
	}
	return ESP_OK;
}

void main_led_task(void *pv)
{
	ESP_ERROR_CHECK( initialize_LED() );
	led_effect_color.red = 0;
	led_effect_color.green = 0;
	led_effect_color.blue = 0;
	while(true){
		ESP_ERROR_CHECK( led_strip_set_effect(&led_strip_effect, RGB, &led_effect_color, 255) );	//Set RGB rounding effect at max speed
		vTaskDelay(6000 / portTICK_PERIOD_MS);														//Wait for 6s
		led_strip_clear(led_strip_effect.led_strip);												//Clear LED Strip
		vTaskDelay(6000 / portTICK_PERIOD_MS);														//Wait for 6s
		//Set white color (RGB=[255,255,255])
		led_effect_color.red = 55;
		led_effect_color.green = 55;
		led_effect_color.blue = 55;
		ESP_ERROR_CHECK( led_strip_set_effect(&led_strip_effect, COLOR, &led_effect_color, 200) );	//Set white color
		vTaskDelay(6000 / portTICK_PERIOD_MS);														//Wait for 6s
	}
}

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

