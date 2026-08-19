/*
 * led.c
 *
 *  Created on: Aug 17, 2026
 *      Author: sdmedinac
 */

#include "led.h"

static GPIO_TypeDef *led_port;
static  uint16_t led_pin;

void LED_Init(GPIO_TypeDef *port, uint16_t pin)
{
	led_port = port;
	led_pin = pin;

	HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_RESET);
}

void LED_On(void)
{
	HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
}

void LED_Off(void)
{
	HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_RESET);
}

void LED_Toggle(void)
{
	HAL_GPIO_TogglePin(led_port, led_pin);
}

bool LED_IsOn(void)
{

	if(HAL_GPIO_ReadPin(led_port, led_pin) == GPIO_PIN_SET){

		return true;
	}
	else{

		return false;
	}
}

