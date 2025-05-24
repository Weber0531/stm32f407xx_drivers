/*
 * 011I2C_Master_Rx_TestingIT.c
 *
 *  Created on: May 22, 2025
 *      Author: weber
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

//extern void initialise_monitor_handles();

// some flag
uint8_t rxComplt = RESET;

#define MY_ADDR 	0x61
#define SLAVE_ADDR 	0x68

I2C_Handle_t I2C1Handle;

// rcv_buffer
uint8_t rcv_buf[32];

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

void I2C1_Inits(void){
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_EN;

	I2C_Init(&I2C1Handle);
}

void GPIO_ButtonInit(void){
	GPIO_Handle_t GPIOBtn;

	// This is btn GPIO configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&GPIOBtn);
}

int main(void){

	uint8_t commandcode;
	uint8_t len;

	//initialise_monitor_handles();

	//printf("Application is running\n");

	GPIO_ButtonInit();

	// I2C pin inits
	I2C1_GPIOInits();

	// I2C peripheral configuration
	I2C1_Inits();

	// I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	// Enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	// ACK bit is made 1 after PE=1
	I2C_ManageAcking(I2C1, ENABLE);

	while(1){
		// wait till button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = 0x51;

		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		commandcode = 0x52;

		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR) != I2C_READY);

		while(I2C_MasterReceiveDataIT(&I2C1Handle, rcv_buf, len, SLAVE_ADDR, I2C_DISABLE_SR) != I2C_READY);

		rxComplt = RESET;
		while(rxComplt != SET);

		rcv_buf[len+1] = '\0';

		//printf("Data : %s",rcv_buf);

		rxComplt = RESET;
	}

	return 0;
}


void I2C1_EV_IRQHandler (void){
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler (void){
	I2C_ER_IRQHandling(&I2C1Handle);
}


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv){
	if(AppEv == I2C_EV_TX_CMPLT) {
		printf("Tx is completed\n");
	} else if(AppEv == I2C_EV_RX_CMPLT) {
		printf("Rx is completed\n");
		rxComplt = SET;
	} else if(AppEv == I2C_ERROR_AF) {
		// in master ACK failure happens when slave fails to send ACK for the byte sent from the master

		printf("Error: ACK failure\n");

		I2C_CloseSendData(pI2CHandle);

		// Generate the stop condition to release the bus
		I2C_GenerateStopCondition(I2C1);

		// Hang in infinite loop
		while(1);
	}
}










