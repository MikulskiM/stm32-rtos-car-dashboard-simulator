/*
 * cpp_app.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <cpp_app.h>
#include "st7735.h"
#include "fonts.h"

#include "cmsis_os.h"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
}

App app;

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
	  ST7735_FillScreen(ST7735_BLACK);
	  ST7735_WriteString(10, 10, "BLACK", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	  osDelay(1000);
	  ST7735_FillScreen(ST7735_BLUE);
	  ST7735_WriteString(10, 10, "BLUE", Font_7x10, ST7735_BLACK, ST7735_BLUE);
	  osDelay(1000);
	  ST7735_FillScreen(ST7735_RED);
	  ST7735_WriteString(10, 10, "RED", Font_7x10, ST7735_BLACK, ST7735_RED);
	  osDelay(1000);
	  ST7735_FillScreen(ST7735_GREEN);
	  ST7735_WriteString(10, 10, "GREEN", Font_7x10, ST7735_BLACK, ST7735_GREEN);
	  osDelay(1000);
	  ST7735_FillScreen(ST7735_YELLOW);
	  ST7735_WriteString(10, 10, "YELLOW", Font_7x10, ST7735_BLACK, ST7735_YELLOW);
	  osDelay(1000);
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

void App::init() {
	ST7735_Unselect();

	HAL_Delay(100);
	ST7735_Init();
	ST7735_InvertColors(false);

	ST7735_FillScreen(ST7735_RED);
	HAL_Delay(1000);

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

