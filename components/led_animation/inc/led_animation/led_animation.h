/*  ---------------------------------------------------------------------------
    File: led_animation.h
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 12/5/2016
    Last modified: 12/5/2016

	Description: Library for creating animations with LED strips.

    ------------------------------------------------------------------------ */

#ifndef _LED_ANIMATION_H_
#define _LED_ANIMATION_H_

#include "led_strip/led_strip.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * LED animations will get tick'd every 10ms and increment a counter in the 
 * struct. There shouldn't be any reason that animations need to go any faster than this.
 */
#define LED_ANIMATION_TICK_RATE_MS 10 
inline uint32_t led_animation_time_ms_to_num_ticks(uint32_t time_ms) {
	return time_ms / LED_ANIMATION_TICK_RATE_MS;
}

struct led_animation_t;

typedef bool (*led_animate_init)(struct led_animation_t *led_animation);
// Main entry point to an animation state machine / function
typedef void (*led_animate)(struct led_animation_t *led_animation, struct led_strip_t *led_strip);
// A callback for changing any settings in the animation data
typedef void (*led_change_animation_settings_cb)(struct led_animation_t *led_animation);
typedef uint64_t (*led_animate_get_ticks_to_wait)(struct led_animation_t *led_animation);

struct led_animation_t {
	led_animate animate;
	led_change_animation_settings_cb change_settings;
	led_animate_init init;
	led_animate_get_ticks_to_wait ticks_to_wait;
	void *data;

	uint64_t current_tick;
};

bool led_animation_init(struct led_animation_t *led_animation);

void led_animation_set_init_cb(struct led_animation_t *led_animation, led_animate_init init);
void led_animation_set_animate_cb(struct led_animation_t *led_animation, led_animate animate);
void led_animation_change_settings_cb(struct led_animation_t *led_animation, led_change_animation_settings_cb change_settings);

bool led_animation_change_settings(struct led_animation_t *led_animation, void *data);
void led_animation_animate(struct led_animation_t *led_animation, uint32_t step_count);

#endif // _LED_ANIMATION_H_