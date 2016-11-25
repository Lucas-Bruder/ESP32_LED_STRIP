/*  ----------------------------------------------------------------------------
    File: led_strip.c
    Author(s):  Lucas Bruder <LBruder@me.com>
    Date Created: 11/23/2016
    Last modified: 11/26/2016

    Description: LED Library for driving various led strips on ESP32.

    This library uses double buffering to display the LEDs.
    If the driver is showing buffer 1, any calls to led_strip_set_pixel_color
    will write to buffer 2. When it's time to drive the pixels on the strip, it
    refers to buffer 1. 
    When led_strip_show is called, it will switch to displaying the pixels
    from buffer 2 and will clear buffer 1. Any writes will now happen on buffer 1 
    and the task will look at buffer 2 for refreshing the LEDs
    ------------------------------------------------------------------------- */

#include "led_strip/led_strip.h"
#include "freertos/task.h"

#include <string.h>

#define LED_STRIP_TASK_SIZE             (512)
#define LED_STRIP_TASK_PRIORITY         (configMAX_PRIORITIES - 1)

#define LED_STRIP_REFRESH_PERIOD_MS     (30U) // TODO: add as parameter to led_strip_init
#define LED_STRIP_RMT_INTR_NUM          (19U) // TODO: add some internal tracking to abstract this for multiple led strips

// Each LED is 24 bits of color. For one bit of color, theres a high period and low period.
// Each rmt_item32_t has two levels and durations associated with it, so each LED will need 24 rmt_items
#define LED_STRIP_NUM_RMT_ITEMS_PER_LED (24U)

// RMT Clock source is @ 80 MHz. Dividing it by 8 gives us 10 MHz frequency, or 100ns period.
#define LED_STRIP_RMT_CLK_DIV (8)

// Each tick is 100ns with a clock divider of 8. 
// The period for each high bit and low bit are pulled from WS2812 Datasheet and converted to ticks
// For both bit 1 and 0, 900ns + 300ns = 1200ns, which fits the timing requirement of 1250ns +/- 150ns
#define LED_STRIP_RMT_TICKS_BIT_1_HIGH 9 // 900ns (900ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_1_LOW  3 // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_HIGH 3 // 300ns (350ns +/- 150ns per datasheet)
#define LED_STRIP_RMT_TICKS_BIT_0_LOW  9 // 900ns (900ns +/- 150ns per datasheet)

inline void led_strip_fill_item_level(rmt_item32_t* item, int high_ticks, int low_ticks)
{
    item->level0 = 1;
    item->duration0 = high_ticks;
    item->level1 = 0;
    item->duration1 = low_ticks;
}

inline void led_strip_rmt_bit_1(rmt_item32_t* item)
{
    led_strip_fill_item_level(item, LED_STRIP_RMT_TICKS_BIT_1_HIGH, LED_STRIP_RMT_TICKS_BIT_1_LOW);
}

inline void led_strip_rmt_bit_0(rmt_item32_t* item)
{
    led_strip_fill_item_level(item, LED_STRIP_RMT_TICKS_BIT_0_HIGH, LED_STRIP_RMT_TICKS_BIT_0_LOW);
}

inline void led_strip_rmt_end(rmt_item32_t* item)
{
    item->level0 = 1;
    item->duration0 = 1;
    item->level1 = 0;
    item->duration1 = 0;
}

static void led_strip_fill_rmt_items(struct led_color_t *led_strip_buf, rmt_item32_t *rmt_items, uint32_t led_strip_length)
{
    uint32_t rmt_items_index = 0;
    for (uint32_t led_index = 0; led_index < led_strip_length; led_index++) {
        struct led_color_t led_color = led_strip_buf[led_index];

        for (uint8_t bit = 8; bit != 0; bit--) {
            uint8_t bit_set = (led_color.green >> (bit - 1)) & 1;
            if(bit_set) {
                led_strip_rmt_bit_1(&(rmt_items[rmt_items_index]));
            } else {
                led_strip_rmt_bit_0(&(rmt_items[rmt_items_index]));
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--) {
            uint8_t bit_set = (led_color.red >> (bit - 1)) & 1;
            if(bit_set) {
                led_strip_rmt_bit_1(&(rmt_items[rmt_items_index]));
            } else {
                led_strip_rmt_bit_0(&(rmt_items[rmt_items_index]));
            }
            rmt_items_index++;
        }
        for (uint8_t bit = 8; bit != 0; bit--) {
            uint8_t bit_set = (led_color.blue >> (bit - 1)) & 1;
            if(bit_set) {
                led_strip_rmt_bit_1(&(rmt_items[rmt_items_index]));
            } else {
                led_strip_rmt_bit_0(&(rmt_items[rmt_items_index]));
            }
            rmt_items_index++;
        }
    }
}

static void led_strip_task(void *arg)
{
    struct led_strip_t *led_strip = (struct led_strip_t *)arg;
    portMUX_TYPE mux;
    vPortCPUInitializeMutex(&mux);

    size_t num_items_malloc = (LED_STRIP_NUM_RMT_ITEMS_PER_LED * led_strip->led_strip_length);
    rmt_item32_t *rmt_items = (rmt_item32_t*) malloc(sizeof(rmt_item32_t) * num_items_malloc);

    if (!rmt_items) {
        vTaskDelete(NULL);
    }

    for(;;) {
        taskENTER_CRITICAL(&mux);

        rmt_wait_tx_done(led_strip->rmt_channel);
        // TODO add logic so this isn't called every single time if previous loop was showing buf 1
        if (led_strip->showing_buf_1) {
            led_strip_fill_rmt_items(led_strip->led_strip_buf_1, rmt_items, led_strip->led_strip_length);
        } else {
            led_strip_fill_rmt_items(led_strip->led_strip_buf_2, rmt_items, led_strip->led_strip_length);
        }

        rmt_write_items(led_strip->rmt_channel, rmt_items, num_items_malloc, false);

        taskEXIT_CRITICAL(&mux);
        vTaskDelay(LED_STRIP_REFRESH_PERIOD_MS / portTICK_PERIOD_MS);
    }

    if (!rmt_items) {
        free(rmt_items);
    }
    vTaskDelete(NULL);
}

static bool led_strip_init_rmt(struct led_strip_t *led_strip)
{
    rmt_config_t rmt_cfg = {
        .rmt_mode = RMT_MODE_TX,
        .channel = led_strip->rmt_channel,
        .clk_div = LED_STRIP_RMT_CLK_DIV,
        .gpio_num = led_strip->gpio,
        .mem_block_num = 1,
        .tx_config = {
            .loop_en = false,
            .carrier_freq_hz = 100, // Not used, but has to be set to avoid divide by 0 err
            .carrier_duty_percent = 50,
            .carrier_level = RMT_CARRIER_LEVEL_LOW,
            .carrier_en = false,
            .idle_level = RMT_IDLE_LEVEL_LOW,
            .idle_output_en = true,
        }
    };

    esp_err_t err = rmt_config(&rmt_cfg);
    if (err != ESP_OK) {
        return false;
    }
    esp_err_t install_ok = rmt_driver_install(rmt_cfg.channel, 0, LED_STRIP_RMT_INTR_NUM);
    if (install_ok != ESP_OK) {
        return false;
    }

    return true;
}

bool led_strip_init(struct led_strip_t *led_strip)
{
    TaskHandle_t led_strip_task_handle;

    if ((led_strip == NULL) ||
        (led_strip->rmt_channel == RMT_CHANNEL_MAX) ||
        (led_strip->gpio > GPIO_NUM_33) ||  // only inputs above 33
        (led_strip->led_strip_buf_1 == NULL) ||
        (led_strip->led_strip_buf_2 == NULL) ||
        (led_strip->led_strip_length == 0)) {
        return false;
    }

    if(led_strip->led_strip_buf_1 == led_strip->led_strip_buf_2) {
        return false;
    }

    memset(led_strip->led_strip_buf_1, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);
    memset(led_strip->led_strip_buf_2, 0, sizeof(struct led_color_t) * led_strip->led_strip_length);

    bool init_rmt = led_strip_init_rmt(led_strip);
    if (!init_rmt) {
        return false;
    }

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

    return true;
}

bool led_strip_set_pixel_color(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color)
{
    bool set_led_success = true;

    if ((!led_strip) || (!color) || (pixel_num > led_strip->led_strip_length)) {
        return false;
    }

    if (led_strip->showing_buf_1) {
        led_strip->led_strip_buf_2[pixel_num] = *color;
    } else {
        led_strip->led_strip_buf_1[pixel_num] = *color;
    }

    return set_led_success;
}

bool led_strip_get_pixel_color(struct led_strip_t *led_strip, uint32_t pixel_num, struct led_color_t *color)
{
    bool get_success = true;

    if ((!led_strip) ||
        (pixel_num > led_strip->led_strip_length) ||
        (!color)) {
        color = NULL;
        return false;
    }

    if (led_strip->showing_buf_1) {
        *color = led_strip->led_strip_buf_1[pixel_num];
    } else {
        *color = led_strip->led_strip_buf_2[pixel_num];
    }

    return get_success;
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
