/*
 * adc_measurement.c
 *
 *  Created on: Aug 18, 2026
 *      Author: sdmedinac
 */

#include "adc_measurement.h"

static ADC_HandleTypeDef *adc_handle;
#define ADC_REFERENCE_VOLTAGE_VALUE 3.3f
#define ADC_MAX_RAW_VALUE 4095.0f

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

	*voltage = (float)raw_value *(ADC_REFERENCE_VOLTAGE_VALUE/ADC_MAX_RAW_VALUE);
	return HAL_OK;

}
