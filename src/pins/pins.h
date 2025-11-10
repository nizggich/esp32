#include "driver/gpio.h"

typedef enum
{
    PIN_MODE_INPUT,
    PIN_MODE_INPUT_PULLUP,
    PIN_MODE_INPUT_PULLDOWN,
    PIN_MODE_OUTPUT_LOW,
    PIN_MODE_OUTPUT_HIGH
} pin_mode_t;

typedef struct
{
    gpio_num_t pin;
    pin_mode_t mode;
} pin_config_t;


void configure_pins(void);