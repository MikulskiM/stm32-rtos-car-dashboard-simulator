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

class UIManager {
public:
	UIManager();
	virtual ~UIManager();

	void rotateLeft();
	void rotateRight();
	void pressButton();
	void render();
	void displayDebugString(const char* msg);

private:
	MenuScreen currentScreen = SCREEN_CAR_STATUS;
};

#endif /* UI_MANAGER_H_ */
