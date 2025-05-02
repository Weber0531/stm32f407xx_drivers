/*
 * 002Led_Button.c
 *
 *  Created on: May 1, 2025
 *      Author: weber
 *      brief: write a program to connect external button to PB12 and external LED to PA14.
 *      	   Toggle the LED whenever the external button is pressed
 */


#include "stm32f407xx.h"

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW

void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

int main(void){

	GPIO_Handle_t GpioLed, GpioBtn;

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
	GpioBtn.pGPIOx = GPIOB;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;

	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&GpioBtn);

	while(1){
		if(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12) == BTN_PRESSED){
			delay(); // de-bouncing
			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
		}

	}

	return 0;
}
