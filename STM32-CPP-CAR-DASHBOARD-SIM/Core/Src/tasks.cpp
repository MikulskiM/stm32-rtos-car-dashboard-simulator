#include "tasks.hpp"
#include "st7735.h"

#include <cstring>	// strcpy()
#include <cstdio>	// printf()

extern "C" {
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
}

inline void displayLog(const char* msg) {
    while (*msg) {
        ITM_SendChar(*msg++);
    }
}

void StartDisplayTask(void *argument) {
	printf("Display task started\r\n");

	uint32_t value = 0;
	char buf[16];

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

	uint32_t queueSize = osMessageQueueGetMsgSize(loggerQueue);
	if (queueSize != sizeof(LogEvent)) {
		printf("ERROR: loggerQueue message size mismatch: queue=%lu struct=%lu\r\n",
			   queueSize, sizeof(LogEvent));
	}

	for (;;) {
		if (osMessageQueueGet(loggerQueue, &log, NULL, osWaitForever) == osOK) {

			const char* levelStr = "";
			switch (log.level) {
				case LOG_INFO:  levelStr = "INFO";  break;
				case LOG_WARN:  levelStr = "WARN";  break;
				case LOG_ERROR: levelStr = "ERROR"; break;
				default:        levelStr = "???";   break;
			}

			char buffer[128];
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

	const uint32_t queueMsgSize = osMessageQueueGetMsgSize(loggerQueue);
	if (queueMsgSize != sizeof(LogEvent)) {
		printf("ERROR: loggerQueue size mismatch! Queue size: %lu, Struct size: %lu\r\n",
			   queueMsgSize, sizeof(LogEvent));
		return;
	}

	LogEvent log;
	log.level = level;
	strncpy(log.msg, msg, sizeof(log.msg) - 1);
	log.msg[sizeof(log.msg) - 1] = '\0';  // Null-terminate

	osStatus_t status = osMessageQueuePut(loggerQueue, &log, 0, 100);
	if (status != osOK) {
		printf("LoggerQueue FULL or error! Dropped log: %s (status: %d)\r\n", log.msg, status);
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
			rotated = true;
		} else if (pos >= ENCODER_POS_THRESHOLD_LEFT_MIN && pos <= ENCODER_POS_THRESHOLD_LEFT_MAX) {
			cmd = ENCODER_LEFT;
			rotated = true;
		}

		if (rotated) {
			osStatus_t status = osMessageQueuePut(encoderQueue, &cmd, 0, 100);
			if (status != osOK) {
				printf("encoderQueue FULL or error! Dropped cmd: %d (status: %d)\r\n", cmd, status);
			}

			__HAL_TIM_SET_COUNTER(&htim1, 0);
		}

		osDelay(10);
	}
}

void StartManagerTask(void *argument) {
	printf("Manager task started\r\n");
	EncoderCommand cmd;
    uint32_t counter = 0;
    char msg[64];

    for (;;) {

    	if (osMessageQueueGet(encoderQueue, &cmd, nullptr, osWaitForever) == osOK) {

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

			osStatus_t status = osMessageQueuePut(displayQueue, &counter, 0, 100);
			if (status != osOK) {
				printf("displayQueue FULL or error! Dropped log: %s (status: %d)\r\n", msg, status);
			}
		}

		osDelay(10);
    }
}
