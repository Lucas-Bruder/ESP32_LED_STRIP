#include "led_animation/led_animation.h"

bool led_animation_init(struct led_animation_t *led_animation)
{
	bool init_ok = false;

	return init_ok;
}

void led_animation_set_init_cb(struct led_animation_t *led_animation, led_animate_init init);
void led_animation_set_animate_cb(struct led_animation_t *led_animation, led_animate animate);
void led_animation_change_settings_cb(struct led_animation_t *led_animation, led_change_animation_settings_cb change_settings);

bool led_animation_change_settings(struct led_animation_t *led_animation, void *data);
void led_animation_animate(struct led_animation_t *led_animation, uint32_t step_count);