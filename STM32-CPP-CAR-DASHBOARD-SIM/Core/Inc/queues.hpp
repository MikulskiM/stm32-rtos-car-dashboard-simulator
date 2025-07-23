/*
 * queues.hpp
 *
 *  Created on: Jul 17, 2025
 *      Author: Marek
 */

#ifndef INC_QUEUES_HPP_
#define INC_QUEUES_HPP_

#include "cmsis_os2.h"

#define EIGHT_MESSAGES		8
#define SIXTEEN_MESSAGES	16

#define TIMEOUT_0			0
#define TIMEOUT_100			100

#define MSG_PRIORITY_0		0

#define LOGGER_MSG_LEN		128
#define DISPLAY_BUF_LEN		16

#define SAFE_QUEUE_PUT(queue, value, queueName, priority, timeout, format)                \
	do {                                                                                  \
		osStatus_t status = osMessageQueuePut(queue, &(value), priority, timeout);        \
		if (status != osOK) {                                                             \
			printf("%s FULL or error! Dropped: " format " (status: %d)\r\n",              \
			       queueName, value, status);                                             \
		}                                                                                 \
	} while (0)

void checkQueueAndMsgSizeMatch(const char* name, osMessageQueueId_t queue, size_t expectedSize);

//	--- ENCODER ---
typedef enum {
	ENCODER_LEFT,
	ENCODER_RIGHT,
	ENCODER_CLICK
} EncoderCommand;

extern osMessageQueueId_t encoderQueue;

//	--- DISPLAY ---
extern osMessageQueueId_t displayQueue;

//	--- LOGGER ---
typedef enum {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

struct LogEvent {
	LogLevel level;
	char msg[LOGGER_MSG_LEN];
};

extern osMessageQueueId_t loggerQueue;

//	--- LSM303DLHC ---
extern osMessageQueueId_t lsm303Queue;

#endif /* INC_QUEUES_HPP_ */
