/*  ----------------------------------------------------------------------------
    File: led_strip.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/23/2016

    Description: LED Library for driving various led strips

    ------------------------------------------------------------------------- */

#include "led_strip/led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>

#define BIT_1_NOPS_HIGH 15
#define BIT_1_NOPS_LOW  13
#define BIT_0_NOPS_HIGH 10
#define BIT_0_NOPS_LOW  13

#define LED_STRIP_TASK_SIZE 1024
#define LED_STRIP_TASK_PRIORITY 2

static inline void led_strip_0_bit(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    for(uint32_t i = 0; i < BIT_0_NOPS_HIGH; i++) {
         __asm__ volatile ("nop");
    }

    gpio_set_level(gpio_num, 0);
    for(uint32_t i = 0; i < BIT_0_NOPS_LOW; i++) {
         __asm__ volatile ("nop");
    }
}

static inline void led_strip_1_bit(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    for(uint32_t i = 0; i < BIT_1_NOPS_HIGH; i++) {
         __asm__ volatile ("nop");
    }

    gpio_set_level(gpio_num, 0);
    for(uint32_t i = 0; i < BIT_1_NOPS_LOW; i++) {
         __asm__ volatile ("nop");
    }
}

static void led_strip_task(void *arg)
{

}

/**
 * Before calling this, make sure that the GPIO that you will be using has already been configured
 */
bool led_strip_init(struct led_strip_t *led_strip)
{
	bool init_ok = true;
	TaskHandle_t led_strip_task_handle;

	if ((led_strip == NULL) || (led_strip->pin_num > GPIO_NUM_33) ||
		(led_strip->rgb_led_type == RGB_LED_TYPE_MAX) || (led_strip->led_strip_buf_1 == NULL) ||
		(led_strip->led_strip_buf_2 == NULL) || (led_strip->led_strip_length == 0)) {
		return false;
	}

	// Ensure the two led buffers aren't the same
	if(led_strip->led_strip_buf_1 == led_strip->led_strip_buf_2) {
		return false;
	}

    gpio_pad_select_gpio(led_strip->pin_num);
    gpio_set_direction(led_strip->pin_num, GPIO_MODE_OUTPUT);

    memset(led_strip->led_strip_buf_1, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
	memset(led_strip->led_strip_buf_2, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);

	BaseType_t task_created = xTaskCreate(led_strip_task,
							  				"led_strip_task",
							  				LED_STRIP_TASK_SIZE,
							  				led_strip,
							  				LED_STRIP_TASK_PRIORITY,
							  				&led_strip_task_handle
						  				 );

	if (!task_created) {
		return false;
	}


    return init_ok;
}

/**
 * Sets the pixel at pixel_num to color
 */
bool led_strip_set_rgb_led(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color)
{
	bool set_led_success = true;

	return set_led_success;
}

/**
 * Updates the led buffer to be shown
 */
bool led_strip_show(struct led_strip_t *led_strip)
{
	bool success = true;

	return success;
}

/**
 * Clears the LED strip
 */
bool led_strip_clear(struct led_strip_t *led_strip)
{
	bool success = true;

	return success;
}