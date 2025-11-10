#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"

void adc_continous_init(adc_continuous_handle_t adc_handle, adc_channel_t *channel, uint32_t channel_num);
void adc_oneshot_init(adc_oneshot_unit_handle_t *out_handle, adc_channel_t *channel, int channel_count);