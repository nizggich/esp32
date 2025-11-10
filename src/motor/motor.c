#include "motor.h"

#define MOTOR_GPIO GPIO_NUM_7

void motor_on() 
{
    gpio_set_level(MOTOR_GPIO, 1);
}


void motor_off() 
{
    gpio_set_level(MOTOR_GPIO, 0);
}