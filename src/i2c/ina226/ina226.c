#include "ina226.h"

static char *TAG = "INA226";

esp_err_t ina226_who_i_am(uint8_t address) 
{   
    esp_err_t res;
    
    uint16_t manufacturer_id;
    res = i2c_read_device_reg16(address, INA226_REG_MANUFACTURER, &manufacturer_id);

    if (!res)
    {
        ESP_LOGE(TAG, "Failed to read manufacturer ID from adress (0x%02X)", address);
        return res;
    }

    if (manufacturer_id != 0x5449)
    { // TI manufacturer ID
        ESP_LOGE(TAG, "Invalid manufacturer ID 0x%04X from address (0x%02X)", manufacturer_id, INA226_ADDR);
        return res;
    }

    // Чтение ID чипа
    uint16_t die_id = 0;
    res = i2c_read_device_reg16(address, INA226_REG_DIE_ID, &die_id);
    if (!res)
    {
        ESP_LOGE(TAG, "Failed to read die ID from adress (0x%02X)", address);
        return res;
    }

    if (die_id != 0x2260)
    { // INA226 die ID
        ESP_LOGE(TAG, "Invalid die ID 0x%04X from address (0x%02X)", die_id, address);
        return res;
    }

    return ESP_OK;
}

esp_err_t ina226_set_calibration(uint8_t address, float shunt_resistance, float max_current_ma)
{   
    float max_current_a = max_current_ma / 1000.0f;
    float current_lsb = max_current_a / 32768.0f;

    //Calibration formula: CAL = 0.00512 / (Current_LSB × RSHUNT)
    uint16_t calibration_reg_val = (uint16_t)(0.00512f / (current_lsb * shunt_resistance));

    esp_err_t ret = i2c_write_device_reg16(address, INA226_REG_CALIBRATION, calibration_reg_val);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write calibration to INA226 (0x%02X): %s",
                 address, esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t ina226_init(uint8_t address)
{      
    esp_err_t ret;

    ret = ina226_who_i_am(address);

    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "INA226 not found");
    }

    ret = i2c_write_device_reg16(address, INA226_REG_CONFIG, INA226_CONFIG_DEFAULT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to write config to adress (0x%02X)", address);
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    uint16_t written_config;
    ret = i2c_read_device_reg16(address, INA226_REG_CONFIG, &written_config);
    if (ret == ESP_OK)
    {   
        ESP_LOGI(TAG, "Written config=0x%04X, Read config=0x%04X",
                 INA226_CONFIG_DEFAULT, written_config);
    }

    ina226_set_calibration(address, INA226_SHUNT_RESISTANCE, INA226_MAX_CURRENT_MA);

    ESP_LOGI(TAG, "Initialized successfully");

    return ESP_OK;
}

static void ina226_read_float_value(uint8_t address, uint8_t reg, float current_lsb, float *res)
{   
    int16_t raw_val;
    esp_err_t ret = i2c_read_device_reg16_sign(address, reg, &raw_val);

    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Error while reading value from INA226: %s", esp_err_to_name(ret));
    }

    *res = current_lsb * (float)raw_val;
}

void ina226_read_current(uint8_t address, float current_lsb, float *res)
{   
    ina226_read_float_value(address, INA226_REG_CURRENT, current_lsb, res); //в амперах
}

void ina226_read_bus_voltage(uint8_t address, float *res)
{   
    ina226_read_float_value(address, INA226_REG_BUS_VOLTAGE, 0.00125f, res); // в вольтах
}

void ina226_read_shunt_voltage(uint8_t address,  float *res)
{   
    ina226_read_float_value(address, INA226_REG_SHUNT_VOLTAGE, 0.0025f, res); //в мили
}

void ina226_read_power(uint8_t address, float current_lsb, float *res)
{   
    ina226_read_float_value(address, INA226_REG_POWER, 25.f * current_lsb, res); // в ватах
}
