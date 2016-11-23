/*  ----------------------------------------------------------------------------
    File: led_strip.h
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/25/2016

    Description: LED Library for driving various led strips
    
    ------------------------------------------------------------------------- */

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define LED_STRIP_REFRESH_PERIOD_MS 30

enum rgb_led_type_t {
	// GRB
	RGB_LED_TYPE_WS2812 = 0,

	RGB_LED_TYPE_MAX
};

/**
 * RGB LED colors
 */
struct led_color_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct led_strip_t {
	gpio_num_t pin_num;

	enum rgb_led_type_t rgb_led_type;

	/*
	 * This library uses double buffering to display the LEDs
	 * If the driver is showing buffer 1, any calls to set pixel color
	 * will write to buffer 2. When led_strip_show is called, it clears strip 1,
	 * starts displaying from buffer 2, and all calls to set pixel color will
	 * write to buffer 1.
	 */
	bool showing_buf_1;
	struct led_color_t *led_strip_buf_1;
	struct led_color_t *led_strip_buf_2;
	uint32_t led_strip_length;

	TaskHandle_t led_strip_task_handle;
};

/**
 * LED strip initializer.
 * NOTE: Right now using nop loops, timing assumes clock frequency of 240MHz
 * NOTE: GPIO pin number must be configured as output before calling this function
 */
bool led_strip_init(struct led_strip_t *led_strip);

/**
 * Untested right now
 */
bool led_strip_delete(struct led_strip_t *led_strip);

/**
 * Sets the pixel at pixel_num to color
 */
bool led_strip_set_pixel_color(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color);

/**
 * Updates the led buffer to be shown
 */
bool led_strip_show(struct led_strip_t *led_strip);

/**
 * Clears the LED strip
 */
bool led_strip_clear(struct led_strip_t *led_strip);

#endif // LED_STRIP_H
