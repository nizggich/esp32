#include "led.h"

static void led_set(led_strip_handle_t strip, uint32_t brightness, enum LedColor color)
{
    switch (color)
    {
        case LED_COLOR_RED:
            led_strip_set_pixel(strip, 0, brightness, 0, 0);
            break;

        case LED_COLOR_GREEN:
            led_strip_set_pixel(strip, 0, 0, brightness, 0);
            break;

        case LED_COLOR_BLUE:
            led_strip_set_pixel(strip, 0, 0, 0, brightness);
            break;

        case LED_COLOR_PURPLE:
            led_strip_set_pixel(strip, 0, brightness, 0, brightness);
            break;

        case LED_COLOR_CYAN:
            led_strip_set_pixel(strip, 0, 0, brightness, brightness);
            break;

        case LED_COLOR_YELLOW:
            led_strip_set_pixel(strip, 0, brightness, brightness, 0);
            break;

        case LED_COLOR_WHITE:
            led_strip_set_pixel(strip, 0, brightness, brightness, brightness);
            break;

        default:
            led_strip_set_pixel(strip, 0, brightness, brightness, brightness);
            break;
    }
}

void led_tick(struct LedState *state)
{
    if (!state->is_initialized)
    {
        return;
    }

    TickType_t current_tick = xTaskGetTickCount();

    if (state->signal_state.is_set)
    {
        if (state->signal_state.is_fired)
        {
            led_strip_clear(state->strip);
            state->signal_state.is_set = false;
            state->mode_is_changed = true;
            return;
        }

        led_set(state->strip, state->brightness, state->signal_state.color);
        led_strip_refresh(state->strip);
        state->signal_state.is_fired = true;
        return;
    }

    if (state->mode_is_changed)
    {
        state->mode_is_changed = false;
        switch (state->mode)
        {
            case LED_MODE_OFF:
                led_strip_clear(state->strip);
                break;

            case LED_MODE_CONSTANT:
                led_set(state->strip, state->brightness, state->color);
                led_strip_refresh(state->strip);
                break;

            case LED_MODE_BLINK:
                led_set(state->strip, state->brightness, state->color);
                led_strip_refresh(state->strip);
                state->blink_state.next_blink_at_tick =
                    current_tick + pdMS_TO_TICKS(state->blink_state.blink_period);
                state->blink_state.last_state = true;
                break;

            default:
                break;
        }
        return;
    }

    if (state->mode == LED_MODE_BLINK && current_tick > state->blink_state.next_blink_at_tick)
    {
        state->blink_state.next_blink_at_tick =
            current_tick + pdMS_TO_TICKS(state->blink_state.blink_period);
        if (state->blink_state.last_state)
        {
            led_strip_clear(state->strip);
        }
        else
        {
            led_set(state->strip, state->brightness, state->color);
            led_strip_refresh(state->strip);
        }
        state->blink_state.last_state = !state->blink_state.last_state;
        return;
    }
}

void led_init(size_t gpio, uint8_t brightness, struct LedState *state)
{
    memset(state, 0, sizeof(struct LedState));
    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio,
        .max_leds = 1,
    };
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI3_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &state->strip));
    led_strip_clear(state->strip);
    state->brightness = brightness;
    state->is_initialized = true;
}

void led_blink(enum LedColor color, enum LedBlinkPeriod period, struct LedState *state)
{
    state->mode_is_changed = true;
    state->color = color;
    state->mode = LED_MODE_BLINK;
    state->blink_state.blink_period = period;
}

void led_turn_on(enum LedColor color, struct LedState *state)
{
    state->mode_is_changed = true;
    state->mode = LED_MODE_CONSTANT;
    state->color = color;
}

void led_turn_off(struct LedState *state)
{
    state->mode_is_changed = true;
    state->mode = LED_MODE_OFF;
}

void led_signal(enum LedColor color, struct LedState *state)
{
    state->signal_state.is_set = true;
    state->signal_state.is_fired = false;
    state->signal_state.color = color;
}