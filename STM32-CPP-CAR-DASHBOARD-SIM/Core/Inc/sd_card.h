/*
 * sd_card.h
 *
 *  Created on: Jul 23, 2025
 *      Author: Marek
 */

#ifndef INC_SD_CARD_H_
#define INC_SD_CARD_H_

#include "stdint.h"

#define SD_CS_GPIO_Port			GPIOB
#define SD_CS_Pin				GPIO_PIN_12

#define SD_LOG_FILENAME			"log.txt"
#define SD_LOG_BUFFER_SIZE		128

#define SD_INIT_DELAY_MS		20

int SD_Card_Init(void);
int SD_Log_Write(const char *text);
int SD_Log_Writef(const char *format, ...);

typedef enum {
	SD_OK = 0,
	SD_ERR_NOT_MOUNTED = -1,
	SD_ERR_OPEN_FAILED = -2,
	SD_ERR_WRITE_FAILED = -3,
	SD_ERR_MOUNT_FAILED = -4
} SD_Status_t;

#endif /* INC_SD_CARD_H_ */
