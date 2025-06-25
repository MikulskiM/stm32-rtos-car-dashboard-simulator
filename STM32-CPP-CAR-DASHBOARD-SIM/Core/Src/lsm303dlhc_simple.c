#include <lsm303dlhc_simple.h>
#include "stm32f4xx_hal.h"

#define LSM303_ACC_ADDRESS		(0x32 >> 1)  // 7-bit address (default)
#define LSM303_CTRL_REG1_A		0x20
#define LSM303_OUT_X_L_A		0x28

void LSM303_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t ctrl1 = 0x57; // 100Hz, normal mode, all axes enabled
    HAL_I2C_Mem_Write(hi2c, LSM303_ACC_ADDRESS << 1, LSM303_CTRL_REG1_A, 1, &ctrl1, 1, HAL_MAX_DELAY);
}

void LSM303_ReadAccel(I2C_HandleTypeDef *hi2c, LSM303DLHC_accel_raw *data) {
    uint8_t buffer[6];
    // auto-increment address (set MSB)
    HAL_I2C_Mem_Read(hi2c, LSM303_ACC_ADDRESS << 1, LSM303_OUT_X_L_A | 0x80, 1, buffer, 6, 1000);

    data->x = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->y = (int16_t)(buffer[3] << 8 | buffer[2]);
    data->z = (int16_t)(buffer[5] << 8 | buffer[4]);
}
