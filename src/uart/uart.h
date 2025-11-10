#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "string.h"

#define UART_BAUD_RATE 9600
#define BUF_SIZE 1024
#define UART_NUM UART_NUM_1
#define UART_TX_PIN GPIO_NUM_38
#define UART_RX_PIN GPIO_NUM_37

#define SENSOR_STOP "sensorStop"
#define SET_RANGE "setRange 0.6 3"
#define SAVE_CFG "saveConfig"
#define SENSOR_START "sensorStart"
#define GET_SOFTWARE_VERSION "getSWV"
#define DISTANCE_MEASUREMENT_CMD "setRunApp 1"

#define HI_LINK_SET_SPEED_ANGLE_SENSIVITY "43 46 01 03 0a 05 0d 0a"
#define HI_LINK_SET_TARGET_SPEED_UNIT "43 46 02 01 02 00 0d 0a"
#define HI_LINK_READ_PARAMS "43 46 07 00 00 00 00 00 00 00 00 00 00"

void uart_init();
void uart_send();
void uart_read();