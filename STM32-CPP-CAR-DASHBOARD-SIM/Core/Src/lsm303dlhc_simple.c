#include <lsm303dlhc_simple.h>
#include "stm32f4xx_hal.h"

#include <math.h>	// atan2f()
#include <stdio.h>

#define LSM303_ACC_ADDRESS		(0x32 >> 1)  // 7-bit address (accelerometer)
#define LSM303_CTRL_REG1_A		0x20
#define LSM303_OUT_X_L_A		0x28

#define LSM303_MAG_ADDRESS      (0x3C >> 1)  // 7-bit address (magnetometer)
#define LSM303_CRA_REG_M        0x00
#define LSM303_CRB_REG_M        0x01
#define LSM303_MR_REG_M         0x02
#define LSM303_OUT_X_H_M        0x03

void LSM303_Init(I2C_HandleTypeDef *hi2c) {
	HAL_StatusTypeDef status;

	// ACCELEROMETER
    uint8_t ctrl1 = 0x57;	// 100Hz, normal mode, all axes enabled
    status = HAL_I2C_Mem_Write(hi2c, LSM303_ACC_ADDRESS << 1, LSM303_CTRL_REG1_A, 1, &ctrl1, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
        printf("LSM303 INIT FAILED!\r\n");
    }

    uint8_t ctrl4 = 0x08;	// HR (high resolution) = 1, FS = +-2g, full 16-bit data
    status = HAL_I2C_Mem_Write(hi2c, LSM303_ACC_ADDRESS << 1, 0x23, 1, &ctrl4, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
		printf("LSM303 INIT FAILED!\r\n");
	}

    // MAGNETOMETER
    uint8_t cra = 0x14;		// Temperature disable, 30Hz output rate
    // TODO: ADD TEMPERATURE MODE in the future
    status = HAL_I2C_Mem_Write(hi2c, LSM303_MAG_ADDRESS << 1, LSM303_CRA_REG_M, 1, &cra, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
		printf("LSM303 INIT FAILED!\r\n");
	}

    uint8_t crb = 0x20;		// Gain = +-1.3 gauss
    status = HAL_I2C_Mem_Write(hi2c, LSM303_MAG_ADDRESS << 1, LSM303_CRB_REG_M, 1, &crb, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
		printf("LSM303 INIT FAILED!\r\n");
	}

    uint8_t mr = 0x00;		// Continuous conversion mode
    status = HAL_I2C_Mem_Write(hi2c, LSM303_MAG_ADDRESS << 1, LSM303_MR_REG_M, 1, &mr, 1, HAL_MAX_DELAY);
    if (status != HAL_OK) {
		printf("LSM303 INIT FAILED!\r\n");
	}

}

void LSM303_ReadAccel(I2C_HandleTypeDef *hi2c, LSM303DLHC_accel_raw *data) {
    uint8_t buffer[6];
    // auto-increment address (set MSB)
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, LSM303_ACC_ADDRESS << 1, LSM303_OUT_X_L_A | 0x80, 1, buffer, 6, 1000);
    if (status != HAL_OK) {
        printf("LSM303 READ FAIL (I2C)!\r\n");
    }

    data->x = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->y = (int16_t)(buffer[3] << 8 | buffer[2]);
    data->z = (int16_t)(buffer[5] << 8 | buffer[4]);
}

void LSM303_ReadMag(I2C_HandleTypeDef *hi2c, LSM303DLHC_mag_raw *data) {
    uint8_t buffer[6];

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, LSM303_MAG_ADDRESS << 1, LSM303_OUT_X_H_M, 1, buffer, 6, 1000);
    if (status != HAL_OK) {
        printf("LSM303 MAG READ FAIL!\r\n");
    }

    // Weird order X, Z, Y
    data->x = (int16_t)(buffer[0] << 8 | buffer[1]);
    data->z = (int16_t)(buffer[2] << 8 | buffer[3]);
    data->y = (int16_t)(buffer[4] << 8 | buffer[5]);
}

int LSM303_HeadingDegrees(const LSM303DLHC_mag_raw* mag) {
    float heading = atan2f((float)mag->y, (float)mag->x);
    if (heading < 0) heading += 2.0f * (float)M_PI;

    return (int)(heading * (180.0f / (float)M_PI)); // degrees out
}

int LSM303_ReadHeadingDegrees(I2C_HandleTypeDef *hi2c) {
	LSM303DLHC_mag_raw data;
	LSM303_ReadMag(hi2c, &data);
	return LSM303_HeadingDegrees(&data);
}
