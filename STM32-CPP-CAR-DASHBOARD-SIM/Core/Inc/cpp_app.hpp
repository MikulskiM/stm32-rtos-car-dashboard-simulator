/*
 * cpp_app.h
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#ifndef INC_CPP_APP_HPP_
#define INC_CPP_APP_HPP_

#include "cmsis_os.h"

extern osMutexId_t i2c1Mutex;	// mutex for I2C1

class App {
public:

	static void init();
	static void run();
};

#endif /* INC_CPP_APP_HPP_ */
