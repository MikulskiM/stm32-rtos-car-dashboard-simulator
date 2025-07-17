/*
 * ui_manager.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Praca
 */

#include <ui_manager.hpp>
#include "st7735.h"
#include "fonts.h"
#include <cstdio>

#include "cpp_app.hpp"			// I2C mutex
#include "lsm303dlhc_simple.h"
#include "main.h"

#define UI_TEXT_X			10
#define UI_TEXT_X_SMALL		5
#define UI_TEXT_Y_HEADER	10
#define UI_TEXT_Y_LINE1		30
#define UI_TEXT_Y_LINE2		40
#define UI_TEXT_Y_LINE3		50

UIManager uiManager;

void UIManager::rotateRight() {
	if (mode == UI_MODE_MENU) {
		currentScreen = nextScreen();
		render();
	} else {
		exitActiveMode();
		currentScreen = nextScreen();
		render();
	}
}

void UIManager::rotateLeft() {

	if (mode == UI_MODE_MENU) {
		currentScreen = previousScreen();
		render();
	} else {
		exitActiveMode();
		currentScreen = previousScreen();
		render();
	}
}

void UIManager::render() {
    ST7735_FillScreen(ST7735_BLACK);
    switch (currentScreen) {
        case SCREEN_CAR_STATUS:
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "CAR STATUS", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_RADIO:
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "RADIO", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_SETTINGS:
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "SETTINGS", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_LED_CONTROL:
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "LED CTRL", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        default:
        	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "wrong currentScreen", Font_11x18, ST7735_WHITE, ST7735_BLACK);
			break;
    }
}

void UIManager::pressButton() {
	if (mode == UI_MODE_MENU) {
		mode = UI_MODE_ACTIVE;

		switch (currentScreen) {
			case SCREEN_CAR_STATUS:		displayCarStatus();		break;
			case SCREEN_RADIO:			displayRadio();			break;
			case SCREEN_SETTINGS:		displaySettings();		break;
			case SCREEN_LED_CONTROL:	displayLedControl();	break;
			default: break;
		}
	}
}

void UIManager::displayDebugString(const char* msg) {
	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, msg, Font_11x18, ST7735_WHITE, ST7735_RED);
}

void UIManager::displayCarStatus() {
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "CAR STATUS", Font_7x10, ST7735_WHITE, ST7735_BLACK);

	if (osMutexAcquire(i2c1Mutex, 100) == osOK) {
		LSM303DLHC_accel_raw acc_data;
		LSM303_ReadAccel(&hi2c1, &acc_data);
		osMutexRelease(i2c1Mutex);

		char buf[32];
		sprintf(buf, "X: %d", acc_data.x);
		ST7735_WriteString(UI_TEXT_X_SMALL, UI_TEXT_Y_LINE1, buf, Font_7x10, ST7735_WHITE, ST7735_BLACK);
		sprintf(buf, "Y: %d", acc_data.y);
		ST7735_WriteString(UI_TEXT_X_SMALL, UI_TEXT_Y_LINE2, buf, Font_7x10, ST7735_WHITE, ST7735_BLACK);
		sprintf(buf, "Z: %d", acc_data.z);
		ST7735_WriteString(UI_TEXT_X_SMALL, UI_TEXT_Y_LINE3, buf, Font_7x10, ST7735_WHITE, ST7735_BLACK);
	} else {
		ST7735_WriteString(UI_TEXT_X_SMALL, UI_TEXT_Y_LINE3, "I2C TIMEOUT", Font_7x10, ST7735_WHITE, ST7735_RED);
	}
}

void UIManager::displayRadio() {
	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, "radio status", Font_7x10, ST7735_WHITE, ST7735_RED);
}

void UIManager::displaySettings() {
	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, "change settings", Font_7x10, ST7735_WHITE, ST7735_RED);
}

void UIManager::displayLedControl() {
	ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, "LED control panel", Font_7x10, ST7735_WHITE, ST7735_RED);
}

bool UIManager::isInActiveMode() {
	return mode == UI_MODE_ACTIVE;
}

MenuScreen UIManager::getCurrentScreen() {
	return currentScreen;
}

MenuScreen UIManager::nextScreen() {
	return (MenuScreen)((currentScreen + 1) % SCREEN_COUNT);
}

MenuScreen UIManager::previousScreen() {
	return (MenuScreen)((currentScreen + SCREEN_COUNT - 1) % SCREEN_COUNT);
}

void UIManager::exitActiveMode() {
	mode = UI_MODE_MENU;
}

