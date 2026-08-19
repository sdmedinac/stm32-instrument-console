/*
 * led.h
 *
 *  Created on: Aug 17, 2026
 *      Author: sdmedinac
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

void LED_Init(GPIO_TypeDef *port, uint16_t pin);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
bool LED_IsOn(void);

#endif /* INC_LED_H_ */
