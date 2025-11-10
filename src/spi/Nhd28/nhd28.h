#include "spi/spi.h"

//Display

#define NHD28_READ_DISPLAY_ID       0x04
#define NHD28_READ_DISPLAY_STATUS   0x09
#define NHD28_READ_DISPLAY_POWER    0x0A
#define NHD28_SLEEP_IN              0x10
#define NHD28_SLEEP_OUT             0x11
#define NHD28_DISP_OFF              0x28
#define NHD28_DISP_ON               0x29
#define NHD28_CASET                 0x2h
#define NHD28_RASET                 0x2b
#define NHD28_RAMWR                 0x2c
#define NHD28_RAMRD                 0x2e


typedef enum Nhd28Status {
    NHD28_STATUS_NOT_INITIALIZED = 0U,
    NHD28_STATUS_FAILED_NULL_DEVICE,
    NHD28_STATUS_FAILED_NULL_CONFIG,
    NHD28_STATUS_FAILED_NULL_SPI_BUS,
    NHD28_STATUS_FAILED_SPI_CLOCK_SPEED_LIMITS,
    NHD28_STATUS_WAITING_SPI_BUS,
    NHD28_STATUS_FAILED_TO_ADD_TO_SPI_BUS,
    NHD28_STATUS_OK,
    NHD28_STATUS_FAILED_TO_ACQUIRE_SPI_BUS,
    NHD28_STATUS_FAILED_TO_WRITE,

} Nhd28Status;

typedef struct Nhd28SpiConfig
{      
    SpiBus *spi_bus;
    gpio_num_t cs;
    int clock_speed_hz;
} Nhd28SpiConfig;

typedef struct Nhd28Device
{   
    Nhd28Status status;
    SpiBus *spi_bus;
    spi_device_handle_t device_handle;
    uint8_t *rx_buffer;
    int rx_buf_len;
    uint8_t *tx_buffer;
    int tx_buf_len;
} Nhd28Device;



Nhd28Status nhd28_init(Nhd28SpiConfig *config, Nhd28Device *device);
Nhd28Status nhd28_write(Nhd28Device *device);
