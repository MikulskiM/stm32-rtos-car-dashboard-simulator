/*
 * cpp_app.h
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#ifndef INC_CPP_APP_HPP_
#define INC_CPP_APP_HPP_

#include "cmsis_os.h"

class App {
public:
	void init();
	void run();

private:
	void initDisplay();
	void initI2CSensors();
	void initMutexes();
	void initTasks();
	void initEncoder();
};

extern App cpp_app;
extern osMutexId_t i2c1Mutex;	// mutex for I2C1

#endif /* INC_CPP_APP_HPP_ */
