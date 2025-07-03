#include "tasks.hpp"
#include "ui_manager.hpp"

extern "C" {
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
}

void StartUiTask(void *argument)
{
    for(;;)
    {
        int32_t pos = __HAL_TIM_GET_COUNTER(&htim1); // return only + values (1-11 for right, 32757-32767 for negative
        if (pos > 1 && pos < 16350) {
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
}

void StartLoggerTask(void *argument)
{
    for (;;) {
        // TODO: logger task
        osDelay(1);
    }
}
