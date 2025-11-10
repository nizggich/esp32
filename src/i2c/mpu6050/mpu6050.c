#include "mpu6050.h"

static char *TAG = "MPU6050";
static int16_t offsets[6];

esp_err_t MPU6050_who_i_am()
{
    uint8_t who_am_i;

    esp_err_t ret = i2c_read_device_reg8(MPU_6050_ADDR, MPU_6050_WHO_I_AM, &who_am_i);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C problem. Сouldn't get WHO_I_AM value: %s", esp_err_to_name(ret));
        return ret;
    }

    if (who_am_i == MPU_6050_WHO_I_AM) {
        ESP_LOGI(TAG, "MPU-6050 found");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "MPU-6050 not found! Current address is: 0x%02X", who_am_i); 
        return ret;
    }
}

esp_err_t MPU6050_wake_up() 
{
    esp_err_t ret = i2c_write_device_reg8(MPU_6050_ADDR, MPU6050_PWR_MGMT_1, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C problem. Сouldn't wake up the sensor: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Sensor is waking up");
    return ESP_OK;
}

esp_err_t MPU6050_configure()
{   
    esp_err_t ret;

    uint8_t data = 0;

    ret = i2c_write_device_reg8(MPU_6050_ADDR, MPU6050_ACCEL_CONFIG, data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C Problem. Сouldn't set accelerometer scale: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = i2c_write_device_reg8(MPU_6050_ADDR, MPU6050_GYRO_CONFIG, data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C Problem. Сouldn't set gyroscope scale: %s", esp_err_to_name(ret));
        return ret;
    }

    data = 0x03;
    ret = i2c_write_device_reg8(MPU_6050_ADDR, MPU6050_CONFIG, data);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C Problem. Сouldn't set DLPF value: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t MPU6050_calibrate()
{   
    int32_t ax_sum = 0, ay_sum = 0, az_sum = 0;
    int32_t gx_sum = 0, gy_sum = 0, gz_sum = 0;

    int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

    for (int i = 0; i < 100; i++)
    {   
        if (MPU6050_get_raw_data(&ax, &ay, &az, &gx, &gy, &gz) == ESP_OK)
        {
            ax_sum += ax;
            ay_sum += ay;
            az_sum += az;

            gx_sum += gx;
            gy_sum += gy;
            gz_sum += gz;

            vTaskDelay(pdMS_TO_TICKS(10));
        }

        //return ESP_FAIL;
    }

    offsets[0] = (int16_t)(ax_sum / 100.0f);
    offsets[1] = (int16_t)(ay_sum / 100.0f);
    offsets[2] = (int16_t)(az_sum / 100.0f);

    offsets[3] = (int16_t)(gx_sum / 100.0f);
    offsets[4] = (int16_t)(gy_sum / 100.0f); 
    offsets[5] = (int16_t)(gz_sum / 100.0f);

    return ESP_OK;
    
}


esp_err_t MPU6050_get_raw_data(int16_t *ax, int16_t *ay, int16_t *az,
                               int16_t *gx, int16_t *gy, int16_t *gz)
{   
    uint8_t data[14];
    
    esp_err_t ret = i2c_read_device_reg(MPU_6050_ADDR, MPU6050_ACCEL_XOUT_H, data, 14);

    if (ret != ESP_OK) 
    {   
        ESP_LOGE(TAG, 
                "Error while reading all data\n Reason: %s", 
                 esp_err_to_name(ret));
        
        *ax = *ay = *az = *gx = *gy = *gz = 0;
        
        return ret; 
    } 

    *ax = (int16_t)(data[0] << 8) | data[1];
    *ay = (int16_t)(data[2] << 8) | data[3];
    *az = (int16_t)(data[4] << 8) | data[5];

    //data[6],data[7] - temperature of sensor

    *gx = (int16_t)(data[8] << 8)  | data[9];
    *gy = (int16_t)(data[10] << 8) | data[11];
    *gz = (int16_t)(data[12] << 8) | data[13];

    return ESP_OK;
}

esp_err_t MPU6050_init()
{
    if (MPU6050_who_i_am() != ESP_OK || !MPU6050_wake_up() != ESP_OK || 
        !MPU6050_configure() != ESP_OK || !MPU6050_calibrate() != ESP_OK)
    {   
        ESP_LOGE(TAG, "Сouldn't init sensor");
        return ESP_FAIL;
    }

    return ESP_OK;
}

void MPU6050_get_data(float *ax, float *ay, float *az,
                      float *gx, float *gy, float *gz)
{   
    int16_t ax_t, ay_t, az_t, gx_t, gy_t, gz_t;

    MPU6050_get_raw_data(&ax_t, &ay_t, &az_t, &gx_t, &gy_t, &gz_t);

    *ax = (ax_t - offsets[0]) / 16384.0f; 
    *ay = (ay_t - offsets[1]) / 16384.0f; 
    *az = (az_t - offsets[2]) / 16384.0f; 

    *gx = (gx_t - offsets[3]) / 131.0f;
    *gy = (gy_t - offsets[4]) / 131.0f;
    *gz = (gz_t - offsets[5]) / 131.0f;

}
