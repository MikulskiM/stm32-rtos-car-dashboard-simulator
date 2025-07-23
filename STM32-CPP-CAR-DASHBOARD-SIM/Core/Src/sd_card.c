/*
 * sd_card.c
 *
 *  Created on: Jul 23, 2025
 *      Author: Marek
 */

#include "sd_card.h"
#include "main.h"
#include "stm32f4xx_hal_spi.h"
#include "fatfs.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

static FATFS	sd_fs;
static FIL		sd_file;
static uint8_t	sd_mounted = 0;

/*
static void SD_Select(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}
*/

static void SD_Deselect(void) {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

int SD_Card_Init(void) {
    SD_Deselect();
    HAL_Delay(SD_INIT_DELAY_MS);

    FRESULT res = f_mount(&sd_fs, "", 1);
    if (res != FR_OK) {
        return SD_ERR_NOT_MOUNTED;
    }

    sd_mounted = 1;
    return SD_OK;
}

int SD_Log_Write(const char *text) {
    if (!sd_mounted) {
        return SD_ERR_NOT_MOUNTED;
    }

    FRESULT res = f_open(&sd_file, SD_LOG_FILENAME, FA_OPEN_APPEND | FA_WRITE);
    if (res != FR_OK) {
        return SD_ERR_OPEN_FAILED;
    }

    UINT bw;
    res = f_write(&sd_file, text, strlen(text), &bw);
    f_close(&sd_file);

    if (res == FR_OK && bw == strlen(text)) {
    	return SD_OK;
    } else {
    	return SD_ERR_WRITE_FAILED;
    }
}

int SD_Log_Writef(const char *format, ...) {
    char buf[SD_LOG_BUFFER_SIZE];

    va_list args;
    va_start(args, format);
    int len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    if (len < 0 || len >= SD_LOG_BUFFER_SIZE) {
        return SD_ERR_MOUNT_FAILED;
    }

    return SD_Log_Write(buf);
}
