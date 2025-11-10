#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

#define SPI_MISO_GPIO GPIO_NUM_10
#define SPI_MOSI_GPIO GPIO_NUM_9
#define SPI_CS_GPIO   GPIO_NUM_48
#define SPI_SCK_GPIO  GPIO_NUM_3
#define SPI_RESET     GPIO_NUM_17
#define SPI_DC        GPIO_NUM_18

typedef enum
{
    SPI_BUS_STATUS_NOT_INITIALIZED = 0U,
    SPI_BUS_STATUS_FAILED_NULL_BUS,
    SPI_BUS_STATUS_FAILED_TO_INIT,
    SPI_BUS_STATUS_OK,
} SpiBusStatus;

typedef struct {
    spi_host_device_t host;
    gpio_num_t miso_gpio;
    gpio_num_t mosi_gpio;
    gpio_num_t sclk_gpio;
} SpiBusConfig;

typedef struct
{
    SpiBusStatus status;
    spi_host_device_t host;
} SpiBus;

SpiBusStatus spi_init(SpiBus *bus, SpiBusConfig *config);

bool spi_bus_is_ready(SpiBus * bus);
