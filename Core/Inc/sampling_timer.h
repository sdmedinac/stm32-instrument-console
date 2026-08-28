/*
 * sampling_timer.h
 *
 *  Created on: Aug 25, 2026
 *      Author: sdmedinac
 */

#ifndef INC_SAMPLING_TIMER_H_
#define INC_SAMPLING_TIMER_H_

#include "stm32f4xx_hal.h"

void Sampling_TimerInit(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef SamplingTimer_Start(void);
HAL_StatusTypeDef SamplingTimer_Stop(void);
void SamplingTimer_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
uint32_t SamplingTimer_GetEventCount(void);
void SamplingTimer_ResetEventCount(void);

#endif /* INC_SAMPLING_TIMER_H_ */
