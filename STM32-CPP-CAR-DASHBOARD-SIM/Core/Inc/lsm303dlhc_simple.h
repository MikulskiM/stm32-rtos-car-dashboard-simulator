#ifndef LSM303DLHC_H
#define LSM303DLHC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h" // I2C_HandleTypeDef

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} LSM303DLHC_accel_raw;

void LSM303_Init(I2C_HandleTypeDef *hi2c);
void LSM303_ReadAccel(I2C_HandleTypeDef *hi2c, LSM303DLHC_accel_raw *data);

#ifdef __cplusplus
}
#endif

#endif /* LSM303DLHC_H */
