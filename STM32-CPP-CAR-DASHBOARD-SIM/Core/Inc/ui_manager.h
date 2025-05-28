/*
 * ui_manager.h
 *
 *  Created on: May 25, 2025
 *      Author: Praca
 */

#ifndef UI_MANAGER_H_
#define UI_MANAGER_H_

typedef enum {
    SCREEN_CAR_STATUS,
    SCREEN_RADIO,
    SCREEN_SETTINGS,
    SCREEN_LED_CONTROL,
    SCREEN_COUNT  // screen counter
} MenuScreen;

typedef enum {
	UI_MODE_MENU,	// display only modes' names
	UI_MODE_ACTIVE	// display what's inside the mode
} UiInteractionMode;

class UIManager {
public:
	UIManager();
	virtual ~UIManager();

	void rotateLeft();
	void rotateRight();
	void pressButton();
	void render();

	void displayDebugString(const char* msg);

	void displayCarStatus();
	void displayRadio();
	void displaySettings();
	void displayLedControl();
	bool isInActiveMode();
	void exitActiveMode();

	MenuScreen getCurrentScreen();
	MenuScreen nextScreen();
	MenuScreen previousScreen();

private:
	MenuScreen currentScreen = SCREEN_CAR_STATUS;
	UiInteractionMode mode = UI_MODE_MENU;
};

#endif /* UI_MANAGER_H_ */
