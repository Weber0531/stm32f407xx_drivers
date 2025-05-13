/*
 * 003Led_Button_EXT.c
 *
 *  Created on: May 1, 2025
 *      Author: weber
 *      brief: write a program to connect external button to PB12 and external LED to PA14.
 *      	   Toggle the LED whenever the external button is pressed
 */

#include <string.h>
#include "stm32f407xx.h"

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW

void delay(void){
	// this will introduce ~200ms delay when system clock is 16MHz
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

int main(void){

	GPIO_Handle_t GpioLed, GpioBtn;
	memset(&GpioLed, 0, sizeof(GpioLed));
	memset(&GpioBtn, 0, sizeof(GpioBtn));

	// this is led gpio configuration
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);

	GPIO_Init(&GpioLed);



	// this is btn gpio configuration
	GpioBtn.pGPIOx = GPIOD;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);

	GPIO_Init(&GpioBtn);



	// IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI9_5, ENABLE);

	while(1);


	return 0;
}


void EXTI9_5_IRQHandler(void){
	delay(); // 200ms . wait till button de-bouncing gets over
	GPIO_IRQHandling(GPIO_PIN_NO_5); // Clear the pending event from EXTI line
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
}
