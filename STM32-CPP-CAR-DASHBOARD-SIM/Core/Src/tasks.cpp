#include "tasks.hpp"
#include "st7735.h"

#include <cstring>	// strcpy()
#include <cstdio>	// printf()
#include <cstdarg>	// va_list, va_start(), va_end()

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
	LSM303DLHC_Snapshot snapshot;
	uint32_t lastLSM303Update = 0;
	bool stateChanged = true;	// initial display of the menu

    for (;;) {

		if (osMessageQueueGet(encoderQueue, &cmd, nullptr, 0) == osOK) {
			handleEncoderCommand(cmd, state);
			stateChanged = true;
		}

		if (osMessageQueueGet(lsm303Queue, &snapshot, nullptr, 0) == osOK) {
			if (state.currentScreen == SCREEN_ACCEL) {
				state.accelX = snapshot.accel.x;
				state.accelY = snapshot.accel.y;
				state.accelZ = snapshot.accel.z;
			}
			else if (state.currentScreen == SCREEN_COMPASS) {
				state.headingDegrees = snapshot.headingDegrees;
			}
		}

		uint32_t now = osKernelGetTickCount();
		if (stateChanged || timeForLSM303AccelUpdate(state, lastLSM303Update, now)) {
			osMessageQueuePut(displayQueue, &state, 0, 0);
			lastLSM303Update = now;
			stateChanged = false;
		}

		osDelay(10);
    }
}

void StartLSM303Task(void *argument) {
    printf("LSM303DLHC task started\r\n");

	bool currentlyFalling = false;
	uint32_t freeFallDuration = 0;
    LSM303DLHC_Snapshot snapshot;
    uint32_t tick = 0;

    for (;;) {
		LSM303_ReadAccel(&hi2c1, &snapshot.accel);
		snapshot.headingDegrees = LSM303_ReadHeadingDegrees(&hi2c1);

		if (freeFallDetected(snapshot.accel)) {
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

		tick += TIME_100_MS;
		if (tick >= TIME_500_MS) {
			tick = 0;

			sendLog(LOG_INFO, "Accel: x= %-7d y= %-7d z= %-7d | Heading:%4d degrees %s",
					 snapshot.accel.x, snapshot.accel.y, snapshot.accel.z,
					 snapshot.headingDegrees, directionFromDegrees(snapshot.headingDegrees));

			osStatus_t status = osMessageQueuePut(lsm303Queue, &snapshot, 0, 100);
			if (status != osOK) {
				printf("lsm303Queue FULL or error! status = %d\r\n", status);
			}
		}

		osDelay(TIME_100_MS);
    }
}

void sendLog(LogLevel level, const char* msg, ...) {
	if (!loggerQueue) {
		printf("loggerQueue not initialized yet!");
		return;
	}

	checkQueueAndMsgSizeMatch("loggerQueue", loggerQueue, sizeof(LogEvent));

	LogEvent log;
	log.level = level;

	va_list args;
	va_start(args, msg);
	vsnprintf(log.msg, sizeof(log.msg), msg, args);
	va_end(args);

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
				} else if (state.currentScreen == SCREEN_RADIO) {
					if (state.radioFrequency < RADIO_FREQ_MAX) {
						state.radioFrequency += RADIO_FREQ_STEP;
					}
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
				} else if (state.currentScreen == SCREEN_RADIO) {
					if (state.radioFrequency > RADIO_FREQ_MIN) {
						state.radioFrequency -= RADIO_FREQ_STEP;
					}
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

bool timeForLSM303AccelUpdate(const DisplayState& state, uint32_t lastLSM303Update, uint32_t now) {
	const uint32_t updateIntervalMs = 500;
	if (((now - lastLSM303Update) > updateIntervalMs) &&
			((state.mode == MODE_ACTIVE && state.currentScreen == SCREEN_ACCEL) ||
			 (state.mode == MODE_ACTIVE && state.currentScreen == SCREEN_COMPASS))) {
		return true;
	} else {
		return false;
	}
}

bool freeFallDetected(const LSM303DLHC_accel_raw& accel_data) {
    int32_t squared = accel_data.x * accel_data.x + accel_data.y * accel_data.y + accel_data.z * accel_data.z;
    return squared < (FREE_FALL_THRESHOLD * FREE_FALL_THRESHOLD);
}
