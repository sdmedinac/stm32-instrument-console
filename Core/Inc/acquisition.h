/*
 * acquisition.h
 *
 *  Created on: Aug 27, 2026
 *      Author: sdmedinac
 */

#ifndef INC_ACQUISITION_H_
#define INC_ACQUISITION_H_

#include "stm32f4xx_hal.h"
#include "adc_measurement.h"

typedef enum
{
	ACQUISITION_STATE_IDLE = 0,
	ACQUISITION_STATE_RUNNING,
	ACQUISITION_STATE_COMPLETE,
	ACQUISITION_STATE_ERROR

} AcquisitionState;

typedef enum
{
	ACQUISITION_ERROR_NONE = 0,
	ACQUISITION_ERROR_ADC_DMA_START,
	ACQUISITION_ERROR_TIMER_START,
	ACQUISITION_ERROR_TIMER_STOP,
	ACQUISITION_ERROR_ADC_DMA_STOP,
	ACQUISITION_ERROR_ADC_DMA,
	ACQUISITION_ERROR_TIMEOUT

} AcquisitionError;

void Acquisition_Init(void);
void Acquisition_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void Acquisition_ErrorCallback(ADC_HandleTypeDef *hadc);
void Acquisition_Process(void);
AcquisitionError Acquisition_GetError(void);
AcquisitionState Acquisition_GetState(void);
uint16_t Acquisition_GetSampleCount(void);
uint16_t Acquisition_GetConfiguredSampleCount(void);
uint16_t Acquisition_GetBufferCapacity(void);
uint32_t Acquisition_GetMissedEventCount(void);
uint32_t Acquisition_GetCapturedSamplingFrequency(void);
HAL_StatusTypeDef Acquisition_Start(void);
HAL_StatusTypeDef Acquisition_GetStats(ADC_MeasurementStats *stats);
HAL_StatusTypeDef Acquisition_Stop(void);
HAL_StatusTypeDef Acquisition_SetSampleCount(uint16_t sample_count);
HAL_StatusTypeDef Acquisition_GetSample(uint16_t index, uint16_t *sample);

#endif /* INC_ACQUISITION_H_ */
