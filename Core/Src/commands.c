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

#include <string.h>
#include <stdio.h>

typedef void (*CommandHandler)(void);

typedef struct
{
	const char *name;
	const char *description;
	CommandHandler handler;

}CommandEntry;

static void Command_Help(void);
static void Command_Status(void);
static void Command_Version(void);
static void Command_LED_On(void);
static void Command_LED_Off(void);
static void Command_LED_Toggle(void);
static void Command_ADC_Raw(void);
static void Command_ADC_Voltage(void);
static void Command_ADC_Stats(void);

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
		{"adc stats", "Show ADC sample statictis", Command_ADC_Stats}
};

#define COMMAND_COUNT (sizeof(command_table)/sizeof(command_table[0]))

void Commands_Execute(const char *line)
{

	for(size_t i = 0U; i < COMMAND_COUNT; i++){

		if(strcmp(command_table[i].name, line) == 0U){

			command_table[i].handler();
			return;
		}
	}

	Console_Write("\r\nUnknown command. Type 'help'.");
}

static void Command_Help(void)
{
	Console_Write("\r\nAvailable commands:");

	for(size_t i = 0U; i < COMMAND_COUNT; i++){

		Console_Write("\r\n ");
		Console_Write(command_table[i].name);
		Console_Write(": ");
		Console_Write(command_table[i].description);

	}
}

static void Command_Status(void)
{
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

static void Command_Version(void)
{
	Console_Write("\r\nFirmware version:");
	Console_Write("\r\n Name: STM32 Instrument Console");
	Console_Write("\r\n Version: 0.1.0");
	Console_Write("\r\n Target: STM32F446RE");
}

static void Command_LED_On(void)
{
	LED_On();
	Console_Write("\r\n LED turned ON");
}

static void Command_LED_Off(void)
{
	LED_Off();
	Console_Write("\r\n LED turned OFF");
}

static void Command_LED_Toggle(void)
{
	LED_Toggle();
	Console_Write("\r\n LED toggled");
}

static void Command_ADC_Raw(void)
{
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

static void Command_ADC_Voltage(void)
{
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

static void Command_ADC_Stats(void)
{
	ADC_MeasurementStats stats;
	HAL_StatusTypeDef status;
	char response[160];

	float minimum_voltage;
	float maximum_voltage;
	float average_voltage;
	float peak_to_peak_voltage;

	status = ADC_Measurement_ReadStats(100U, &stats);


	if(status == HAL_OK){

		minimum_voltage = ADC_Measurement_RawToVoltage(stats.minimum);
		maximum_voltage = ADC_Measurement_RawToVoltage(stats.maximum);
		average_voltage = ADC_Measurement_RawToVoltage(stats.average);
		peak_to_peak_voltage = ADC_Measurement_RawToVoltage(stats.peak_to_peak);

		snprintf(
				response,
				sizeof(response),
				"\r\nADC statistics:"
				"\r\n Samples: %lu"
				"\r\n Minimum: %lu  (%.3lf V)"
				"\r\n Maximum: %lu  (%.3lf V)"
				"\r\n Average: %lu  (%.3lf V)"
				"\r\n Peak-to-peak: %lu  (%.3lf V)",
				(unsigned long)stats.sample_count,
				(unsigned long)stats.minimum, (double)minimum_voltage,
				(unsigned long)stats.maximum, (double)maximum_voltage,
				(unsigned long)stats.average, (double)average_voltage,
				(unsigned long)stats.peak_to_peak, (double)peak_to_peak_voltage
		);

		Console_Write(response);
	}
	else{

		Console_Write("\r\nADC read error");
	}
}
