#ifndef TASKS_HPP_
#define TASKS_HPP_

#include "queues.hpp"
#include "ui_display.hpp"
#include <lsm303dlhc_simple.h>

#define ENCODER_POS_THRESHOLD_RIGHT_MIN	2
#define ENCODER_POS_THRESHOLD_RIGHT_MAX	16349
#define ENCODER_POS_THRESHOLD_LEFT_MIN	16350
#define ENCODER_POS_THRESHOLD_LEFT_MAX	32766

#define GENERAL_STACK_SIZE	512*4

#define TIME_100_MS			100
#define TIME_500_MS			500

#define FREE_FALL_THRESHOLD	4915	// 0.3g * 16384

#ifdef __cplusplus
extern "C" {
#endif

void StartDisplayTask(void *argument);
void StartLoggerTask(void *argument);
void StartManagerTask(void *argument);
void StartEncoderTask(void *argument);
void StartLSM303Task(void *argument);

void sendLog(LogLevel level, const char* msg, ...);
void toggleLED(DisplayState& state);
void handleEncoderCommand(EncoderCommand cmd, DisplayState& state);
bool timeForLSM303AccelUpdate(const DisplayState& state, uint32_t lastLSM303Update, uint32_t now);
bool freeFallDetected(const LSM303DLHC_accel_raw& accel_data);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_HPP_ */
