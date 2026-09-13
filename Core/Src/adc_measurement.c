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

HAL_StatusTypeDef ADC_Measurement_ReadInjectedRaw(uint32_t *raw_value)
{
	HAL_StatusTypeDef status;

	if((adc_handle == NULL) || (raw_value == NULL))
		return HAL_ERROR;

	status = HAL_ADCEx_InjectedStart(adc_handle);

	if(status != HAL_OK)
		return status;

	status = HAL_ADCEx_InjectedPollForConversion(adc_handle, 10U);

	if(status != HAL_OK){

		HAL_ADCEx_InjectedStop(adc_handle);
		return status;
	}

	*raw_value = HAL_ADCEx_InjectedGetValue(adc_handle, ADC_INJECTED_RANK_1);
	status = HAL_ADCEx_InjectedStop(adc_handle);

	if(status != HAL_OK)
		return status;

	return HAL_OK;
}

HAL_StatusTypeDef ADC_Measurement_ReadRaw(uint32_t *raw_value)
{
	return ADC_Measurement_ReadInjectedRaw(raw_value);
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

HAL_StatusTypeDef ADC_Measurement_StartDMA(uint16_t *buffer, uint32_t sample_count)
{
	if((adc_handle == NULL) || (buffer == NULL) || sample_count == 0U){

		return HAL_ERROR;
	}

	return HAL_ADC_Start_DMA(adc_handle, (uint32_t *)buffer, sample_count);
}

HAL_StatusTypeDef ADC_Measurement_StopDMA(void)
{
	if(adc_handle == NULL){

		return HAL_ERROR;
	}

	return HAL_ADC_Stop_DMA(adc_handle);
}

bool ADC_Measurement_IsHandle(ADC_HandleTypeDef *hadc)
{
	if((adc_handle == NULL) || (hadc == NULL)){

		return false;
	}

	if(hadc->Instance == adc_handle->Instance){

		return true;
	}

	return false;
}
