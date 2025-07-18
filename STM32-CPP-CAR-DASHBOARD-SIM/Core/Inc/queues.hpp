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
	char msg[64];
};

extern osMessageQueueId_t loggerQueue;

#endif /* INC_QUEUES_HPP_ */
