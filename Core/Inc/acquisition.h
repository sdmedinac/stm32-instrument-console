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

void Acquisition_Init(void);
void Acquisition_Process(void);
AcquisitionState Acquisition_GetState(void);
uint16_t Acquisition_GetSampleCount(void);
uint32_t Acquisition_GetMissedEventCount(void);
HAL_StatusTypeDef Acquisition_Start(void);
HAL_StatusTypeDef Acquisition_GetStats(ADC_MeasurementStats *stats);

#endif /* INC_ACQUISITION_H_ */
