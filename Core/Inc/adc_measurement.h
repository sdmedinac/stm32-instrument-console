/*
 * adc_measurement.h
 *
 *  Created on: Aug 18, 2026
 *      Author: sdmedinac
 */

#ifndef INC_ADC_MEASUREMENT_H_
#define INC_ADC_MEASUREMENT_H_

#include "stm32f4xx_hal.h"

void ADC_Measurement_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef ADC_Measurement_ReadRaw(uint32_t *raw_value);
HAL_StatusTypeDef ADC_Measurement_ReadVoltage(float *voltage);

#endif /* INC_ADC_MEASUREMENT_H_ */
