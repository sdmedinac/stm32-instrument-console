/*
 * commands.c
 *
 *  Created on: Aug 17, 2026
 *      Author: sdmedinac
 */

#include "commands.h"
#include "console.h"
#include "led.h"

#include <string.h>

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

static const CommandEntry
command_table[] = {

		{"help", "Show available commands", Command_Help},
		{"status", "Show system status", Command_Status},
		{"version", "Show firmware version", Command_Version},
		{"led on", "Turn the user LED on", Command_LED_On},
		{"led off", "Turn the user LED off", Command_LED_Off},
		{"led toggle", "Toggle the user LED", Command_LED_Toggle}
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
