/*
 * sampling_timer.c
 *
 *  Created on: Aug 25, 2026
 *      Author: sdmedinac
 */

#include "sampling_timer.h"

static TIM_HandleTypeDef *timer_handle;

void Sampling_TimerInit(TIM_HandleTypeDef *htim)
{
	timer_handle = htim;
}

HAL_StatusTypeDef SamplingTimer_Start(void)
{
	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	return HAL_TIM_Base_Start(timer_handle);
}

HAL_StatusTypeDef SamplingTimer_Stop(void)
{
	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	return HAL_TIM_Base_Stop(timer_handle);
}

