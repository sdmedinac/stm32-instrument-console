/*
 * sampling_timer.c
 *
 *  Created on: Aug 25, 2026
 *      Author: sdmedinac
 */

#include "sampling_timer.h"
#include <stdbool.h>

#define SAMPLING_TIMER_MIN_FREQUENCY_HZ 10U
#define SAMPLING_TIMER_MAX_FREQUENCY_HZ 10000U
#define SAMPLING_TIMER_COUNTER_CLOCK_HZ 1000000U

static TIM_HandleTypeDef *timer_handle;
static bool timer_running;
static uint32_t sampling_frequency_hz;

void Sampling_TimerInit(TIM_HandleTypeDef *htim)
{
	timer_handle = htim;
	timer_running = false; // Timer stop
	sampling_frequency_hz = 1000U;
}

HAL_StatusTypeDef SamplingTimer_SetFrequency(uint32_t requested_frequency_hz)
{
	uint32_t timer_counts, auto_reload;

	if(timer_handle == NULL)
		return HAL_ERROR;

	if(requested_frequency_hz < SAMPLING_TIMER_MIN_FREQUENCY_HZ)
		return HAL_ERROR;

	if(requested_frequency_hz > SAMPLING_TIMER_MAX_FREQUENCY_HZ)
		return HAL_ERROR;

	if(timer_running)
		return HAL_BUSY;

	timer_counts = SAMPLING_TIMER_COUNTER_CLOCK_HZ / requested_frequency_hz;

	if(timer_counts == 0)
		return HAL_ERROR;

	auto_reload = timer_counts - 1U;

	timer_handle->Init.Period = auto_reload;
	__HAL_TIM_SET_AUTORELOAD(timer_handle, auto_reload);
	__HAL_TIM_SET_COUNTER(timer_handle, 0U);

	sampling_frequency_hz = SAMPLING_TIMER_COUNTER_CLOCK_HZ / timer_counts;

	return HAL_OK;
}

uint32_t SamplingTimer_GetFrequency(void)
{
	return sampling_frequency_hz;
}

HAL_StatusTypeDef SamplingTimer_Start(void)
{

	HAL_StatusTypeDef status;

	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	if(timer_running){

		return HAL_BUSY;
	}

	status = HAL_TIM_Base_Start(timer_handle);

	if(status == HAL_OK){

		timer_running = true;
	}

	return status;
}

HAL_StatusTypeDef SamplingTimer_Stop(void)
{
	HAL_StatusTypeDef status;

	if(timer_handle == NULL){

		return HAL_ERROR;
	}

	if(!timer_running){

		return HAL_OK;
	}

	status = HAL_TIM_Base_Stop(timer_handle);

	if(status == HAL_OK){

		timer_running = false;
	}

	return status;
}

