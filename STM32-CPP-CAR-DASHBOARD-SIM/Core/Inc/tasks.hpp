#ifndef TASKS_HPP_
#define TASKS_HPP_

#include "queues.hpp"

#define ENCODER_POS_THRESHOLD_RIGHT_MIN	2
#define ENCODER_POS_THRESHOLD_RIGHT_MAX	16349
#define ENCODER_POS_THRESHOLD_LEFT_MIN	16350
#define ENCODER_POS_THRESHOLD_LEFT_MAX	32766

#ifdef __cplusplus
extern "C" {
#endif

void StartDisplayTask(void *argument);
void StartLoggerTask(void *argument);
void StartManagerTask(void *argument);
void StartEncoderTask(void *argument);
void sendLog(LogLevel level, const char* msg);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_HPP_ */
