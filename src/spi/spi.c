#include "spi.h"

static const char *TAG = "SPI";

SpiBusStatus spi_init(SpiBus *bus, SpiBusConfig *config)
{
    esp_err_t ret;

    if (config == NULL) 
    {
        return SPI_BUS_STATUS_FAILED_NULL_BUS;
    }

    const spi_bus_config_t bus_cfg = {
        .sclk_io_num = config->sclk_gpio,
        .mosi_io_num = config->mosi_gpio,
        .miso_io_num = config->miso_gpio,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = ESP_INTR_FLAG_LEVEL3 
    };

    ret = spi_bus_initialize(config->host, &bus_cfg, SPI_DMA_CH_AUTO);

    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Error while initi SPI BUS: %s", esp_err_to_name(ret));
        bus->status = SPI_BUS_STATUS_FAILED_TO_INIT;
        return bus->status; 
    }

    bus->host = config->host;
    bus->status = SPI_BUS_STATUS_OK;
    return bus->status;
   
}


bool spi_bus_is_ready(SpiBus * bus)
{
    return bus->status == SPI_BUS_STATUS_OK;
}
