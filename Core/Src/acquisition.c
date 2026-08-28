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

static AcquisitionState acquisition_state;
static uint16_t sample_index;
static uint32_t processed_event_count, missed_event_count;
static uint16_t sample_buffer[ACQUISITION_BUFFER_SIZE];

void Acquisition_Init(void)
{
	acquisition_state = ACQUISITION_STATE_IDLE;
	sample_index = 0U;
	processed_event_count = 0U;
	missed_event_count = 0U;
}

HAL_StatusTypeDef Acquisition_Start(void)
{
	HAL_StatusTypeDef status;

	if(acquisition_state == ACQUISITION_STATE_RUNNING)
		return HAL_BUSY;

	sample_index = 0U;
	processed_event_count = 0U;
	missed_event_count = 0U;
	SamplingTimer_ResetEventCount();

	status = SamplingTimer_Start();
	if(status != HAL_OK){

		acquisition_state = ACQUISITION_STATE_ERROR;
		return status;
	}

	acquisition_state = ACQUISITION_STATE_RUNNING;
	return HAL_OK;
}

void Acquisition_Process(void)
{
	uint32_t generated_event_count;
	uint32_t pending_event_count;
	uint32_t current_sample;

	if(acquisition_state != ACQUISITION_STATE_RUNNING){

		return;
	}

	generated_event_count = SamplingTimer_GetEventCount();

	if(generated_event_count > processed_event_count){

		pending_event_count = generated_event_count - processed_event_count;

		if(pending_event_count > 1U){

			missed_event_count += pending_event_count - 1U;
		}

		processed_event_count = generated_event_count;

		if(ADC_Measurement_ReadRaw(&current_sample) != HAL_OK){

			SamplingTimer_Stop();
			acquisition_state = ACQUISITION_STATE_ERROR;
			return;
		}

		if(sample_index >= ACQUISITION_BUFFER_SIZE){

			SamplingTimer_Stop();
			acquisition_state = ACQUISITION_STATE_ERROR;
			return;
		}

		sample_buffer[sample_index] = (uint16_t)current_sample;
		sample_index++;

		if(sample_index >= ACQUISITION_BUFFER_SIZE){

			if(SamplingTimer_Stop() != HAL_OK){

				acquisition_state = ACQUISITION_STATE_ERROR;
				return;
			}

			acquisition_state = ACQUISITION_STATE_COMPLETE;
		}
	}
}

AcquisitionState Acquisition_GetState(void)
{
	return acquisition_state;
}

uint16_t Acquisition_GetSampleCount(void)
{
	return sample_index;
}

uint32_t Acquisition_GetMissedEventCount(void)
{
	return missed_event_count;
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

	for(uint16_t i = 1U; i < sample_index; i++){

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
