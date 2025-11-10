// #include "pwm.h"

// void pwm_init() 
// {   
//     esp_err_t ret;

//     ledc_channel_config_t channel_cfg = {
//         .gpio_num = RGB_GPIO,
//         .speed_mode = LEDC_SPEED_MODE,
//         .channel = LEDC_CHANNEL,
//         .intr_type = LEDC_INTR_DISABLE,
//         .timer_sel = LEDC_TIMER_0,
//         .duty = 0,
//         .hpoint = 0,
//     };

//     ledc_timer_config_t timer_cfg = {
//         .freq_hz = 10000,
//         .timer_num = LEDC_TIMER_0,
//         .duty_resolution = LEDC_TIMER_10_BIT,
//         .speed_mode = LEDC_SPEED_MODE,
//         .clk_cfg = LEDC_AUTO_CLK
//     };

//     ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));
//     ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));
// }

// void set_duty(ledc_channel_t channel, uint32_t duty) {
//     ledc_set_duty(LEDC_SPEED_MODE, channel, duty);
//     ledc_update_duty(LEDC_SPEED_MODE, channel);
// }
