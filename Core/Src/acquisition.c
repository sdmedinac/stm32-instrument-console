/*
 * acquisition.c
 *
 *  Created on: Aug 27, 2026
 *      Author: sdmedinac
 */
#include "acquisition.h"
#include "sampling_timer.h"
#include "adc_measurement.h"

#define ACQUISITION_BUFFER_SIZE 100U
#define ACQUISITION_TIMEOUT_MS 500U

static volatile AcquisitionState acquisition_state;
static volatile AcquisitionError acquisition_error;
static volatile uint16_t sample_index;
static uint16_t sample_buffer[ACQUISITION_BUFFER_SIZE];
static uint32_t acquisition_start_tick;

void Acquisition_Init(void)
{
	acquisition_start_tick = 0;
	acquisition_state = ACQUISITION_STATE_IDLE;
	acquisition_error = ACQUISITION_ERROR_NONE;
	sample_index = 0U;
}

HAL_StatusTypeDef Acquisition_Start(void)
{
	HAL_StatusTypeDef status;

	if(acquisition_state == ACQUISITION_STATE_RUNNING)
		return HAL_BUSY;

	acquisition_error = ACQUISITION_ERROR_NONE;
	sample_index = 0U;

	status = ADC_Measurement_StartDMA(sample_buffer, ACQUISITION_BUFFER_SIZE);

	if(status != HAL_OK){

		acquisition_error = ACQUISITION_ERROR_ADC_DMA_START;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return status;
	}

	status = SamplingTimer_Start();

	if(status != HAL_OK){

		ADC_Measurement_StopDMA();
		acquisition_error = ACQUISITION_ERROR_TIMER_START;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return status;
	}

	acquisition_start_tick = HAL_GetTick();
	acquisition_state = ACQUISITION_STATE_RUNNING;
	return HAL_OK;
}

void Acquisition_Process(void)
{
	uint32_t current_tick, elapsed_time;

	if(acquisition_state != ACQUISITION_STATE_RUNNING)
		return;

	current_tick = HAL_GetTick();
	elapsed_time = current_tick - acquisition_start_tick;

	if(elapsed_time < ACQUISITION_TIMEOUT_MS)
		return;

	acquisition_error = ACQUISITION_ERROR_TIMEOUT;
	acquisition_state = ACQUISITION_STATE_ERROR;

	SamplingTimer_Stop();
	ADC_Measurement_StopDMA();
}

AcquisitionState Acquisition_GetState(void)
{
	return acquisition_state;
}

uint16_t Acquisition_GetSampleCount(void)
{
	return sample_index;
}

HAL_StatusTypeDef Acquisition_GetStats(ADC_MeasurementStats *stats)
{
	if(stats == NULL){

		return HAL_ERROR;
	}

	if(acquisition_state != ACQUISITION_STATE_COMPLETE){

		return HAL_BUSY;
	}

	if(sample_index == 0U){

		return HAL_ERROR;
	}

	uint64_t sum = sample_buffer[0];
	uint16_t maximum = sample_buffer[0];
	uint16_t minimum = sample_buffer[0];

	for(uint16_t i = 0U; i < sample_index; i++){

		sum += sample_buffer[i];

		if(sample_buffer[i] > maximum){

			maximum = sample_buffer[i];
		}

		if(sample_buffer[i] < minimum){

			minimum = sample_buffer[i];
		}
	}

	stats->minimum = (uint32_t)minimum;
	stats->maximum = (uint32_t)maximum;
	stats->peak_to_peak = (uint32_t)(maximum - minimum);
	stats->average = (uint32_t)(sum / sample_index);
	stats->sample_count = (uint32_t)sample_index;

	return HAL_OK;
}

void Acquisition_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	if(!ADC_Measurement_IsHandle(hadc)){

		return;
	}

	if(acquisition_state != ACQUISITION_STATE_RUNNING){

		return;
	}

	if(SamplingTimer_Stop() != HAL_OK){

		ADC_Measurement_StopDMA();
		acquisition_error = ACQUISITION_ERROR_TIMER_STOP;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return;
	}

	if(ADC_Measurement_StopDMA() != HAL_OK){

		acquisition_error = ACQUISITION_ERROR_ADC_DMA_STOP;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return;
	}

	sample_index = ACQUISITION_BUFFER_SIZE;
	acquisition_state = ACQUISITION_STATE_COMPLETE;
}

void Acquisition_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	if(!ADC_Measurement_IsHandle(hadc))
		return;

	if(acquisition_state != ACQUISITION_STATE_RUNNING)
		return;

	SamplingTimer_Stop();
	ADC_Measurement_StopDMA();

	acquisition_error = ACQUISITION_ERROR_ADC_DMA;
	acquisition_state = ACQUISITION_STATE_ERROR;
}

AcquisitionError Acquisition_GetError(void)
{
	return acquisition_error;
}

HAL_StatusTypeDef Acquisition_Stop(void)
{
	if((acquisition_state == ACQUISITION_STATE_IDLE) || (acquisition_state == ACQUISITION_STATE_COMPLETE))
		return HAL_OK;

	if(acquisition_state == ACQUISITION_STATE_ERROR)
		return HAL_ERROR;

	HAL_StatusTypeDef status = SamplingTimer_Stop();

	if(status != HAL_OK){

		ADC_Measurement_StopDMA();
		acquisition_error = ACQUISITION_ERROR_TIMER_STOP;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return status;
	}

	status = ADC_Measurement_StopDMA();

	if(status != HAL_OK){

		acquisition_error = ACQUISITION_ERROR_ADC_DMA_STOP;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return status;
	}

	sample_index = 0U;
	acquisition_error = ACQUISITION_ERROR_NONE;
	acquisition_state = ACQUISITION_STATE_IDLE;
	return HAL_OK;
}
