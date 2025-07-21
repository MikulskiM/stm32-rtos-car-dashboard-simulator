#ifndef TASKS_HPP_
#define TASKS_HPP_

#include "queues.hpp"
#include "ui_display.hpp"

#define ENCODER_POS_THRESHOLD_RIGHT_MIN	2
#define ENCODER_POS_THRESHOLD_RIGHT_MAX	16349
#define ENCODER_POS_THRESHOLD_LEFT_MIN	16350
#define ENCODER_POS_THRESHOLD_LEFT_MAX	32766

#define GENERAL_STACK_SIZE	512*4

#define TIME_100_MS			100
#define TIME_500_MS			500

#ifdef __cplusplus
extern "C" {
#endif

void StartDisplayTask(void *argument);
void StartLoggerTask(void *argument);
void StartManagerTask(void *argument);
void StartEncoderTask(void *argument);
void StartAccelTask(void *argument);

void sendLog(LogLevel level, const char* msg);
void toggleLED(DisplayState& state);
void handleEncoderCommand(EncoderCommand cmd, DisplayState& state);
bool timeForAccelUpdate(const DisplayState& state, uint32_t lastAccelUpdate, uint32_t now);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_HPP_ */
