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

void UIDisplay::init() {
    ST7735_FillScreen(ST7735_BLACK);
}

void UIDisplay::renderMenuMode(const DisplayState& state, const char** screenTitles, uint16_t currentBackgroundColor) {
    char buf[DISPLAY_TEXT_BUF_LEN];

    snprintf(buf, sizeof(buf), "< %s >", screenTitles[(int)state.currentScreen]);
    ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
}

void UIDisplay::renderActiveMode(const DisplayState& state, uint16_t currentBackgroundColor) {
    char buf[DISPLAY_TEXT_BUF_LEN];

	switch (state.currentScreen) {
		case SCREEN_ACCEL:
			snprintf(buf, sizeof(buf), "X = %d", state.accelX);
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);

			snprintf(buf, sizeof(buf), "Y = %d", state.accelY);
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);

			snprintf(buf, sizeof(buf), "Z = %d", state.accelZ);
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE3, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
			break;

		case SCREEN_LED:
			if (state.ledOn) {
				snprintf(buf, sizeof(buf), "LED: ON");
			} else {
				snprintf(buf, sizeof(buf), "LED: OFF");
			}
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
			break;

		case SCREEN_SETTINGS:
			snprintf(buf, sizeof(buf), "BG Color #%d", state.backgroundColor);
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
			break;

		case SCREEN_COMPASS:
			snprintf(buf, sizeof(buf), "Heading: %d degrees", state.headingDegrees);
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, buf, Font_7x10, ST7735_WHITE, currentBackgroundColor);

			snprintf(buf, sizeof(buf), "%s", directionFromDegrees(state.headingDegrees));
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_LINE1, buf, Font_11x18, ST7735_WHITE, currentBackgroundColor);
			break;

		default:
			ST7735_WriteString(UI_TEXT_X, UI_TEXT_Y_HEADER, "Unknown screen", Font_11x18, ST7735_WHITE, currentBackgroundColor);
			break;
	}
}

void UIDisplay::render(const DisplayState& state) {
    static const char* screenTitles[] = { "ACCEL", "LED", "SETTINGS", "COMPASS" };

    uint16_t currentBackgroundColor = backgroundColors[state.backgroundColor % backgroundColorCount()];
    ST7735_FillScreen(currentBackgroundColor);

    if (state.mode == MODE_MENU) {
		renderMenuMode(state, screenTitles, currentBackgroundColor);
	} else {
		renderActiveMode(state, currentBackgroundColor);
	}
}

size_t backgroundColorCount() {
	return sizeof(backgroundColors) / sizeof(backgroundColors[0]);
}

DisplayScreen nextScreen(const DisplayState& state) {
	return (DisplayScreen)((state.currentScreen + 1) % (SCREEN_COUNT));
}

DisplayScreen prevScreen(const DisplayState& state) {
	return (DisplayScreen)((state.currentScreen + SCREEN_COUNT - 1) % (SCREEN_COUNT));
}

uint16_t nextBackgroundColor(const DisplayState& state) {
	return (state.backgroundColor + 1) % backgroundColorCount();
}

uint16_t prevBackgroundColor(const DisplayState& state) {
	return (state.backgroundColor + 4) % backgroundColorCount();
}

const char* directionFromDegrees(int headingDagrees) {
	if (headingDagrees <= HEADING_TOLERANCE || headingDagrees >= HEADING_NORTH_2 - HEADING_TOLERANCE) {
		return "NORTH";
	} else if (headingDagrees >= HEADING_EAST - HEADING_TOLERANCE && headingDagrees <= HEADING_EAST + HEADING_TOLERANCE) {
		return "EAST";
	} else if (headingDagrees >= HEADING_SOUTH - HEADING_TOLERANCE && headingDagrees <= HEADING_SOUTH + HEADING_TOLERANCE) {
		return "SOUTH";
	} else if (headingDagrees >= HEADING_WEST - HEADING_TOLERANCE && headingDagrees <= HEADING_WEST + HEADING_TOLERANCE) {
		return "WEST";
	} else {
		return "";
	}
}
