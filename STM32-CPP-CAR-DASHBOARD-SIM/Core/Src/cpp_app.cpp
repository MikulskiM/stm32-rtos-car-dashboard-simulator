/*
 * cpp_app.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <cpp_app.h>
#include "st7735.h"
#include "fonts.h"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
}

App app;

void CppApp() {
	app.init();
	app.run();
}

void App::init() {
	ST7735_Unselect();

	HAL_Delay(100);
	ST7735_Init();
	ST7735_InvertColors(false);

	ST7735_FillScreen(ST7735_RED);
	HAL_Delay(1000);
}

void App::run() {
	while (1) {
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_WriteString(10, 10, "BLACK", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		HAL_Delay(1000);
		ST7735_FillScreen(ST7735_BLUE);
		ST7735_WriteString(10, 10, "BLUE", Font_7x10, ST7735_BLACK, ST7735_BLUE);
		HAL_Delay(1000);
		ST7735_FillScreen(ST7735_RED);
		ST7735_WriteString(10, 10, "RED", Font_7x10, ST7735_BLACK, ST7735_RED);
		HAL_Delay(1000);
		ST7735_FillScreen(ST7735_GREEN);
		ST7735_WriteString(10, 10, "GREEN", Font_7x10, ST7735_BLACK, ST7735_GREEN);
		HAL_Delay(1000);
		ST7735_FillScreen(ST7735_YELLOW);
		ST7735_WriteString(10, 10, "YELLOW", Font_7x10, ST7735_BLACK, ST7735_YELLOW);
		HAL_Delay(1000);
	}
}

App::App() {
	// TODO Auto-generated constructor stub

}

App::~App() {
	// TODO Auto-generated destructor stub
}

