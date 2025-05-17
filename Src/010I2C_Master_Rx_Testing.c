/*
 * 010I2C_Master_Rx_Testing.c
 *
 *  Created on: May 18, 2025
 *      Author: weber
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

#define MY_ADDR 	0x61
#define SLAVE_ADDR 	0x68

I2C_Handle_t I2C1Handle;

void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

/*
 * PB6 --> SCL
 * PB7 --> SDA
 */
void I2C1_GPIOInits(void){
	GPIO_Handle_t I2CPins;

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	// SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2CPins);

	// SDA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2CPins);
}

int main(void){


	return 0;
}
