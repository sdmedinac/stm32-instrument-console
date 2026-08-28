/*
 * sampling_timer.c
 *
 *  Created on: Aug 25, 2026
 *      Author: sdmedinac
 */

#include "sampling_timer.h"

static TIM_HandleTypeDef *timer_handle;
static volatile uint32_t timer_event_count = 0U;

void Sampling_TimerInit(TIM_HandleTypeDef *htim)
{
	timer_handle = htim;
	timer_event_count = 0U;
}

HAL_StatusTypeDef SamplingTimer_Start(void)
{
	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	return HAL_TIM_Base_Start_IT(timer_handle);
}

HAL_StatusTypeDef SamplingTimer_Stop(void)
{
	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	return HAL_TIM_Base_Stop_IT(timer_handle);
}

void SamplingTimer_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if((timer_handle != NULL) && (htim != NULL) && (htim->Instance == timer_handle->Instance)){

		timer_event_count++;
	}
}

uint32_t SamplingTimer_GetEventCount(void)
{
	return timer_event_count;
}

void SamplingTimer_ResetEventCount(void)
{
	timer_event_count = 0U;
}
