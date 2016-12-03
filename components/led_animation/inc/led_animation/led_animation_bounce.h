/*  ---------------------------------------------------------------------------
    File: led_animation.h
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 12/5/2016
    Last modified: 12/5/2016

	Description: Library for creating animations with LED strips.

    ------------------------------------------------------------------------ */

#ifndef _LED_ANIMATION_H_
#define _LED_ANIMATION_H_

#include "led_animation/led_animation.h"

enum led_animation_bounce_style_t {

	/*
	 * Normal bouncing, the light is treated as a solid object
	 */
	LED_ANIMATION_BOUNCE_NORMAL = 0,

	/*
	 * When LEDs bounce and hit the beginning or end of the strip,
	 * the color will 'turn around' and will overlap, kind of like a snake
	 * Logically, the r, g, and b will be or'd together
	 */
	LED_ANIMATION_BOUNCE_SNAKE_OR,
	// Same, but r,g,b colors will be added
	LED_ANIMATION_BOUNCE_SNAKE_ADD
};

enum led_color_style_t {
	// All of the light in the "bouncer" is the same
	LED_COLOR_STYLE_SAME = 0,
	// Lights fade out from the center
	LED_COLOR_STYLE_FADE_FROM_CENTER,
	// Lights fade from the tail of the moving "bouncer"
	LED_COLOR_STYLE_FADE_FROM_TAIL,
};

// Data relevant to bounce animation
struct led_animation_bounce_data_t {
	uint32_t bounce_center_index;
	uint16_t bounce_led_width;
	enum led_animation_bounce_style_t bounce_style;
	enum led_color_type_t color_style;

	uint32_t velocity_pixels_per_second;
};


#endif // _LED_ANIMATION_H_