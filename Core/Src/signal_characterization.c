/*
 * signal_characterization.c
 *
 *  Created on: Oct 6, 2026
 *      Author: sdmedinac
 */

#include "signal_characterization.h"
#include <math.h>

#define SIGNAL_CHARACTERIZATION_ADC_MIN_LIMIT 5U
#define SIGNAL_CHARACTERIZATION_ADC_MAX_LIMIT 4090U

HAL_StatusTypeDef SignalCharacterization_Analyze(
		const uint16_t *samples,
		uint16_t sample_count,
		uint32_t sampling_frequency_hz,
		uint16_t baseline_start,
		uint16_t baseline_count,
		SignalCharacterizationResult *result
)
{
	if(samples == NULL || result == NULL)
		return HAL_ERROR;

	if(sample_count == 0U || sampling_frequency_hz == 0U)
		return HAL_ERROR;

	if(baseline_count == 0U)
		return HAL_ERROR;

	if(baseline_start >= sample_count)
		return HAL_ERROR;

	if(baseline_count > (sample_count - baseline_start))
		return HAL_ERROR;

	uint64_t baseline_sum = 0U;
	uint16_t baseline_end = baseline_start + baseline_count;

	for(uint16_t i = baseline_start; i < baseline_end; i++)
	{
		baseline_sum += samples[i];
	}

	result->baseline = (uint32_t)(baseline_sum / baseline_count);

	uint64_t squared_sum = 0U;
	float mean_squared;

	for(uint16_t i = baseline_start; i < baseline_end; i++)
	{
		int32_t difference = (int32_t)samples[i] - (int32_t)(result->baseline);
		int64_t squared = (int64_t)difference * (int64_t)difference;
		squared_sum += (uint64_t)squared;
	}

	mean_squared = (float)squared_sum / (float)baseline_count;

	result->rms_noise = sqrtf(mean_squared);

	uint16_t peak_value = samples[0], peak_index = 0U;
	result->lower_saturation = false;
	result->upper_saturation = false;

	for(uint16_t i = 0U; i < sample_count; i++)
	{
		if(samples[i] > peak_value)
		{
			peak_value = samples[i];
			peak_index = i;
		}

		if(samples[i] >= SIGNAL_CHARACTERIZATION_ADC_MAX_LIMIT)
		{
			result->upper_saturation = true;
		}

		if(samples[i] <= SIGNAL_CHARACTERIZATION_ADC_MIN_LIMIT)
		{
			result->lower_saturation = true;
		}
	}

	result->peak_value = peak_value;
	result->peak_index = peak_index;

	if(peak_value >= result->baseline)
	{
		result->amplitude = (uint32_t)peak_value - result->baseline;
	}
	else
	{
		result->amplitude = 0U;
	}

	result->peak_time_us = (uint32_t)((uint64_t)peak_index * 1000000U/ sampling_frequency_hz);

	return HAL_OK;
}

