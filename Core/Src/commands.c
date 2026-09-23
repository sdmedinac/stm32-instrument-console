/*
 * commands.c
 *
 *  Created on: Aug 17, 2026
 *      Author: sdmedinac
 */

#include "commands.h"
#include "console.h"
#include "led.h"
#include "adc_measurement.h"
#include "sampling_timer.h"
#include "acquisition.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*CommandHandler)(const char *args);

typedef struct
{
	const char *name;
	const char *description;
	CommandHandler handler;

}CommandEntry;

static void Command_Help(const char *args);
static void Command_Status(const char *args);
static void Command_Version(const char *args);
static void Command_LED_On(const char *args);
static void Command_LED_Off(const char *args);
static void Command_LED_Toggle(const char *args);
static void Command_ADC_Raw(const char *args);
static void Command_ADC_Voltage(const char *args);
static void Command_Acquisition_Start(const char *args);
static void Command_Acquisition_Status(const char *args);
static void Command_Acquisition_Result(const char *args);
static void Command_Acquisition_Stop(const char *args);
static void Command_Acquisition_Rate(const char *args);

static const CommandEntry
command_table[] = {

		{"help", "Show available commands", Command_Help},
		{"status", "Show system status", Command_Status},
		{"version", "Show firmware version", Command_Version},
		{"led on", "Turn the user LED on", Command_LED_On},
		{"led off", "Turn the user LED off", Command_LED_Off},
		{"led toggle", "Toggle the user LED", Command_LED_Toggle},
		{"adc raw", "Read the raw ADC value", Command_ADC_Raw},
		{"adc voltage", "Read the ADC voltage value", Command_ADC_Voltage},
		{"acq start", "Start a periodic ADC acquisition", Command_Acquisition_Start},
		{"acq status", "Show acquisition status", Command_Acquisition_Status},
		{"acq result", "Show the latest acquisition result", Command_Acquisition_Result},
		{"acq stop", "Stop the active acquisition", Command_Acquisition_Stop},
		{"acq rate", "Set sampling rate in hz", Command_Acquisition_Rate}
};

#define COMMAND_COUNT (sizeof(command_table)/sizeof(command_table[0]))

void Commands_Execute(const char *line)
{
	if(line == NULL){

		Console_Write("\r\nInvalid command line");
		return;
	}

	for(size_t i = 0U; i < COMMAND_COUNT; i++){

		size_t command_length = strlen(command_table[i].name);

		if(strncmp(line, command_table[i].name, command_length) != 0){

			continue; // Pasa a la siguiente iteracion
		}

		if(line[command_length] != '\0' && line[command_length] != ' '){

			continue;
		}

		const char *args;

		args = &line[command_length]; // Apunta al primer caracter despues del nombre del comando

		while(*args == ' '){

			args++; // si args es un espacio apunta al siguiente caracter
		}

		command_table[i].handler(args);
		return;
	}

	Console_Write("\r\nUnknown command. Type 'help'.");
}

static void Command_Help(const char *args)
{
	(void)args;

	Console_Write("\r\nAvailable commands:");

	for(size_t i = 0U; i < COMMAND_COUNT; i++){

		Console_Write("\r\n ");
		Console_Write(command_table[i].name);
		Console_Write(": ");
		Console_Write(command_table[i].description);

	}
}

static void Command_Status(const char *args)
{
	(void)args;

	Console_Write("\r\nSystem Status:");
    Console_Write("\r\n Console: Ready");
    Console_Write("\r\n UART: USART2");
    Console_Write("\r\n Baud rate: 115200");
    Console_Write("\r\n Input limit: 63 characters");

    if(LED_IsOn()){

    	Console_Write("\r\n LED: ON");
    }
    else{

    	Console_Write("\r\n LED: OFF");
    }
}

static void Command_Version(const char *args)
{
	(void)args;

	Console_Write("\r\nFirmware version:");
	Console_Write("\r\n Name: STM32 Instrument Console");
	Console_Write("\r\n Version: 0.1.0");
	Console_Write("\r\n Target: STM32F446RE");
}

static void Command_LED_On(const char *args)
{
	(void)args;

	LED_On();
	Console_Write("\r\n LED turned ON");
}

static void Command_LED_Off(const char *args)
{
	(void)args;

	LED_Off();
	Console_Write("\r\n LED turned OFF");
}

static void Command_LED_Toggle(const char *args)
{
	(void)args;

	LED_Toggle();
	Console_Write("\r\n LED toggled");
}

static void Command_ADC_Raw(const char *args)
{
	(void)args;

	uint32_t raw_value;
	HAL_StatusTypeDef status;
	char response[32];

	status = ADC_Measurement_ReadRaw(&raw_value);

	if(status == HAL_OK){

		snprintf(response, sizeof(response), "\r\nADC raw: %lu", (unsigned long)raw_value);
		Console_Write(response);
	}
	else{

		Console_Write("\r\nADC read error");
	}

}

static void Command_ADC_Voltage(const char *args)
{
	(void)args;

	float measured_voltage;
	HAL_StatusTypeDef status;
	char response[32];

	status = ADC_Measurement_ReadVoltage(&measured_voltage);

	if(status == HAL_OK){

		snprintf(response, sizeof(response), "\r\n ADC voltage: %.3lf V", (double)measured_voltage);
		Console_Write(response);
	}
	else{

		Console_Write("\r\nADC read error");
	}
}

static void Command_Acquisition_Start(const char *args)
{
	(void)args;

	HAL_StatusTypeDef status = Acquisition_Start();

	if(status == HAL_OK){

		Console_Write("\r\nAcquisition started");
	}
	else if(status == HAL_BUSY){

		Console_Write("\r\nAcquisition already running");
	}
	else{

		Console_Write("\r\nAcquisition start error");
	}
}

static void Command_Acquisition_Status(const char *args)
{
	(void)args;

	AcquisitionState state;
	AcquisitionError error;
	uint16_t sample_count;
	uint32_t sampling_frequency_hz;
	char response[160];
	const char *state_text;
	const char *error_text;

	error = Acquisition_GetError();
	state = Acquisition_GetState();
	sample_count = Acquisition_GetSampleCount();
	sampling_frequency_hz = SamplingTimer_GetFrequency();

	switch(state)
	{
	case ACQUISITION_STATE_IDLE:
		state_text = "IDLE";
		break;

	case ACQUISITION_STATE_RUNNING:
		state_text = "RUNNING";
		break;

	case ACQUISITION_STATE_COMPLETE:
		state_text = "COMPLETE";
		break;

	case ACQUISITION_STATE_ERROR:
		state_text = "ERROR";
		break;

	default:
		state_text = "UNKNOWN";
		break;
	}

	switch(error)
	{
	case ACQUISITION_ERROR_NONE:
		error_text = "NONE";
		break;

	case ACQUISITION_ERROR_ADC_DMA_START:
		error_text = "ADC_DMA_START";
		break;

	case ACQUISITION_ERROR_TIMER_START:
		error_text = "TIMER_START";
		break;

	case ACQUISITION_ERROR_TIMER_STOP:
			error_text = "TIMER_STOP";
			break;

	case ACQUISITION_ERROR_ADC_DMA_STOP:
			error_text = "ADC_DMA_STOP";
			break;

	case ACQUISITION_ERROR_ADC_DMA:
			error_text = "ADC_DMA";
			break;

	case ACQUISITION_ERROR_TIMEOUT:
			error_text = "TIMEOUT";
			break;

	default:
		error_text = "UNKNOWN";
		break;
	}

	snprintf(
			response,
			sizeof(response),
			"\r\nAcquisition status:"
			"\r\n State: %s"
			"\r\n Samples: %u"
			"\r\n Error: %s"
			"\r\n Transfer mode: DMA"
			"\r\n Sampling rate: %lu Hz",
			state_text,
			(unsigned int)sample_count,
			error_text,
			(unsigned long)sampling_frequency_hz
	);

	Console_Write(response);
}


static void Command_Acquisition_Result(const char *args)
{
	(void)args;

	ADC_MeasurementStats stats;
	HAL_StatusTypeDef status;
	char response[256];

	float minimum_voltage;
	float maximum_voltage;
	float average_voltage;
	float peak_to_peak_voltage;

	status = Acquisition_GetStats(&stats);

	if(status == HAL_OK){

		minimum_voltage = ADC_Measurement_RawToVoltage(stats.minimum);
		maximum_voltage = ADC_Measurement_RawToVoltage(stats.maximum);
		average_voltage = ADC_Measurement_RawToVoltage(stats.average);
		peak_to_peak_voltage = ADC_Measurement_RawToVoltage(stats.peak_to_peak);

		snprintf(
				response,
				sizeof(response),
				"\r\nAcquisition result:"
				"\r\n Samples: %lu"
				"\r\n minimum: %lu (%.3lf V)"
				"\r\n maximum: %lu (%.3lf V)"
				"\r\n average: %lu (%.3lf V)"
				"\r\n peak-to-peak: %lu (%.3lf V)",
				(unsigned long)stats.sample_count,
				(unsigned long)stats.minimum, (double)minimum_voltage,
				(unsigned long)stats.maximum, (double)maximum_voltage,
				(unsigned long)stats.average, (double)average_voltage,
				(unsigned long)stats.peak_to_peak, (double)peak_to_peak_voltage
		);

		Console_Write(response);
	}
	else if(status == HAL_BUSY){

		Console_Write("\r\nNo completed acquisition is available");
	}
	else{

		Console_Write("\r\nAcquisition result error");
	}
}

static void Command_Acquisition_Stop(const char *args)
{
	(void)args;

	AcquisitionState state;
	HAL_StatusTypeDef status;

	state = Acquisition_GetState();

	if(state == ACQUISITION_STATE_IDLE){

		Console_Write("\r\nAcquisition is already idle");
		return;
	}
	else if(state == ACQUISITION_STATE_COMPLETE){

		Console_Write("\r\nAcquisition already completed");
		return;
	}
	else if(state == ACQUISITION_STATE_ERROR){

		Console_Write("\r\nAcquisition is in error state");
		return;
	}

	status = Acquisition_Stop();

	if(status == HAL_OK){

		Console_Write("\r\nAcquisition stopped");
	}
	else{

		Console_Write("\r\nAcquisition stop error");
	}
}

static void Command_Acquisition_Rate(const char *args)
{
	unsigned long requested_frequency;
	char *end_pointer;
	HAL_StatusTypeDef status;
    uint32_t actual_frequency;
	char response[128];

	if(*args == '\0'){

		Console_Write("\r\nUsage: acq rate <10-10000hz>");
		return;
	}

	requested_frequency = strtoul(args, &end_pointer, 10);

	if((end_pointer == args) || (*end_pointer != '\0')){

		Console_Write("\r\nInvalid sampling rate");
		return;
	}

	if(requested_frequency > UINT32_MAX){

		Console_Write("\r\nSampling rate is too large");
		return;
	}

	status = SamplingTimer_SetFrequency((uint32_t)requested_frequency);

	if(status == HAL_BUSY){

		Console_Write("\r\nCannot change sampling rate while acquisition is running");
		return;
	}

	if(status == HAL_ERROR){

		Console_Write("\r\nSampling rate out of range");
		Console_Write("\r\nValid range: <10-10000 Hz>");
		return;
	}

	if(status != HAL_OK){

		Console_Write("\r\nSampling rate update failed");
		return;
	}

	actual_frequency = SamplingTimer_GetFrequency();

	snprintf(
			response,
			sizeof(response),
			"\r\nSampling rate updated:"
			"\r\n Requested: %lu Hz"
			"\r\n Actual: %lu Hz",
			requested_frequency,
			(unsigned long)actual_frequency
	);

	Console_Write(response);
}
