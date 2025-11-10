#include "i2c.h"

static char* TAG = "I2C";

void i2c_init() 
{   
    esp_err_t res;

    i2c_config_t master_cfg = 
    {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 5,
        .scl_io_num = 4,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .clk_flags = 0
    };

    i2c_param_config(I2C_NUM_0, &master_cfg);

    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    ESP_LOGI(TAG, "I2C Driver installed\r\n");
}

void i2c_print_device_table() 
{   
    printf("    ");
    for (int col = 0; col < 16; col++)
    {
        printf(" %2X ", col);
    }
    printf("\n");

    int addr = 0;
    int multiplier = 1;
    for (int row_index = 0; row_index < 8; row_index++)
    {   
        printf("0x%x0", row_index);

        for ( ; addr < 16 * multiplier; addr++)
        {   
            if ((addr >= 0 && addr <= 7) || (addr >= 120 && addr <= 128))
            {   
                printf(" .. ");
                continue;
            }

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_stop(cmd);
            if (i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS) == ESP_OK)
            {   
                printf(" ✓  ");
            } 
            else 
            {
                printf(" .. ");
            }

            i2c_cmd_link_delete(cmd);
            vTaskDelay(pdMS_TO_TICKS(5));
        }

        multiplier++;

        printf("\n");
        
    }
    
}

esp_err_t i2c_read_device_reg(uint8_t address, uint8_t reg, uint8_t *data, size_t len)
{   

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);

    i2c_master_write_byte(cmd, reg, 1);

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, 1);
    i2c_master_read(cmd, data, len , I2C_MASTER_LAST_NACK);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_read_device_reg8(uint8_t address, uint8_t reg, uint8_t *value)
{   
    int len = 1;
    uint8_t data[len];

    esp_err_t ret = i2c_read_device_reg(address, reg, data, len);
    *value = data[0];

    return ret;
}

esp_err_t i2c_read_device_reg8_sign(uint8_t address, uint8_t reg, int8_t *value)
{
    uint8_t tmp_val;
    esp_err_t ret = i2c_read_device_reg8(address, reg, &tmp_val);

    *value = (int8_t)tmp_val;

    return ret;
}

esp_err_t i2c_read_device_reg16(uint8_t address, uint8_t reg, uint16_t *value)
{   
    int len = 2;
    uint8_t data[len];

    esp_err_t ret = i2c_read_device_reg(address, reg, data, len);
    *value = (uint16_t)((data[0] << 8) | data[1]);

    return ret;
}

esp_err_t i2c_read_device_reg16_sign(uint8_t address, uint8_t reg, int16_t *value)
{
    uint16_t tmp_val;
    esp_err_t ret = i2c_read_device_reg16(address, reg, &tmp_val);

    *value = (int16_t)tmp_val;

    return ret;
}

esp_err_t i2c_write_device_reg(uint8_t address, uint8_t reg, uint8_t *data, size_t len)
{
    uint8_t msg[len + 1];
    msg[0] = reg;
    memcpy(msg + 1, data, len);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);

    i2c_master_write(cmd, msg, len + 1, 1);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));

    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_write_device_reg8(uint8_t address, uint8_t reg, uint8_t value)
{   
    uint8_t data[] = {value};
    esp_err_t ret = i2c_write_device_reg(address, reg, data, 1);

    return ret;
}

esp_err_t i2c_write_device_reg16(uint8_t address, uint8_t reg, uint16_t value)
{   
    uint8_t data[2];
    data[0] = (value >> 8) | 0xFF;
    data[1] = value | 0xFF;

    esp_err_t ret = i2c_write_device_reg(address, reg, data, 2);

    return ret;
}
