#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "pins/pins.h"
#include "motor/motor.h"
#include "wifi/wifi_sta.h"
#include "adc/adc.h"
#include "led/led.h"
#include "udp/udp_server.h"
#include "bluetooth/bt_scan/bt_scan.h"
#include "i2c/mpu6050/mpu6050.h"
#include "spi/spi.h"


#define BUTTONS_NUM 9
#define ADC_CHANNEL_NUM 4
#define RGB_GPIO GPIO_NUM_38

#define WIFI_SSID "SETKA"
#define WIFI_PASS "CPLDGBCJ337555"

#define IP_STRING "0.0.0.0"
#define PORT_STRING "9999"

#define who_Am_I 0x75
#define MPU6050_NACK_VAL 0x0
#define MPU6050_I2C_ADDR 0x70
#define MPU6050_I2C_CH1  0x01
#define MPU6050_ADD 0x68
#define MPU6050_PWR 0x6B
#define MPU6050_RAW_GYRO 0x43
#define MPU6050_ACK_VAL 0x1
#define MPU6050_NACK_VAL 0x0

static const char *TAG_BUTTON = "BUTTON";
static const char *TAG_PWM = "PWM";
static const char *TAG_ADC = "ADC";
static const char *TAG_MOTOR = "MOTOR";
static const char *TAG_UDP = "UDP_SERVER";
static const char *TAG_SPI = "SPI";
static const char *TAG_BLE = "BLE";

static TaskHandle_t joystick_check_task_handle;
static TaskHandle_t button_check_task_handle;
static TaskHandle_t wifi_check_status_task_handler;

static uint32_t duty = 0;

static adc_channel_t channels[ADC_CHANNEL_NUM] = 
{
    ADC_CHANNEL_0, ADC_CHANNEL_1, //GPIO_NUM_1, GPIO_NUM_2
    ADC_CHANNEL_4, ADC_CHANNEL_5 // GPIO_NUM_5, GPIO_NUM_6
};

static int buttons[BUTTONS_NUM] = 
{
    GPIO_NUM_11, //r-button
    GPIO_NUM_12, //r-button
    GPIO_NUM_13, //r-button
    GPIO_NUM_14, //r-button
    GPIO_NUM_15, //l-button
    GPIO_NUM_16, //l-button
    GPIO_NUM_17, //l-button
    GPIO_NUM_18, //l-button
    GPIO_NUM_39, //stop-button, common lvl = 1
};

WifiState wifi_state = {
    .wifi_ssid = WIFI_SSID,
    .wifi_password = WIFI_PASS,
    .status = WIFI_CLIENT_DISCONNECTED
};

struct LedState led_state = {0};

bool network_is_available = false;

struct UdpServer udp_server = {0};


#define TEST_DATA_SIZE  4
uint8_t tx_data[TEST_DATA_SIZE] = {0x12, 0x34, 0x56, 0x78};
uint8_t rx_data[TEST_DATA_SIZE] = {0};


bool is_task_mode_oneshot = false;
bool is_btn_continous_log = false;
bool is_joystick_continous_log = false;
bool is_wifi_continous_log = true;
bool is_ntc_continous_log = true;

void adc_continous_task(void *arg) 
{   
    esp_err_t ret;
    uint32_t bytes_read = 0;
    uint8_t buff[4];

    adc_continuous_handle_t adc_handle = (adc_continuous_handle_t)arg;

    ret = adc_continuous_start(adc_handle);

    if (ret != ESP_OK) 
    {   
        ESP_LOGE(TAG_ADC, "Failed to start ADC continuous mode");
        vTaskDelete(NULL);
    }
    
    while (1) 
    {
        esp_err_t ret = adc_continuous_read(adc_handle, buff, 4, &bytes_read, 0);
        if (ret == ESP_OK) 
        {
            int num_smaples = bytes_read / sizeof(adc_digi_output_data_t);
            ESP_LOGI(TAG_ADC, "bytes_read: %"PRIu32"", bytes_read);
            ESP_LOGI(TAG_ADC, "num_smaples: %d", num_smaples);
            for (int i = 0; i < bytes_read; i+=SOC_ADC_DIGI_RESULT_BYTES)
            {
               adc_digi_output_data_t *data = (adc_digi_output_data_t*)&buff[i];
               uint32_t raw_data = data->type2.data;
               uint32_t channel = data->type2.channel;
               ESP_LOGI(TAG_ADC, "DATA: %"PRIu32"  from CHANNEL: %"PRIu32"", raw_data, channel);
            }
            
        }
        vTaskDelay(10);
    }
    
}

void joystick_check_task(void *arg) 
{
    adc_oneshot_unit_handle_t adc_handle = (adc_oneshot_unit_handle_t)arg; 
   
    int x1_raw = 0;
    int y1_raw = 0;

    int x2_raw = 0;
    int y2_raw = 0;

    while (1) 
    {  
        if (is_joystick_continous_log == 0)
        {   
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        adc_oneshot_read(adc_handle, channels[0], &x1_raw); 
        adc_oneshot_read(adc_handle, channels[1], &y1_raw); 
        adc_oneshot_read(adc_handle, channels[2], &x2_raw); 
        adc_oneshot_read(adc_handle, channels[3], &y2_raw); 
        
        ESP_LOGI(TAG_ADC, "x1: %d, y1: %d", x1_raw, y1_raw);
        ESP_LOGI(TAG_ADC, "x2: %d, y2: %d", x2_raw, y2_raw);
        printf("\r\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void button_check_task (void *arg) {
    int gpio_num = 0;
    int level = 0;

    while (1) 
    {   
        
        if (is_btn_continous_log == 0)
        {   
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        for (int i = 0; i < BUTTONS_NUM; i++)
        {   
            gpio_num = buttons[i];
            level = gpio_get_level(gpio_num);
            vTaskDelay(pdMS_TO_TICKS(100));
            ESP_LOGI(TAG_BUTTON, "BUTTON_%d, GPIO_NUM: %d, LEVEL: %d", i, gpio_num, level);
        }

    }
}

void wifi_check_status_task(void *arg) 
{
    WifiState *wifi_state = (WifiState*)arg;
    while (1) 
    {   
        if (is_wifi_continous_log == 0)
        {   
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        wifi_status_print(wifi_state);
        if (wifi_state->status == WIFI_CLIENT_WORKING)
        {
            network_is_available = true;
        } 
        else 
        {
            network_is_available = false;
        } 
        
        printf("\r\n");
        vTaskDelay(1000);
    }
}

void led_task(void *arg) 
{
    struct LedState *led_state = (struct LedState*)arg;

    while(1) 
    {
        led_tick(led_state);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


void NTC_check_task(void *arg) 
{
  
    adc_oneshot_unit_handle_t adc_handle = (adc_oneshot_unit_handle_t)arg; 
   
    int raw_val = 0;

    while (1) 
    {  
        if (is_ntc_continous_log == 0)
        {   
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        adc_oneshot_read(adc_handle, channels[0], &raw_val);  
        
        ESP_LOGI(TAG_ADC, "raw_val: %d", raw_val);
        printf("\r\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void udp_task_manager(void *arg)
{   
    udp_server_init(IP_STRING, PORT_STRING, &network_is_available, 1024, &udp_server);

    udp_server.status = UDP_SERVER_STATUS_ENABLED;
    ESP_LOGI(TAG_UDP, "UDP_SERV_STATUS: INIT_SUCCESS");
    while (1)
    {   
        udp_server_tick(&udp_server);
        if (udp_server_has_data(&udp_server))
        {    
            size_t msg_size = udp_server.bytes_received;
            //udp_server.p_buffer[msg_size] = (uint8_t)"/0";

            ESP_LOGI("MSG", "MSG_VALUE:%s", udp_server.p_buffer);

            if (strcmp((char*)udp_server.p_buffer, "led_on_r") == 0) 
            {   
                led_turn_on(LED_COLOR_RED, &led_state);
                ESP_LOGI(TAG_UDP, "LED IS RED!");
            }

            else if (strcmp((char*)udp_server.p_buffer, "led_on_y") == 0) 
            {   
                led_turn_on(LED_COLOR_YELLOW, &led_state);
                ESP_LOGI(TAG_UDP, "LED IS YELLOW!");
            }

            else if (strcmp((char*)udp_server.p_buffer, "led_on_g") == 0) 
            {   
                led_turn_on(LED_COLOR_GREEN, &led_state);
                ESP_LOGI(TAG_UDP, "LED IS GREEN!");
            }

            else if (strcmp((char*)udp_server.p_buffer, "led_off") == 0) 
            {   
                led_turn_off(&led_state);
                ESP_LOGI(TAG_UDP, "LED IS GREEN!");
            }

            else if (strcmp((char*)udp_server.p_buffer, "motor_on") == 0) 
            {   
                motor_on();
                ESP_LOGI(TAG_UDP, "MOTOR IS ON");
            }

            else if (strcmp((char*)udp_server.p_buffer, "motor_off") == 0) 
            {
                motor_off();
                ESP_LOGI(TAG_UDP, "MOTOR IS OFF");
            }

            else if (strcmp((char*)udp_server.p_buffer, "btn_log") == 0) 
            {   
                xTaskNotifyGive(button_check_task_handle);
                ESP_LOGI(TAG_UDP, "BUTTONS CHECK TASK NOTIFY");
            }

            else if (strcmp((char*)udp_server.p_buffer, "btn_continous_log") == 0) 
            {   
                is_btn_continous_log = is_btn_continous_log ? false : false;
                xTaskNotifyGive(button_check_task_handle);
                ESP_LOGI(TAG_UDP, "BTN CONTINUS LOG IS:%d", is_btn_continous_log);
            }

            else if (strcmp((char*)udp_server.p_buffer, "joystick_log") == 0) 
            {   
                xTaskNotifyGive(joystick_check_task_handle);
                ESP_LOGI(TAG_UDP, "JOYSTICK CHECK TASK NOTIFY");
            }

            else if (strcmp((char*)udp_server.p_buffer, "joystick_continous_log") == 0) 
            {   
                is_joystick_continous_log = is_joystick_continous_log ? false : true;
                xTaskNotifyGive(joystick_check_task_handle);
                ESP_LOGI(TAG_UDP, "JOYSTICK CONTINUS LOG IS: %d", is_joystick_continous_log);
            }

            else if (strcmp((char*)udp_server.p_buffer, "wifi_log") == 0) 
            {   
                xTaskNotifyGive(wifi_check_status_task_handler);
                ESP_LOGI(TAG_UDP, "WIFI CHECK TASK NOTIFY");
            }

            else if (strcmp((char*)udp_server.p_buffer, "wifi_continous_log") == 0) 
            {   
                is_wifi_continous_log = is_wifi_continous_log ? false : true;
                xTaskNotifyGive(wifi_check_status_task_handler);
                ESP_LOGI(TAG_UDP, "WIFI CONTINUS LOG IS: %d", is_wifi_continous_log);
            }
          
            udp_server_set_listening(&udp_server);
            memset(udp_server.p_buffer, 0, msg_size);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) 
{   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());

        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    for (int i = 0; i < 2; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI("DELAY", "DELAY");
    }

    configure_pins();
    i2c_init();
    adc_oneshot_unit_handle_t adc_handle = NULL;
    adc_oneshot_init(&adc_handle, channels, ADC_CHANNEL_NUM);

    wifi_sta_init(&wifi_state);

    led_init(RGB_GPIO, 255, &led_state);
    i2c_init();
    xTaskCreate(&udp_task_manager, "udp_task_manager", 4096, NULL, 1, NULL);
    xTaskCreate(&joystick_check_task, "joystick_check_task", 4096, adc_handle, 1, &joystick_check_task_handle);
    xTaskCreate(&button_check_task, "button_check_task", 4096, NULL, 1, &button_check_task_handle);
    xTaskCreate(&wifi_check_status_task, "wifi_check_status_task", 4096, &wifi_state, 1, &wifi_check_status_task_handler);
    xTaskCreate(&led_task, "led_check_task", 4096, &led_state, 1, NULL);
}