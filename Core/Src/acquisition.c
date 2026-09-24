/*
 * acquisition.c
 *
 *  Created on: Aug 27, 2026
 *      Author: sdmedinac
 */
#include "acquisition.h"
#include "sampling_timer.h"
#include "adc_measurement.h"

#define ACQUISITION_BUFFER_CAPACITY 1024U
#define ACQUISITION_MIN_SAMPLE_COUNT 10U
#define ACQUISITION_DEFAULT_SAMPLE_COUNT 100U
#define ACQUISITION_TIMEOUT_MARGIN_MS 500U

static volatile AcquisitionState acquisition_state;
static volatile AcquisitionError acquisition_error;
static volatile uint16_t sample_index;
static uint16_t sample_buffer[ACQUISITION_BUFFER_CAPACITY], configured_sample_count;
static uint32_t acquisition_start_tick, acquisition_timeout_ms;
static uint32_t captured_sampling_frequency_hz;

void Acquisition_Init(void)
{
	acquisition_start_tick = 0U;
	acquisition_state = ACQUISITION_STATE_IDLE;
	acquisition_error = ACQUISITION_ERROR_NONE;
	acquisition_timeout_ms = 0U;
	sample_index = 0U;
	configured_sample_count = ACQUISITION_DEFAULT_SAMPLE_COUNT;
	captured_sampling_frequency_hz = 0U;
}

HAL_StatusTypeDef Acquisition_SetSampleCount(uint16_t sample_count)
{
	if(acquisition_state == ACQUISITION_STATE_RUNNING)
		return HAL_BUSY;

	if(sample_count < ACQUISITION_MIN_SAMPLE_COUNT)
		return HAL_ERROR;

	if(sample_count > ACQUISITION_BUFFER_CAPACITY)
		return HAL_ERROR;

	configured_sample_count = sample_count;

	return HAL_OK;
}

uint16_t Acquisition_GetConfiguredSampleCount(void)
{
	return configured_sample_count;
}

uint16_t Acquisition_GetBufferCapacity(void)
{
	return ACQUISITION_BUFFER_CAPACITY;
}

HAL_StatusTypeDef Acquisition_Start(void)
{
	HAL_StatusTypeDef status;
	uint64_t duration_numerator;
	uint32_t expected_duration_ms, sampling_frequency_hz;

	if(acquisition_state == ACQUISITION_STATE_RUNNING)
		return HAL_BUSY;

	sampling_frequency_hz = SamplingTimer_GetFrequency();

	if(sampling_frequency_hz == 0){

		acquisition_error = ACQUISITION_ERROR_TIMER_START;
		acquisition_state = ACQUISITION_STATE_ERROR;
		return HAL_ERROR;
	}

	duration_numerator = (uint64_t)configured_sample_count * 1000U;
	expected_duration_ms = (uint32_t)(duration_numerator + sampling_frequency_hz - 1U) / sampling_frequency_hz;
	acquisition_timeout_ms = expected_duration_ms + ACQUISITION_TIMEOUT_MARGIN_MS;

	acquisition_error = ACQUISITION_ERROR_NONE;
	sample_index = 0U;

	status = ADC_Measurement_StartDMA(sample_buffer, configured_sample_count);

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

	captured_sampling_frequency_hz = sampling_frequency_hz;
	acquisition_start_tick = HAL_GetTick();
	acquisition_state = ACQUISITION_STATE_RUNNING;
	return HAL_OK;
}

uint32_t Acquisition_GetCapturedSamplingFrequency(void)
{
	return captured_sampling_frequency_hz;
}

void Acquisition_Process(void)
{
	uint32_t current_tick, elapsed_time;

	if(acquisition_state != ACQUISITION_STATE_RUNNING)
		return;

	current_tick = HAL_GetTick();
	elapsed_time = current_tick - acquisition_start_tick;

	if(elapsed_time < acquisition_timeout_ms)
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

	uint64_t sum = 0U;
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

	sample_index = configured_sample_count;
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

HAL_StatusTypeDef Acquisition_GetSample(uint16_t index, uint16_t *sample)
{
	if(sample == NULL)
		return HAL_ERROR;

	if(acquisition_state != ACQUISITION_STATE_COMPLETE)
		return HAL_BUSY;

	if(index >= sample_index)
		return HAL_ERROR;

	*sample = sample_buffer[index];

	return HAL_OK;
}
