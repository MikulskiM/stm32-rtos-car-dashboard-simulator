/*
 * cpp_app.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <cpp_app.hpp>
#include <ui_manager.hpp>
#include "st7735.h"
#include "fonts.h"
#include "lsm303dlhc_simple.h"
#include <cstdio>

#include "cmsis_os.h"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
}

App app;
UIManager uiManager;

void CppApp() {
	app.init();
	app.run();
}

void StartUiTask(void *argument)
{
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		int32_t pos = __HAL_TIM_GET_COUNTER(&htim1);	// return only + values (1-11 for right, 32757-32767 for negative
		if (pos >1 && pos < 16350 ) {
			if (uiManager.isInActiveMode()) {
				uiManager.exitActiveMode();
				uiManager.render();
			} else {
				uiManager.rotateRight();
			}
			__HAL_TIM_SET_COUNTER(&htim1, 0);
		} else if (pos > 16350) {
			if (uiManager.isInActiveMode()) {
				uiManager.exitActiveMode();
				uiManager.render();
			} else {
				uiManager.rotateLeft();
			}
			__HAL_TIM_SET_COUNTER(&htim1, 0);
		}

		if (uiManager.isInActiveMode()) {
		    switch (uiManager.getCurrentScreen()) {
		        case SCREEN_CAR_STATUS:
		            uiManager.displayCarStatus();
		            break;
		        case SCREEN_RADIO:
		            uiManager.displayRadio();
		            break;
		        case SCREEN_SETTINGS:
		            uiManager.displaySettings();
		            break;
		        case SCREEN_LED_CONTROL:
		            uiManager.displayLedControl();
		            break;
		        default:
		            break;
		    }
		}


		osDelay(10);	// very light polling

		/* not a classic polling with while(1)
		* it's a RTOS task that sleeps most of the time
		* and looks only once in a while
		*/

	}
	/* USER CODE END 5 */
}

void StartLoggerTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

// WRAPPER FUNCCTIONS FOR UI MANAGER TO USE THEM IN MAIN.C
extern "C" void UI_EncoderRotatedLeft() {
    uiManager.rotateLeft();
}

extern "C" void UI_EncoderRotatedRight() {
    uiManager.rotateRight();
}

extern "C" void UI_EncoderButtonPressed() {
    uiManager.pressButton();
}

extern "C" void UI_DisplayDebugString(const char* msg) {
    uiManager.displayDebugString(msg);
}

void App::init() {
	ST7735_Unselect();

	HAL_Delay(100);
	ST7735_Init();
	ST7735_InvertColors(false);

	ST7735_FillScreen(ST7735_RED);
	HAL_Delay(1000);

	uiManager.render();

	for (uint8_t addr = 0; addr < 127; addr++) {
	    if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
	        printf("Device found at 0x%02X\r\n", addr);
	    }
	}

	/* Init LSM303DLHC */
	LSM303_Init(&hi2c1);

	const osMutexAttr_t i2cMutexAttr = {
	    .name = "i2c1Mutex"
	};
	i2c1Mutex = osMutexNew(&i2cMutexAttr);

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

	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);	// start the timer as an encoder
}

void App::run() {

	/* Start scheduler */
	osKernelStart();
	// the code won't go further. Now the control belongs to Scheduler

}

App::App() {
	// TODO Auto-generated constructor stub

}

App::~App() {
	// TODO Auto-generated destructor stub
}

