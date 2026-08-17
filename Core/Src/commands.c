/*
 * commands.c
 *
 *  Created on: Aug 17, 2026
 *      Author: sdmedinac
 */

#include "commands.h"
#include "console.h"

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

static const CommandEntry
command_table[] = {

		{"help", "Show available commands", Command_Help},
		{"status", "Show system status", Command_Status},
		{"version", "Show firmware version", Command_Version}
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
}

static void Command_Version(void)
{
	Console_Write("\r\nFirmware version:");
	Console_Write("\r\n Name: STM32 Instrument Console");
	Console_Write("\r\n Version: 0.1.0");
	Console_Write("\r\n Target: STM32F446RE");
}
