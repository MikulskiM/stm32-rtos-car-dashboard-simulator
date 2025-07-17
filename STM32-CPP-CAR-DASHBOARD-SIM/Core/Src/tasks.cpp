#include "tasks.hpp"
#include "ui_manager.hpp"
#include "queues.hpp"

extern "C" {
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
}

void StartEncoderTask(void* argument) {
	for(;;)	{
		int32_t pos = __HAL_TIM_GET_COUNTER(&htim1); // return only + values (1-11 for right, 32757-32767 for negative
		EncoderCommand cmd = {};

		if (pos > ENCODER_POS_THRESHOLD_RIGHT_MIN && pos <= ENCODER_POS_THRESHOLD_RIGHT_MAX) {
			cmd.type = ENCODER_RIGHT;
		} else if (pos >= ENCODER_POS_THRESHOLD_LEFT_MIN) {
			cmd.type = ENCODER_LEFT;
		}
		osMessageQueuePut(encoderQueue, &cmd, 0, 0);
		__HAL_TIM_SET_COUNTER(&htim1, 0);

		osDelay(10);	// very light polling

		/* not a classic polling with while(1)
		* it's a RTOS task that sleeps most of the time
		* and looks only once in a while
		*/
	}
}

void StartUiManagerTask(void* argument) {
	EncoderCommand encCmd;

	for (;;) {
		if (osMessageQueueGet(encoderQueue, &encCmd, NULL, osWaitForever) == osOK) {
			switch (encCmd.type) {
				case ENCODER_RIGHT:
					if (uiManager.isInActiveMode()) {
						uiManager.exitActiveMode();
						uiManager.render();
					} else {
						uiManager.rotateRight();
					}
					break;
				case ENCODER_LEFT:
					if (uiManager.isInActiveMode()) {
						uiManager.exitActiveMode();
						uiManager.render();
					} else {
						uiManager.rotateLeft();
					}
					break;
				case ENCODER_CLICK:
					uiManager.pressButton();
					break;
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
		}
	}
}

void StartDisplayTask(void* argument) {

}

//void StartUiTask(void *argument)
//{
//    for(;;)
//    {
//        int32_t pos = __HAL_TIM_GET_COUNTER(&htim1); // return only + values (1-11 for right, 32757-32767 for negative
//
//        if (pos > ENCODER_POS_THRESHOLD_RIGHT_MIN && pos <= ENCODER_POS_THRESHOLD_RIGHT_MAX) {
//            if (uiManager.isInActiveMode()) {
//                uiManager.exitActiveMode();
//                uiManager.render();
//            } else {
//                uiManager.rotateRight();
//            }
//            __HAL_TIM_SET_COUNTER(&htim1, 0);
//        } else if (pos >= ENCODER_POS_THRESHOLD_LEFT_MIN) {
//            if (uiManager.isInActiveMode()) {
//                uiManager.exitActiveMode();
//                uiManager.render();
//            } else {
//                uiManager.rotateLeft();
//            }
//            __HAL_TIM_SET_COUNTER(&htim1, 0);
//        }
//
//        if (uiManager.isInActiveMode()) {
//            switch (uiManager.getCurrentScreen()) {
//                case SCREEN_CAR_STATUS:
//                    uiManager.displayCarStatus();
//                    break;
//                case SCREEN_RADIO:
//                    uiManager.displayRadio();
//                    break;
//                case SCREEN_SETTINGS:
//                    uiManager.displaySettings();
//                    break;
//                case SCREEN_LED_CONTROL:
//                    uiManager.displayLedControl();
//                    break;
//                default:
//                    break;
//            }
//        }
//
//        osDelay(10);	// very light polling
//
//		/* not a classic polling with while(1)
//		* it's a RTOS task that sleeps most of the time
//		* and looks only once in a while
//		*/
//    }
//}

void StartLoggerTask(void *argument)
{
    for (;;) {
        // TODO: logger task
        osDelay(1);
    }
}
