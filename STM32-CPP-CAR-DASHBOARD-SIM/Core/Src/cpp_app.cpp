/*
 * cpp_app.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <cstdio>

#include "cpp_app.hpp"
#include "ui_manager.hpp"
#include "st7735.h"
#include "fonts.h"
#include "lsm303dlhc_simple.h"
#include "tasks.hpp"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
}

App cpp_app;

osMutexId_t i2c1Mutex;	// mutex for I2C1

void App::init() {
	initDisplay();
	initI2CSensors();
	initMutexes();
	initTasks();
	initEncoder();
}

void App::run() {
	/* Start scheduler */
	osKernelStart();
	// the code won't go further. Now the control belongs to Scheduler
}

void App::initDisplay() {
	ST7735_Init();
	ST7735_InvertColors(false);

	HAL_Delay(1000);

	uiManager.render();
}

void App::initI2CSensors() {
	LSM303_Init(&hi2c1);

	// scan I2C addresses - debug purposes
	for (uint8_t addr = 0; addr < 127; addr++) {
		if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
			printf("Device found at 0x%02X\r\n", addr);
		}
	}
}

void App::initMutexes() {
	const osMutexAttr_t i2cMutexAttr = {
		.name = "i2c1Mutex"
	};
	i2c1Mutex = osMutexNew(&i2cMutexAttr);
}

void App::initTasks() {
	/* Init scheduler */
	osKernelInitialize();

	osThreadId_t uiTaskHandle;
	osThreadId_t loggerTaskHandle;

	const osThreadAttr_t uiTask_attributes = {
	  .name = "uiTask",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
	};
	const osThreadAttr_t loggerTask_attributes = {
	  .name = "loggerTask",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityNormal,
	};

	uiTaskHandle = osThreadNew(StartUiTask, NULL, &uiTask_attributes);
	loggerTaskHandle = osThreadNew(StartLoggerTask, NULL, &loggerTask_attributes);
}

void App::initEncoder() {
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);	// start the timer as an encoder
}
