/*  ----------------------------------------------------------------------------
    File: led_strip.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/25/2016

    Description: LED Library for driving various led strips on ESP32

    ------------------------------------------------------------------------- */

#include "led_strip/led_strip.h"

#include <stdio.h>
#include <string.h>

#define LED_STRIP_BIT_1_NOPS_HIGH 15
#define LED_STRIP_BIT_1_NOPS_LOW  13
#define LED_STRIP_BIT_0_NOPS_HIGH 10
#define LED_STRIP_BIT_0_NOPS_LOW  13

#define LED_STRIP_TASK_SIZE 	1024
#define LED_STRIP_TASK_PRIORITY configMAX_PRIORITIES - 2

static inline void led_strip_0_bit(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    for(uint32_t i = 0; i < LED_STRIP_BIT_0_NOPS_HIGH; i++) {
         __asm__ volatile ("nop");
    }

    gpio_set_level(gpio_num, 0);
    for(uint32_t i = 0; i < LED_STRIP_BIT_0_NOPS_LOW; i++) {
         __asm__ volatile ("nop");
    }
}

static inline void led_strip_1_bit(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    for(uint32_t i = 0; i < LED_STRIP_BIT_1_NOPS_HIGH; i++) {
         __asm__ volatile ("nop");
    }

    gpio_set_level(gpio_num, 0);
    for(uint32_t i = 0; i < LED_STRIP_BIT_1_NOPS_LOW; i++) {
         __asm__ volatile ("nop");
    }
}

static void led_strip_show_ws2812b(struct led_color_t *led_color, gpio_num_t gpio_num) 
{
	uint8_t green = led_color->green;

	for (uint8_t i = 7; i != 0; i--) {
		uint8_t bit = (green >> i) & 1;
		if (bit) {
			led_strip_1_bit(gpio_num);
		} else {
			led_strip_0_bit(gpio_num);
		}
	}
	if(green & 1) {
		led_strip_1_bit(gpio_num);
	} else {
		led_strip_0_bit(gpio_num);
	}

	uint8_t red = led_color->red;

	for (uint8_t i = 7; i != 0; i--) {
		uint8_t bit = (red >> i) & 1;
		if (bit) {
			led_strip_1_bit(gpio_num);
		} else {
			led_strip_0_bit(gpio_num);
		}
	}
	if(red & 1) {
		led_strip_1_bit(gpio_num);
	} else {
		led_strip_0_bit(gpio_num);
	}

	uint8_t blue = led_color->blue;

	for (uint8_t i = 7; i != 0; i--) {
		uint8_t bit = (blue >> i) & 1;
		if (bit) {
			led_strip_1_bit(gpio_num);
		} else {
			led_strip_0_bit(gpio_num);
		}
	}
	if(blue & 1) {
		led_strip_1_bit(gpio_num);
	} else {
		led_strip_0_bit(gpio_num);
	}
}

static void led_strip_task(void *arg)
{
	struct led_strip_t *led_strip = (struct led_strip_t *)arg;
	portMUX_TYPE mux;
	vPortCPUInitializeMutex(&mux);

	for(;;) {

		taskENTER_CRITICAL(&mux);
		struct led_color_t *led_colors = NULL;

		if (led_strip->showing_buf_1) {
			led_colors = led_strip->led_strip_buf_1;
		} else {
			led_colors = led_strip->led_strip_buf_2;
		}

		for (uint32_t led_index = 0; led_index < led_strip->led_strip_length; led_index++) {
			struct led_color_t led_color = led_colors[led_index];

			// TODO: remove this logic from loop to speed up 
			switch(led_strip->rgb_led_type) {
				case RGB_LED_TYPE_WS2812:
					led_strip_show_ws2812b(&led_color, led_strip->pin_num);
					break;
				default:
					// error
					break;
			}
		}

		taskEXIT_CRITICAL(&mux);
        vTaskDelay(LED_STRIP_REFRESH_PERIOD_MS / portTICK_PERIOD_MS);
	}
}

/**
 * Before calling this, make sure that the GPIO that you will be using has already been configured
 */
bool led_strip_init(struct led_strip_t *led_strip)
{
	bool init_ok = true;
	TaskHandle_t led_strip_task_handle;

	if ((led_strip == NULL) || 
		(led_strip->pin_num > GPIO_NUM_33) || // GPIO above 33 is input only
		(led_strip->rgb_led_type == RGB_LED_TYPE_MAX) || 
		(led_strip->led_strip_buf_1 == NULL) ||
		(led_strip->led_strip_buf_2 == NULL) || 
		(led_strip->led_strip_length == 0)) {
		return false;
	}

	// Ensure the two buffers aren't the same
	if(led_strip->led_strip_buf_1 == led_strip->led_strip_buf_2) {
		return false;
	}

    led_strip->showing_buf_1 = true;

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

	led_strip->led_strip_task_handle = led_strip_task_handle;

    return init_ok;
}

/**
 * Deletes the LED strip task
 */
bool led_strip_delete(struct led_strip_t *led_strip) {
	bool delete_success = true;

	if (!led_strip) {
		return false;
	}

	if(led_strip->led_strip_task_handle != NULL) {
		vTaskDelete(led_strip->led_strip_task_handle);
		led_strip->led_strip_task_handle = NULL;
	} else {
		delete_success = false;
	}

	return delete_success;
}

bool led_strip_set_pixel_color(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color)
{
	bool set_led_success = true;

	if((!led_strip) || (!color) || (pixel_num > led_strip->led_strip_length)) {
		return false;
	}

	if (led_strip->showing_buf_1) {
		led_strip->led_strip_buf_2[pixel_num] = *color;
	} else {
		led_strip->led_strip_buf_1[pixel_num] = *color;
	}

	return set_led_success;
}

/**
 * Updates the led buffer to be shown
 */
bool led_strip_show(struct led_strip_t *led_strip)
{
	bool success = true;

	if (!led_strip) {
		return false;
	}

	if (led_strip->showing_buf_1) {
		led_strip->showing_buf_1 = false;
	    memset(led_strip->led_strip_buf_1, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
	} else {
		led_strip->showing_buf_1 = true;
	    memset(led_strip->led_strip_buf_2, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
	}

	return success;
}

/**
 * Clears the LED strip
 */
bool led_strip_clear(struct led_strip_t *led_strip)
{
	bool success = true;

	if (!led_strip) {
		return false;
	}

	if (led_strip->showing_buf_1) {
    	memset(led_strip->led_strip_buf_2, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
	} else {
    	memset(led_strip->led_strip_buf_1, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
	}

	return success;
}
