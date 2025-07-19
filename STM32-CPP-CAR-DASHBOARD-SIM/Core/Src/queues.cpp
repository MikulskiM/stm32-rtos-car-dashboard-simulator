/*
 * queues.cpp
 *
 *  Created on: Jul 17, 2025
 *      Author: Marek
 */

#include "queues.hpp"
#include <cstdio>		// prinft()

osMessageQueueId_t displayQueue = nullptr;
osMessageQueueId_t loggerQueue	= nullptr;
osMessageQueueId_t encoderQueue	= nullptr;

void checkQueueAndMsgSizeMatch(const char* name, osMessageQueueId_t queue, size_t expectedSize) {
	uint32_t actualSize = osMessageQueueGetMsgSize(queue);
	if (actualSize != expectedSize) {
		printf("ERROR: %s message size mismatch: queue=%lu struct=%lu\r\n",
		       name, actualSize, expectedSize);
		return;
	}
	return;
}
