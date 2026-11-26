/*
 * signal_characterization.h
 *
 *  Created on: Oct 6, 2026
 *      Author: sdmedinac
 */

#ifndef INC_SIGNAL_CHARACTERIZATION_H_
#define INC_SIGNAL_CHARACTERIZATION_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint32_t baseline;
	float rms_noise;

	uint16_t peak_value;
	uint16_t peak_index;

	uint32_t peak_time_us;
	uint32_t amplitude;

	bool lower_saturation;
	bool upper_saturation;

} SignalCharacterizationResult;

HAL_StatusTypeDef SignalCharacterization_Analyze(
		const uint16_t *samples,
		uint16_t sample_count,
		uint32_t sampling_frequency_hz,
		uint16_t baseline_start,
		uint16_t baseline_count,
		SignalCharacterizationResult *result
);


#endif /* INC_SIGNAL_CHARACTERIZATION_H_ */
