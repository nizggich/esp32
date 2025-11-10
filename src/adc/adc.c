#include "adc.h"

static const char *TAG = "ADC";

void adc_continous_init(adc_continuous_handle_t adc_handle, adc_channel_t *channel, uint32_t channel_num) 
{
    adc_continuous_handle_cfg_t adc_handle_cfg = {
        .max_store_buf_size = 32, 
        .conv_frame_size = 16,
    };

    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_handle_cfg, &adc_handle));

    adc_continuous_config_t adc_cfg= {
        .pattern_num = channel_num,
        .sample_freq_hz = 20*1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };
 

    adc_digi_pattern_config_t pattern_cfg[channel_num];

    for (int i = 0; i < channel_num; i++)
    {
        pattern_cfg[i].atten = ADC_ATTEN_DB_12;
        pattern_cfg[i].channel = channel[i];
        pattern_cfg[i].unit = ADC_UNIT_1;
        pattern_cfg[i].bit_width = ADC_BITWIDTH_12; //SOC_ADC_DIGI_MAX_BITWIDTH
    }

    adc_cfg.adc_pattern = pattern_cfg;
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &adc_cfg));
}

void adc_oneshot_init(adc_oneshot_unit_handle_t *out_handle, adc_channel_t *channel, int channel_count) {

    adc_oneshot_unit_handle_t adc_handle = NULL;
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t channel_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };
     
    for (int i = 0; i < channel_count; i++)
    {
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel[i], &channel_cfg));
    }
    
    *out_handle = adc_handle;
}