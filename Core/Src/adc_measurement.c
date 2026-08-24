/*
 * adc_measurement.c
 *
 *  Created on: Aug 18, 2026
 *      Author: sdmedinac
 */

#include "adc_measurement.h"

static ADC_HandleTypeDef *adc_handle;
#define ADC_REFERENCE_VOLTAGE_VALUE 3.3f
#define ADC_MAX_RAW_COUNT 4095U

void ADC_Measurement_Init(ADC_HandleTypeDef *hadc)
{
	adc_handle = hadc;
}

HAL_StatusTypeDef ADC_Measurement_ReadRaw(uint32_t *raw_value)
{
	if((adc_handle == NULL) || (raw_value == NULL)){

		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_ADC_Start(adc_handle);

	if(status != HAL_OK){

		return status;
	}

	status = HAL_ADC_PollForConversion(adc_handle, 10U);

	if(status != HAL_OK){

		HAL_ADC_Stop(adc_handle);
		return status;
	}

	*raw_value = HAL_ADC_GetValue(adc_handle);
	HAL_ADC_Stop(adc_handle);
	return HAL_OK;
}

float ADC_Measurement_RawToVoltage(uint32_t raw_value)
{
	 return (float)raw_value*(ADC_REFERENCE_VOLTAGE_VALUE / (float)ADC_MAX_RAW_COUNT);
}

HAL_StatusTypeDef ADC_Measurement_ReadVoltage(float *voltage)
{
	uint32_t raw_value;
	HAL_StatusTypeDef status;

	if(voltage == NULL){

		return HAL_ERROR;
	}

	status = ADC_Measurement_ReadRaw(&raw_value);

	if(status != HAL_OK){

		return status;
	}

	*voltage = ADC_Measurement_RawToVoltage(raw_value);
	return HAL_OK;

}

HAL_StatusTypeDef ADC_Measurement_ReadStats(uint32_t sample_count, ADC_MeasurementStats *stats)
{
	if((adc_handle == NULL) || (stats == NULL) || (sample_count == 0U)){

		return HAL_ERROR;
	}

	uint32_t current_sample;
	uint32_t maximum = 0U;
	uint32_t minimum = ADC_MAX_RAW_COUNT;
	uint64_t sum = 0U;
	HAL_StatusTypeDef status;

	for(uint32_t i = 0U; i < sample_count; i++){

		status = ADC_Measurement_ReadRaw(&current_sample);

		if(status != HAL_OK){

			return status;
		}

		sum += current_sample;

		if(current_sample > maximum){

			maximum = current_sample;
		}

		if(current_sample < minimum){

			minimum = current_sample;
		}

	}

	stats->maximum = maximum;
	stats->minimum = minimum;
	stats->peak_to_peak = maximum - minimum;
	stats->sample_count = sample_count;
	stats->average = (uint32_t)(sum / sample_count);

	return HAL_OK;
}
