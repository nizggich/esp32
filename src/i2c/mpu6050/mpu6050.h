#include "i2c/i2c.h"
#include "stdint.h"

#define MPU_6050_ADDR     0x68

#define MPU_6050_WHO_I_AM 0x75

#define MPU6050_PWR_MGMT_1  0x6B

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_TEMP 0x41

#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_CONFIG       0x1A

esp_err_t MPU6050_init();

esp_err_t MPU6050_who_i_am();
esp_err_t MPU6050_wake_up();
esp_err_t MPU6050_configure();
esp_err_t MPU6050_calibrate();

esp_err_t MPU6050_get_raw_data(int16_t *ax, int16_t *ay, int16_t *az,
                          int16_t *gx, int16_t *gy, int16_t *gz);
void MPU6050_get_data(float *ax, float *ay, float *az,
                      float *gx, float *gy, float *gz);



