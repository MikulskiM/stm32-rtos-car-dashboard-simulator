#ifndef CONFIG_H
#define CONFIG_H

#include "stm32f4xx_hal.h"

#define ERROR   "[ERROR] "
#define INFO    "[INFO ] "

#define RED_LED_PIN     GPIO_PIN_14
#define LED_PORT        GPIOD

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart2;

void SystemClock_Config(void);
void Error_Handler(const char* prefix, const char* msg);
void UART_Init();

#ifdef __cplusplus
}
#endif

#endif