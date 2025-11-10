#include "nhd28.h"

static const char *TAG = "NHD28";

Nhd28Status nhd28_init(Nhd28SpiConfig *config, Nhd28Device *device)
{      
    if (config = NULL)
    {
        return NHD28_STATUS_FAILED_NULL_CONFIG;
    }

    if (config->spi_bus = NULL)
    {
        return NHD28_STATUS_FAILED_NULL_SPI_BUS;
    }

    esp_err_t ret;

    spi_device_interface_config_t device_cfg = {
        .address_bits = 0,
        .command_bits = 0,  
        .mode = 0,
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .spics_io_num = config->cs,
        .clock_speed_hz = config->clock_speed_hz,
        .queue_size = 1 //подумай на размерами очереди
    };

    spi_device_handle_t device_handle = NULL;

    ret = spi_bus_add_device(config->spi_bus->host, &device_cfg, &device_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Error while adding device on SPI BUS: %s", esp_err_to_name(ret));
        device->status = NHD28_STATUS_FAILED_TO_ADD_TO_SPI_BUS;
        return device->status;
    }

    device->device_handle = device_handle;
    device->spi_bus = config->spi_bus;
    device->status = SPI_BUS_STATUS_OK;
}

Nhd28Status nhd28_write(Nhd28Device *device)
{
    if (device == NULL)
    {
        return NHD28_STATUS_FAILED_NULL_DEVICE;
    }

    if (device != NHD28_STATUS_OK)
    {
        return device->status;
    }

    esp_err_t ret;

    ret = spi_device_acquire_bus(device, portMAX_DELAY);

    if (ret != ESP_OK)
    {
        device->status = NHD28_STATUS_FAILED_TO_ACQUIRE_SPI_BUS;
        return device->status;
    }

    
    spi_transaction_t tr = {
        .tx_buffer = device->tx_buffer,
        .rx_buffer = device->rx_buffer,
        .length = device->tx_buf_len + device->rx_buf_len,
    };

    ret = spi_device_polling_transmit(device->device_handle, &tr);

    if (ret != ESP_OK)
    {
        device->status = NHD28_STATUS_FAILED_TO_WRITE;
        spi_device_release_bus(device->device_handle);
        return device->status;
    }

    spi_device_release_bus(device->device_handle);
    device->status = NHD28_STATUS_OK;
    return device->status;
}