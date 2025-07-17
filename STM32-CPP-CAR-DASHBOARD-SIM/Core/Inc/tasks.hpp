#ifndef TASKS_HPP_
#define TASKS_HPP_

#define ENCODER_POS_THRESHOLD_RIGHT_MIN	1
#define ENCODER_POS_THRESHOLD_RIGHT_MAX	16349

#define ENCODER_POS_THRESHOLD_LEFT_MIN	16350

#ifdef __cplusplus
extern "C" {
#endif

void StartUiTask(void *argument);
void StartEncoderTask(void* argument);
void StartUiManagerTask(void* argument);
void StartDisplayTask(void* argument);
void StartLoggerTask(void* argument);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_HPP_ */
