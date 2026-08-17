/*
 * console.h
 *
 *  Created on: Aug 15, 2026
 *      Author: sdmedinac
 */

#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_

#include "stm32f4xx_hal.h"

void Console_Init(UART_HandleTypeDef *huart);

void Console_Process(void);

void Console_RxCpltCallback(UART_HandleTypeDef *huart);

void Console_Write(const char *text);

#endif /* INC_CONSOLE_H_ */
