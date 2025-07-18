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

	LogEvent e;
	e.level = level;
	strncpy(e.msg, msg, sizeof(e.msg) - 1);
	e.msg[sizeof(e.msg) - 1] = '\0';  // Null-terminate

	osStatus_t status = osMessageQueuePut(loggerQueue, &e, 0, 100);
	if (status != osOK) {
		printf("LoggerQueue FULL or error! Dropped log: %s (status: %d)\r\n", e.msg, status);
	}
}

void StartManagerTask(void *argument) {
	printf("Manager task started\r\n");
    uint32_t counter = 0;

    for (;;) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Manager sends log #%lu", counter);
        sendLog(LOG_INFO, msg);

        osStatus_t status = osMessageQueuePut(displayQueue, &counter, 0, 100);
		if (status != osOK) {
			printf("LoggerQueue FULL or error! Dropped log: %s (status: %d)\r\n", msg, status);
		}

        osDelay(2000);  // 2 sekundy
        counter++;
    }
}
