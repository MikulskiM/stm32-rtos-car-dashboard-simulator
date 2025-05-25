/*
 * cpp_app.h
 *
 *  Created on: May 25, 2025
 *      Author: Marek
 */

#ifndef INC_CPP_APP_H_
#define INC_CPP_APP_H_

#ifdef __cplusplus
extern "C" {
#endif
void CppApp(void);
#ifdef __cplusplus
}
#endif

class App {
public:
	App();
	virtual ~App();

	void init();
	void run();
};

#endif /* INC_CPP_APP_H_ */
