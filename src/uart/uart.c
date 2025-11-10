#include "uart.h"

static const char *TAG = "UART";

static void uart_init()
{
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE, 0, 0, NULL, 0));
    ESP_LOGI(TAG, "UART%d инициализирован: TX=%d, RX=%d", UART_NUM, UART_TX_PIN, UART_RX_PIN);
}

int uart_send(const char *data, int len)
{   
    int writen_data_len = uart_write_bytes(UART_NUM, data, len);
    if (len > 0)
    {
        ESP_LOGI(TAG, "Command sent: %s, len: %d", data, writen_data_len);
        return len;
    }

    ESP_LOGI(TAG, "Command not sent");
    return len;
}

void uart_read(char *data, int len) 
{
    int read_data_len = uart_read_bytes(UART_NUM, data, len, pdMS_TO_TICKS(500));

    ESP_LOGI(TAG, "Data: %s", data);
    
    printf("\r\n");
    printf("\r\n");
}