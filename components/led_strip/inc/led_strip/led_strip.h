/*  ----------------------------------------------------------------------------
    File: led_strip.h
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/23/2016

    Description: LED Library for driving various led strips
    
    ------------------------------------------------------------------------- */

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <driver/gpio.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum rgb_led_type_t {
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

	// Double buffer led strips
	bool using_buf_1;
	struct led_color_t *led_strip_buf_1;
	struct led_color_t *led_strip_buf_2;
	uint32_t led_strip_length;
};

bool led_strip_init(struct led_strip_t *led_strip);

/**
 * Sets the pixel at pixel_num to color
 */
bool led_strip_set_rgb_led(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color);

/**
 * Updates the led buffer to be shown
 */
bool led_strip_show(struct led_strip_t *led_strip);

/**
 * Clears the LED strip
 */
bool led_strip_clear(struct led_strip_t *led_strip);

#endif // LED_STRIP_H
