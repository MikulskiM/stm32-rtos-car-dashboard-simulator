#include "tasks.hpp"
#include "st7735.h"

#include <cstring>	// strcpy()
#include <cstdio>	// printf()

extern "C" {
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "lsm303dlhc_simple.h"
}

inline void displayLog(const char* msg) {
    while (*msg) {
        ITM_SendChar(*msg++);
    }
}

void StartDisplayTask(void *argument) {
	printf("Display task started\r\n");

	UIDisplay display;
	display.init();
	DisplayState state;

    for(;;) {
    	if (osMessageQueueGet(displayQueue, &state, NULL, osWaitForever) == osOK) {
			display.render(state);
		}
        osDelay(10);
    }
}

void StartLoggerTask(void *argument) {
	printf("Logger task started\r\n");
	LogEvent log;

	checkQueueAndMsgSizeMatch("loggerQueue", loggerQueue, sizeof(LogEvent));

	for (;;) {
		if (osMessageQueueGet(loggerQueue, &log, NULL, osWaitForever) == osOK) {

			const char* levelStr = "";
			switch (log.level) {
				case LOG_INFO:  levelStr = "INFO";  break;
				case LOG_WARN:  levelStr = "WARN";  break;
				case LOG_ERROR: levelStr = "ERROR"; break;
				default:        levelStr = "???";   break;
			}

			char buffer[LOGGER_MSG_LEN];
			snprintf(buffer, sizeof(buffer), "[%s] %s\r\n", levelStr, log.msg);
			displayLog(buffer);
		}
		osDelay(1);
	}
}

void StartEncoderTask(void *argument) {
	printf("Encoder task started\r\n");

	for (;;) {

		uint32_t pos = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
		EncoderCommand cmd;
		bool rotated = false;

		if (pos >= ENCODER_POS_THRESHOLD_RIGHT_MIN && pos <= ENCODER_POS_THRESHOLD_RIGHT_MAX) {
			cmd = ENCODER_RIGHT;
			sendLog(LOG_INFO, "Encoder rotated RIGHT");
			rotated = true;
		} else if (pos >= ENCODER_POS_THRESHOLD_LEFT_MIN && pos <= ENCODER_POS_THRESHOLD_LEFT_MAX) {
			cmd = ENCODER_LEFT;
			sendLog(LOG_INFO, "Encoder rotated LEFT");
			rotated = true;
		}

		if (rotated) {
    		SAFE_QUEUE_PUT(encoderQueue, cmd, "encoderQueue", MSG_PRIORITY_0, TIMEOUT_100, "%d");

			__HAL_TIM_SET_COUNTER(&htim1, 0);
		}

		osDelay(10);
	}
}

void StartManagerTask(void *argument) {
	printf("Manager task started\r\n");

	EncoderCommand cmd;
	DisplayState state;
	LSM303DLHC_accel_raw accelSnapshot;
	uint32_t lastAccelUpdate = 0;
	bool stateChanged = true;	// initial display of the menu

    for (;;) {

		if (osMessageQueueGet(encoderQueue, &cmd, nullptr, 0) == osOK) {
			handleEncoderCommand(cmd, state);
			stateChanged = true;
		}

		if (osMessageQueueGet(accelQueue, &accelSnapshot, nullptr, 0) == osOK) {
			state.accelX = accelSnapshot.x;
			state.accelY = accelSnapshot.y;
			state.accelZ = accelSnapshot.z;
		}

		uint32_t now = osKernelGetTickCount();
		if (stateChanged || timeForAccelUpdate(state, lastAccelUpdate, now)) {
			osMessageQueuePut(displayQueue, &state, 0, 0);
			lastAccelUpdate = now;
			stateChanged = false;
		}

		osDelay(10);
    }
}

void StartAccelTask(void *argument) {
	printf("Accelerator task started\r\n");

	LSM303DLHC_accel_raw latestSample = {0};
	uint32_t tick = 0;
	bool currentlyFalling = false;
	uint32_t freeFallDuration = 0;

	for (;;) {
		LSM303_ReadAccel(&hi2c1, &latestSample);

		int32_t x = latestSample.x;
		int32_t y = latestSample.y;
		int32_t z = latestSample.z;

		if (freeFallDetected(latestSample)) {
			if (currentlyFalling) {
				freeFallDuration += TIME_100_MS;
				if (freeFallDuration % TIME_500_MS == 0) {
					sendLog(LOG_WARN, "STILL FALLING...");
				}
			} else {
				currentlyFalling = true;
				sendLog(LOG_WARN, "FREE FALL!!!");
			}
		} else if (currentlyFalling) {
		    currentlyFalling = false;
		    sendLog(LOG_INFO, "FREE FALL ENDED (%dms)!!! We hope you're ok...", freeFallDuration);
		    freeFallDuration = 0;
		}

		// every 500 ms send snapshot to the manager
		tick += TIME_100_MS;
		if (tick >= TIME_500_MS) {
			tick = 0;

			printf("x = %d    y = %d    z = %d\n", x, y, z);

			osStatus_t status = osMessageQueuePut(accelQueue, &latestSample, 0, 100);
			if (status != osOK) {
				printf("accelQueue FULL or error! status = %d\r\n", status);
			}
		}

		osDelay(TIME_100_MS);
	}
}

void sendLog(LogLevel level, const char* msg) {
	if (!loggerQueue) {
		printf("loggerQueue not initialized yet!");
		return;
	}

	checkQueueAndMsgSizeMatch("loggerQueue", loggerQueue, sizeof(LogEvent));

	LogEvent log;
	log.level = level;
	strncpy(log.msg, msg, sizeof(log.msg) - 1);
	log.msg[sizeof(log.msg) - 1] = '\0';  // Null-terminate

	SAFE_QUEUE_PUT(loggerQueue, log, "loggerQueue", MSG_PRIORITY_0, TIMEOUT_100, "%s");
}

void toggleLED(DisplayState& state) {
	state.ledOn = !state.ledOn;
	if (state.ledOn) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	}
}

void handleEncoderCommand(EncoderCommand cmd, DisplayState& state) {
	switch (cmd) {
		case ENCODER_RIGHT:
			if (state.mode == MODE_MENU) {
				state.currentScreen = nextScreen(state);
			} else {
				if (state.currentScreen == SCREEN_LED) {
					toggleLED(state);
				} else if (state.currentScreen == SCREEN_SETTINGS) {
					state.backgroundColor = nextBackgroundColor(state);
				}
			}
			break;

		case ENCODER_LEFT:
			if (state.mode == MODE_MENU) {
				state.currentScreen = prevScreen(state);
			} else {
				if (state.currentScreen == SCREEN_LED) {
					toggleLED(state);
				} else if (state.currentScreen == SCREEN_SETTINGS) {
					state.backgroundColor = prevBackgroundColor(state);
				}
			}
			break;

		case ENCODER_CLICK:
			if (state.mode == MODE_MENU) {
				state.mode = MODE_ACTIVE;
			} else {
				state.mode = MODE_MENU;
			}
			break;
	}
}

bool timeForAccelUpdate(const DisplayState& state, uint32_t lastAccelUpdate, uint32_t now) {
	const uint32_t updateIntervalMs = 500;
	if (((now - lastAccelUpdate) > updateIntervalMs) &&
			state.mode == MODE_ACTIVE &&
			state.currentScreen == SCREEN_ACCEL) {
		return true;
	} else {
		return false;
	}
}

bool freeFallDetected(const LSM303DLHC_accel_raw& accel_data) {
    int32_t squared = accel_data.x * accel_data.x + accel_data.y * accel_data.y + accel_data.z * accel_data.z;
    return squared < (FREE_FALL_THRESHOLD * FREE_FALL_THRESHOLD);
}
