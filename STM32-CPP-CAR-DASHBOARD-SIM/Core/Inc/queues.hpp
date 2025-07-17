/*
 * queues.hpp
 *
 *  Created on: Jul 17, 2025
 *      Author: Marek
 */

#ifndef INC_QUEUES_HPP_
#define INC_QUEUES_HPP_

#include "cmsis_os2.h"
#include "ui_manager.hpp"	// MenuScreen enum

#define EIGHT_MESSAGES		8
#define SIXTEEN_MESSAGES	16

//	--- ENCODER ---
enum EncoderCommandType {
	ENCODER_LEFT,
	ENCODER_RIGHT,
	ENCODER_CLICK
};

struct EncoderCommand {
	EncoderCommandType type;
};
extern osMessageQueueId_t encoderQueue;

//	--- DISPLAY ---
enum DisplayCommandType {
	DISPLAY_MENU,
	DISPLAY_MODE_CAR,
	DISPLAY_MODE_RADIO,
	DISPLAYCLEAR
};

struct DisplayCommand {
	DisplayCommandType	type;
	MenuScreen			screen;
};
extern osMessageQueueId_t displayQueue;


//	--- LOGGER ---
struct LogEvent {
	char msg[64];
};
extern osMessageQueueId_t loggerQueue;

#endif /* INC_QUEUES_HPP_ */
