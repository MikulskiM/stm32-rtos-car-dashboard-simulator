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

	uint32_t value = 0;
	char buf[DISPLAY_BUF_LEN];

	ST7735_FillScreen(ST7735_BLACK);

    for(;;) {
    	if (osMessageQueueGet(displayQueue, &value, nullptr, osWaitForever) == osOK) {
			snprintf(buf, sizeof(buf), "Value: %d", value);
			ST7735_FillScreen(ST7735_BLACK);
			ST7735_WriteString(10, 10, buf, Font_11x18, ST7735_WHITE, ST7735_BLACK);
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

void StartEncoderTask(void *argument) {
	printf("Encoder task started\r\n");

	for (;;) {

		uint32_t pos = (int16_t)__HAL_TIM_GET_COUNTER(&htim1);
		EncoderCommand cmd;
		bool rotated = false;

		if (pos >= ENCODER_POS_THRESHOLD_RIGHT_MIN && pos <= ENCODER_POS_THRESHOLD_RIGHT_MAX) {
			cmd = ENCODER_RIGHT;
			rotated = true;
		} else if (pos >= ENCODER_POS_THRESHOLD_LEFT_MIN && pos <= ENCODER_POS_THRESHOLD_LEFT_MAX) {
			cmd = ENCODER_LEFT;
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
    uint32_t counter = 0;
    char msg[LOGGER_MSG_LEN];
    LSM303DLHC_accel_raw accel;

    for (;;) {

    	if (osMessageQueueGet(encoderQueue, &cmd, nullptr, 0) == osOK) {

    		if (cmd == ENCODER_RIGHT) {
    			counter++;
        		snprintf(msg, sizeof(msg), "Encoder turned right - counter: %d", counter);
    		} else if (cmd == ENCODER_LEFT) {
				counter--;
				snprintf(msg, sizeof(msg), "Encoder turned left - counter: %d", counter);
    		} else {
    			snprintf(msg, sizeof(msg), "Encoder CLICKED - counter: %d", counter);
    		}

    		sendLog(LOG_INFO, msg);

    		SAFE_QUEUE_PUT(displayQueue, counter, "displayQueue", MSG_PRIORITY_0, TIMEOUT_100, "%s");
		}

    	if (osMessageQueueGet(accelQueue, &accel, nullptr, 0) == osOK) {
    		snprintf(msg, sizeof(msg), "Accel: X=%d Y=%d Z=%d", accel.x, accel.y, accel.z);
    		sendLog(LOG_INFO, msg);

    		// TODO: DISPLAY ACCEL DATA
    	}

		osDelay(10);
    }
}


void StartAccelTask(void *argument) {
	printf("Accelerator task started\r\n");

	LSM303DLHC_accel_raw latestSample = {0};
	uint32_t tick = 0;

	for (;;) {
		LSM303_ReadAccel(&hi2c1, &latestSample);

		int32_t x = latestSample.x;
		int32_t y = latestSample.y;
		int32_t z = latestSample.z;

		// every 500 ms send snapshot to the manager
		tick += TIME_100_MS;
		if (tick >= TIME_500_MS) {
			tick = 0;

			osStatus_t status = osMessageQueuePut(accelQueue, &latestSample, 0, 100);
			if (status != osOK) {
				printf("accelQueue FULL or error! status = %d\r\n", status);
			}
		}

		osDelay(TIME_100_MS);
	}
}

