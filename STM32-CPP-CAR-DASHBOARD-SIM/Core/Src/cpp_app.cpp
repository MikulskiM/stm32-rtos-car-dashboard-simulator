/*
 * cpp_app.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <cstdio>

#include "cpp_app.hpp"
#include "st7735.h"
#include "fonts.h"
#include "lsm303dlhc_simple.h"
#include "tasks.hpp"
#include "queues.hpp"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
}

App cpp_app;

void App::init() {
	initDisplay();
	initI2CSensors();
	initTasks();
	initQueues();
	initEncoder();
}

void App::run() {
	printf("osKernelStart() takes control from now on\n");
	/* Start scheduler */
	osKernelStart();
	// the code won't go further. Now the control belongs to Scheduler
}

void App::initDisplay() {
	printf("initiating display...\n");

	ST7735_Init();
	ST7735_InvertColors(false);

	HAL_Delay(1000);
}

void App::initI2CSensors() {
	printf("initiating I2C sensors...\n");

	LSM303_Init(&hi2c1);

	// scan I2C addresses - debug purposes
	for (uint8_t addr = 0; addr < 127; addr++) {
		if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
			printf("Device found at 0x%02X\r\n", addr);
		}
	}
}

void App::initTasks() {
	printf("initiating tasks...\n");

	/* Init scheduler */
	osKernelInitialize();

	const osThreadAttr_t displayTask_attributes = {
	  .name = "displayTask",
	  .stack_size = STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	};
	const osThreadAttr_t managerTask_attributes = {
	  .name = "managerTask",
	  .stack_size = STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	};
	const osThreadAttr_t loggerTask_attributes = {
	  .name = "loggerTask",
	  .stack_size = STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	};
	const osThreadAttr_t encoderTask_attributes = {
	  .name = "encoderTask",
	  .stack_size = STACK_SIZE,
	  .priority = (osPriority_t) osPriorityNormal,
	};

	osThreadNew(StartDisplayTask, NULL, &displayTask_attributes);
	osThreadNew(StartManagerTask, NULL, &managerTask_attributes);
	osThreadNew(StartLoggerTask, NULL, &loggerTask_attributes);
	osThreadNew(StartEncoderTask, NULL, &encoderTask_attributes);
}

void App::initEncoder() {
	printf("initiating encoder...\n");

	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);	// start the timer as an encoder
}

void App::initQueues() {
	printf("initiating queues...\n");

	const osMessageQueueAttr_t loggerQueue_attributes = {
		.name = "loggerQueue"
	};
	const osMessageQueueAttr_t displayQueue_attributes = {
		.name = "displayQueue"
	};
	const osMessageQueueAttr_t encoderQueue_attributes = {
		.name = "encoderQueue"
	};

	displayQueue = osMessageQueueNew(SIXTEEN_MESSAGES, sizeof(uint32_t), &displayQueue_attributes);
	loggerQueue = osMessageQueueNew(SIXTEEN_MESSAGES, sizeof(LogEvent), &loggerQueue_attributes);
	encoderQueue = osMessageQueueNew(EIGHT_MESSAGES, sizeof(EncoderCommand), &loggerQueue_attributes);

	if (loggerQueue == NULL) {
		printf("ERROR: loggerQueue not initialized!\r\n");
	}
	if (displayQueue == NULL) {
		printf("ERROR: displayQueue not initialized!\r\n");
	}
	if (encoderQueue == NULL) {
		printf("ERROR: encoderQueue not initialized!\r\n");
	}
}
