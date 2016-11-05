#ifndef LED_WALL_H
#define LED_WALL_H

enum led_wall_corner_t {
	LED_WALL_EDGE_TOP_LEFT = 0,
	LED_WALL_EDGE_TOP_RIGHT,
	LED_WALL_CORNER_BOTTM_LEFT,
	LED_WALL_CORNER_BOTTOM_RIGHT,
};

struct led_wall_context_t {

};

bool led_wall_init(struct led_wall_context_t *context);

bool led_wall_set_pixel_at_wall_corner(struct led_wall_context_t *context, uint32_t wall_num,
                            led_wall_corner_t corner, struct led_strip_t *led_strip, uint32_t led_strip_num);

#endif // LED_WALL_H