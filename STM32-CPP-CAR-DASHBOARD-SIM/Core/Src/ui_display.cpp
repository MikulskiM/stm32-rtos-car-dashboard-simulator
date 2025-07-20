/*
 * ui_manager.cpp
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#include <ui_display.hpp>
#include "st7735.h"
#include "fonts.h"
#include <cstdio>

static const uint16_t backgroundColors[] = {
    ST7735_BLACK,
    ST7735_BLUE,
    ST7735_RED,
    ST7735_GREEN,
    ST7735_WHITE
};

const size_t BACKGROUND_COLOR_COUNT = sizeof(backgroundColors) / sizeof(backgroundColors[0]);

void UIDisplay::init() {
    ST7735_FillScreen(ST7735_BLACK);
}

void UIDisplay::render(const DisplayState& state) {
    static const char* screenTitles[] = { "ACCEL", "LED", "SETTINGS" };

    uint16_t currentBackgroundColor = backgroundColors[state.backgroundColor % BACKGROUND_COLOR_COUNT];
    ST7735_FillScreen(currentBackgroundColor);

    char buf[DISPLAY_TEXT_BUF_LEN];

    if (state.mode == MODE_MENU) {
        snprintf(buf, sizeof(buf), "< %s >", screenTitles[(int)state.currentScreen]);
        ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
        return;
    }

    switch (state.currentScreen) {
        case SCREEN_ACCEL:
            snprintf(buf, sizeof(buf), "X=%d", state.accelX);
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);

            snprintf(buf, sizeof(buf), "Y=%d", state.accelY);
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);

            snprintf(buf, sizeof(buf), "Z=%d", state.accelZ);
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE3, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
            break;

        case SCREEN_LED:
            snprintf(buf, sizeof(buf), "LED: %s", state.ledOn ? "ON" : "OFF");
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
            break;

        case SCREEN_SETTINGS:
            snprintf(buf, sizeof(buf), "BG Color #%d", state.backgroundColor);
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
            break;

        default:
            ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "Unknown screen", Font_11x18, ST7735_WHITE, currentBackgroundColor);
            break;
    }
}
