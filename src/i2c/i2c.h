#include "driver/i2c.h"
#include "string.h"
#include "esp_err.h"
#include "esp_log.h"

void i2c_init();
void i2c_print_device_table();

esp_err_t i2c_read_device_reg(uint8_t address, uint8_t reg, uint8_t *data, size_t len);
esp_err_t i2c_read_device_reg8(uint8_t address, uint8_t reg, uint8_t *value);
esp_err_t i2c_read_device_reg8_sign(uint8_t address, uint8_t reg, int8_t *value);
esp_err_t i2c_read_device_reg16(uint8_t address, uint8_t reg, uint16_t *value);
esp_err_t i2c_read_device_reg16_sign(uint8_t address, uint8_t reg, int16_t *value);

esp_err_t i2c_write_device_reg(uint8_t address, uint8_t reg, uint8_t *data, size_t len);
esp_err_t i2c_write_device_reg8(uint8_t address, uint8_t reg, uint8_t value);
esp_err_t i2c_write_device_reg16(uint8_t address, uint8_t reg, uint16_t value);
