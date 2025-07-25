/*
 * ui_manager.h
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#ifndef UI_DISPLAY_HPP
#define UI_DISPLAY_HPP

#include <cstdint>
#include <cstddef>
#include "st7735.h"

#define UI_TEXT_X			10
#define UI_TEXT_X_SMALL		5

#define UI_TEXT_Y_HEADER	10
#define UI_TEXT_Y_LINE1		30
#define UI_TEXT_Y_LINE2		40
#define UI_TEXT_Y_LINE3		50

#define DISPLAY_TEXT_BUF_LEN  32

#define HEADING_NORTH		0
#define HEADING_EAST		90
#define HEADING_SOUTH		180
#define HEADING_WEST		270
#define HEADING_NORTH_2		360
#define HEADING_TOLERANCE	3

#define RADIO_FREQ_INIT_VAL	900
#define RADIO_FREQ_DIVIDER	10.0
#define RADIO_FREQ_STEP		1
#define RADIO_FREQ_MIN		875
#define RADIO_FREQ_MAX		1080

typedef enum {
    SCREEN_ACCEL,
	SCREEN_LED,
	SCREEN_SETTINGS,
	SCREEN_COMPASS,
	SCREEN_RADIO,
	SCREEN_COUNT
} DisplayScreen;

typedef enum {
    MODE_MENU,
	MODE_ACTIVE
} DisplayMode;

struct DisplayState {
    DisplayMode mode = MODE_MENU;
    DisplayScreen currentScreen = SCREEN_ACCEL;

    bool ledOn = false;
    int16_t accelX = 0;
    int16_t accelY = 0;
    int16_t accelZ = 0;

    uint8_t backgroundColor = 0;

    int headingDegrees;	// degrees 0–359

    uint16_t radioFrequency = RADIO_FREQ_INIT_VAL;	// MHz * 10
};

class UIDisplay {
public:
    void init();
    void render(const DisplayState& state);

private:
    void renderMenuMode(const DisplayState& state, const char** screenTitles, uint16_t currentBackgroundColor);
    void renderActiveMode(const DisplayState& state, uint16_t currentBackgroundColor);
};

DisplayScreen nextScreen(const DisplayState& state);
DisplayScreen prevScreen(const DisplayState& state);
uint16_t nextBackgroundColor(const DisplayState& state);
uint16_t prevBackgroundColor(const DisplayState& state);
size_t backgroundColorCount();
const char* directionFromDegrees(int headingDagrees);

#endif // UI_DISPLAY_HPP
