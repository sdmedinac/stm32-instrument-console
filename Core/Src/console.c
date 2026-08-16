/*
 * console.c
 *
 *  Created on: Aug 15, 2026
 *      Author: sdmedinac
 */

#include "console.h"

#define RX_BUFFER_SIZE 64U

static UART_HandleTypeDef *console_uart;

static const char console_prompt[] = "\r\n> ";

static uint8_t rx_byte;
static volatile uint8_t rx_ready = 0U;

static uint8_t line_ready = 0U;
static uint8_t input_overflow = 0U;

static char rx_buffer[RX_BUFFER_SIZE];
static uint16_t rx_index;


static void Console_WriteString(const char *text)
{
	uint16_t lenght = 0U;

	while(text[lenght] != '\0'){

		lenght++;
	}

	HAL_UART_Transmit(console_uart, (uint8_t *)text, lenght, HAL_MAX_DELAY);
}

void Console_Init(UART_HandleTypeDef *huart)
{
	console_uart = huart;

	rx_byte = 0U;
	rx_ready  = 0U;
	line_ready = 0U;
	input_overflow = 0U;
	rx_index = 0U;

	Console_WriteString("\r\nSTM32 Instrument Console");
	Console_WriteString(console_prompt);

	HAL_UART_Receive_IT(console_uart, &rx_byte, 1U);

}

void Console_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if((huart != NULL) && (console_uart != NULL) && (huart->Instance == console_uart->Instance)){

		rx_ready = 1U;
	}
}

void Console_Process(void)
{
	if(rx_ready == 1U){

		uint8_t current_byte = rx_byte;

		rx_ready = 0U;

		HAL_UART_Receive_IT(console_uart, &rx_byte, 1U);

		if((current_byte == '\b') || (current_byte == 127U)){

			if((input_overflow == 0U) && (rx_index > 0U)){

				rx_index--;
				Console_WriteString("\b \b");
			}
		}
		else if((current_byte == '\r') || (current_byte == '\n')){

			if(input_overflow == 1U){

				rx_index = 0U;
				input_overflow = 0U;
				Console_WriteString(console_prompt);
			}
			else if(rx_index > 0U){

				rx_buffer[rx_index] = '\0';
				line_ready = 1U;
			}
		}
		else{

			if(input_overflow == 0U){

				if(rx_index < (RX_BUFFER_SIZE - 1U)){

					rx_buffer[rx_index] = (char)current_byte;
					rx_index++;

					HAL_UART_Transmit(console_uart, &current_byte, 1U, HAL_MAX_DELAY);
				}
				else{

					input_overflow = 1U;
					Console_WriteString("\r\nInput limit reached. Press Enter to discard.");
				}
			}
		}
	}

	if(line_ready == 1U){

		line_ready = 0U;

		Console_WriteString("\r\nReceived: ");
		HAL_UART_Transmit(console_uart, (uint8_t *)rx_buffer, rx_index, HAL_MAX_DELAY);
		Console_WriteString(console_prompt);

		rx_index = 0U;
	}
}




