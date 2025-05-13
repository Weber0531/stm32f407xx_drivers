/*
 * 009I2C_Master_TX_Testing.c
 *
 *  Created on: May 13, 2025
 *      Author: weber
 */


#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"


void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

/*
 * PB6 --> SCL
 * PB9 --> SDA
 */
void I2C1_GPIOInits(void){
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
}

void I2C1_Inits(void){

}

void GPIO_ButtonInit(void){
	GPIO_Handle_t GPIOBtn;

	// This is btn GPIO configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);
}


int main(void){

	return 0;
}
