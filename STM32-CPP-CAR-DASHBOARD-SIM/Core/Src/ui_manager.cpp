/*
 * ui_manager.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Praca
 */

#include "ui_manager.h"
#include "st7735.h"
#include "fonts.h"

UIManager::UIManager() {
	// TODO Auto-generated constructor stub

}

UIManager::~UIManager() {
	// TODO Auto-generated destructor stub
}

void UIManager::rotateRight() {
    currentScreen = (MenuScreen)((currentScreen + 1) % SCREEN_COUNT);
    render();
}

void UIManager::rotateLeft() {
    currentScreen = (MenuScreen)((currentScreen + SCREEN_COUNT- 1) % SCREEN_COUNT);
    render();
}

void UIManager::render() {
    ST7735_FillScreen(ST7735_BLACK);
    switch (currentScreen) {
        case SCREEN_CAR_STATUS:
            ST7735_WriteString(10, 10, "CAR STATUS", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_RADIO:
            ST7735_WriteString(10, 10, "RADIO", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_SETTINGS:
            ST7735_WriteString(10, 10, "SETTINGS", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
        case SCREEN_LED_CONTROL:
            ST7735_WriteString(10, 10, "LED CTRL", Font_11x18, ST7735_WHITE, ST7735_BLACK);
            break;
    }
}

void UIManager::pressButton() {
	// TODO: enter / exit mode
	displayDebugString("BUTTON PRESSED");
}

void UIManager::displayDebugString(const char* msg) {
	ST7735_WriteString(10, 10, msg, Font_11x18, ST7735_WHITE, ST7735_RED);
}
