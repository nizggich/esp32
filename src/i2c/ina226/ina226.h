#include "i2c/i2c.h"
#include "stdint.h"

 //вот это нужно будет вынести для конфигурации
 //потому что есть пины на плате, с помощью которых можно менять адресс
 //либо написать код, который проходится по всем возможным адресам
#define INA226_ADDR 0x40

// INA226 Register Addresses
#define INA226_REG_CONFIG 0x00
#define INA226_REG_SHUNT_VOLTAGE 0x01
#define INA226_REG_BUS_VOLTAGE 0x02
#define INA226_REG_POWER 0x03
#define INA226_REG_CURRENT 0x04
#define INA226_REG_CALIBRATION 0x05
#define INA226_REG_MASK_ENABLE 0x06
#define INA226_REG_ALERT_LIMIT 0x07
#define INA226_REG_MANUFACTURER 0xFE
#define INA226_REG_DIE_ID 0xFF

// Сопротивление шунтов (в Омах)
#define INA226_SHUNT_RESISTANCE 0.02f // Реальное сопротивление шунта 0.02Ω
// Максимальные токи для калибровки (в мА)
#define INA226_MAX_CURRENT_MA 3000.0f

// INA226 Configuration Register Values
#define INA226_CONFIG_AVG_1 (0x0000 << 9)    // 1 sample average
#define INA226_CONFIG_AVG_4 (0x0001 << 9)    // 4 samples average
#define INA226_CONFIG_AVG_16 (0x0002 << 9)   // 16 samples average
#define INA226_CONFIG_AVG_64 (0x0003 << 9)   // 64 samples average
#define INA226_CONFIG_AVG_128 (0x0004 << 9)  // 128 samples average
#define INA226_CONFIG_AVG_256 (0x0005 << 9)  // 256 samples average
#define INA226_CONFIG_AVG_512 (0x0006 << 9)  // 512 samples average
#define INA226_CONFIG_AVG_1024 (0x0007 << 9) // 1024 samples average

#define INA226_CONFIG_BUS_VOLTAGE_140US (0x0000 << 6)  // 140us conversion time
#define INA226_CONFIG_BUS_VOLTAGE_204US (0x0001 << 6)  // 204us conversion time
#define INA226_CONFIG_BUS_VOLTAGE_332US (0x0002 << 6)  // 332us conversion time
#define INA226_CONFIG_BUS_VOLTAGE_588US (0x0003 << 6)  // 588us conversion time
#define INA226_CONFIG_BUS_VOLTAGE_1100US (0x0004 << 6) // 1.1ms conversion time
#define INA226_CONFIG_BUS_VOLTAGE_2116US (0x0005 << 6) // 2.116ms conversion time
#define INA226_CONFIG_BUS_VOLTAGE_4156US (0x0006 << 6) // 4.156ms conversion time
#define INA226_CONFIG_BUS_VOLTAGE_8244US (0x0007 << 6) // 8.244ms conversion time

#define INA226_CONFIG_SHUNT_VOLTAGE_140US (0x0000 << 3)  // 140us conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_204US (0x0001 << 3)  // 204us conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_332US (0x0002 << 3)  // 332us conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_588US (0x0003 << 3)  // 588us conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_1100US (0x0004 << 3) // 1.1ms conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_2116US (0x0005 << 3) // 2.116ms conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_4156US (0x0006 << 3) // 4.156ms conversion time
#define INA226_CONFIG_SHUNT_VOLTAGE_8244US (0x0007 << 3) // 8.244ms conversion time

#define INA226_CONFIG_MODE_POWER_DOWN (0x0000 << 0)     // Power down
#define INA226_CONFIG_MODE_SHUNT_TRIG (0x0001 << 0)     // Shunt voltage, triggered
#define INA226_CONFIG_MODE_BUS_TRIG (0x0002 << 0)       // Bus voltage, triggered
#define INA226_CONFIG_MODE_SHUNT_BUS_TRIG (0x0003 << 0) // Shunt and bus, triggered
#define INA226_CONFIG_MODE_POWER_DOWN2 (0x0004 << 0)    // Power down
#define INA226_CONFIG_MODE_SHUNT_CONT (0x0005 << 0)     // Shunt voltage, continuous
#define INA226_CONFIG_MODE_BUS_CONT (0x0006 << 0)       // Bus voltage, continuous
#define INA226_CONFIG_MODE_SHUNT_BUS_CONT (0x0007 << 0) // Shunt and bus, continuous

// Default configuration согласно даташиту INA226 для стабильности
// Бит 14 = 1 обязателен для корректной работы (Power-On Reset значение: 0x4127)
#define INA226_CONFIG_DEFAULT                                                                      \
    (INA226_CONFIG_AVG_16 | INA226_CONFIG_BUS_VOLTAGE_1100US |                                     \
     INA226_CONFIG_SHUNT_VOLTAGE_1100US | INA226_CONFIG_MODE_SHUNT_BUS_CONT |                      \
     0x4000) // Бит 14 = 1 (обязательный для стабильности)



esp_err_t ina226_who_i_am(uint8_t address);
esp_err_t ina226_init(uint8_t address);
esp_err_t ina226_set_calibration(uint8_t address, float shunt_resistance, float max_current_ma);

void ina226_read_current(uint8_t address, float current_lsb, float *res);
void ina226_read_bus_voltage(uint8_t address, float *res);
void ina226_read_shunt_voltage(uint8_t address,  float *res);
void ina226_read_power(uint8_t address, float current_lsb, float *res);

