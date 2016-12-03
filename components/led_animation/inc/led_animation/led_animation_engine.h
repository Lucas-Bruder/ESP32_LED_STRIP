/*  ---------------------------------------------------------------------------
    File: led_animation.h
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 12/2/2016
    Last modified: 12/2/2016

	Description: Engine for driving animations with LED strips.

    ------------------------------------------------------------------------ */

#ifndef LED_ANIMATION_ENGINE_H
#define LED_ANIMATION_ENGINE_H

#include "led_strip/led_strip.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

enum led_animation_animation_end_effect_t {
	LED_ANIMATION_END_EFFECT_NONE = 0,

	LED_ANIMATION_END_EFFECT_FADE_BRIGHTNESS,
	LED_ANIMATION_END_EFFECT_MAX;
};

// Function that changes the LED strip animation after one "step"
typedef void (*led_animation_step)(struct *led_strip_t led_strip);

struct led_animation_engine_t {
	struct led_strip_t *led_strips;
	uint32_t num_led_strips;

	uint32_t _refresh_tick_rate_hz; // How often to "tick"
	uint64_t _refresh_tick_count;   // count of ticks to use in led_animation_step for current function
};

bool led_animation_init(struct led_animation_data_t *led_animation_engine_t, uint32_t refresh_tick_rate);
bool led_animation_set_tick_rate(struct led_animation_data_t *led_animation_engine_t, uint32_t new_refresh_tick_rate);

bool led_animation_start_builtin_animation(struct led_animation_data_t *led_animation_engine_t, 
											enum led_animation_builtin_t builtin,
												uint32_t max_wait_to_start_ms);
bool led_animation_start_custom_animation(struct led_animation_data_t *led_animation_engine_t, 
											led_animation_step *led_animation,
												uint32_t max_wait_to_start_ms);

bool led_animation_is_animation_playing(struct led_animation_data_t *led_animation_engine_t);

bool led_animation_end(struct led_animation_data_t *led_animation_engine_t, 
						enum led_animation_animation_end_effect_t end_effect);

// OVERLAP animations

#endif // LED_ANIMATION_ENGINE_H
