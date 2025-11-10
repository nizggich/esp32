#include "pins.h"

static pin_config_t pins_cfg[] = 
{
    // {GPIO_NUM_7,  PIN_MODE_OUTPUT_LOW}, //motor on/off
    // {GPIO_NUM_11, PIN_MODE_INPUT}, //r-button
    // {GPIO_NUM_12, PIN_MODE_INPUT}, //r-button
    // {GPIO_NUM_13, PIN_MODE_INPUT}, //r-button
    // {GPIO_NUM_14, PIN_MODE_INPUT}, //r-button
    // {GPIO_NUM_15, PIN_MODE_INPUT}, //l-button
    // {GPIO_NUM_16, PIN_MODE_INPUT}, //l-button
    // {GPIO_NUM_17, PIN_MODE_INPUT}, //l-button
    // {GPIO_NUM_18, PIN_MODE_INPUT}, //l-button
    // {GPIO_NUM_39, PIN_MODE_INPUT}, //stop-button, default lvl = 1
    // {GPIO_NUM_40, PIN_MODE_INPUT}, //NTC
    // {GPIO_NUM_47, PIN_MODE_INPUT}, //SDA
    // {GPIO_NUM_48, PIN_MODE_INPUT}, //SCL
        {GPIO_NUM_18, PIN_MODE_OUTPUT_HIGH},
        {GPIO_NUM_17, PIN_MODE_OUTPUT_HIGH}
};

int pins_count = sizeof(pins_cfg) / sizeof(pin_config_t);

void configure_pins(void) 
{   

    gpio_config_t gpio_cfg = {
            .mode = GPIO_MODE_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
        };
    
    pin_config_t pin_cfg = {0};   

    for (int i = 0; i < pins_count; i++)
    {
       pin_cfg = pins_cfg[i];

       gpio_cfg.pin_bit_mask = pin_cfg.pin;

       switch (pin_cfg.mode)
       {
            case PIN_MODE_INPUT:
                gpio_cfg.mode = GPIO_MODE_INPUT;
                gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
                break;
            case PIN_MODE_INPUT_PULLUP:
                gpio_cfg.mode = GPIO_MODE_INPUT;
                gpio_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
                gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
                break;
            case PIN_MODE_INPUT_PULLDOWN:
                gpio_cfg.mode = GPIO_MODE_INPUT;
                gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
                gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
                break;
            case PIN_MODE_OUTPUT_LOW:
            case PIN_MODE_OUTPUT_HIGH:
                gpio_cfg.mode = GPIO_MODE_OUTPUT;
                break;
            default:
                break;
       }

       ESP_ERROR_CHECK(gpio_config(&gpio_cfg));

       if (pin_cfg.mode == PIN_MODE_OUTPUT_HIGH) 
       {
            ESP_ERROR_CHECK(gpio_set_level(pin_cfg.pin, 1));
       }

       if (pin_cfg.mode == PIN_MODE_OUTPUT_LOW) 
       {
            ESP_ERROR_CHECK(gpio_set_level(pin_cfg.pin, 0));
       }
    }

    
}
