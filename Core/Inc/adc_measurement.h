/*
 * adc_measurement.h
 *
 *  Created on: Aug 18, 2026
 *      Author: sdmedinac
 */

#ifndef INC_ADC_MEASUREMENT_H_
#define INC_ADC_MEASUREMENT_H_

#include "stm32f4xx_hal.h"


typedef struct
{
	uint32_t minimum;
	uint32_t maximum;
	uint32_t average;
	uint32_t peak_to_peak;
	uint32_t sample_count;

} ADC_MeasurementStats;

void ADC_Measurement_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef ADC_Measurement_ReadRaw(uint32_t *raw_value);
HAL_StatusTypeDef ADC_Measurement_ReadVoltage(float *voltage);
HAL_StatusTypeDef ADC_Measurement_ReadStats(uint32_t sample_count, ADC_MeasurementStats *stats);
float ADC_Measurement_RawToVoltage(uint32_t raw_voltage);

#endif /* INC_ADC_MEASUREMENT_H_ */
