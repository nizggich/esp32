#pragma once

#include <stdbool.h>
#include <string.h>

#include "led_strip.h"

enum LedColor
{
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_PURPLE,
    LED_COLOR_CYAN,
    LED_COLOR_YELLOW,
    LED_COLOR_WHITE,
};

enum LedMode
{
    LED_MODE_OFF,
    LED_MODE_CONSTANT,
    LED_MODE_BLINK,
};

enum LedBlinkPeriod
{
    LED_BLINK_PERIOD_SHORT = 200U,
    LED_BLINK_PERIOD_LONG = 1000U
};

struct LedBlinkState
{
    enum LedBlinkPeriod blink_period;
    bool last_state;
    TickType_t next_blink_at_tick;
};

struct LedSignalState
{
    bool is_set;
    bool is_fired;
    enum LedColor color;
};

struct LedState
{
    bool is_initialized;
    bool mode_is_changed;
    led_strip_handle_t strip;
    uint8_t brightness;
    enum LedMode mode;
    enum LedColor color;
    struct LedBlinkState blink_state;
    struct LedSignalState signal_state;
};

void led_init(size_t gpio, uint8_t brightness, struct LedState *state);
void led_tick(struct LedState *state);
void led_blink(enum LedColor color, enum LedBlinkPeriod period, struct LedState *state);
void led_turn_on(enum LedColor color, struct LedState *state);
void led_turn_off(struct LedState *state);
void led_signal(enum LedColor color, struct LedState *state);