/*
 * 012I2C_Slave_Tx_String.c
 *
 *  Created on: May 22, 2025
 *      Author: weber
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

#define SLAVE_ADDR 	0x68
#define MY_ADDR 	SLAVE_ADDR

I2C_Handle_t I2C1Handle;

// Tx_buffer
uint8_t Tx_buf[32] = "STM32 slave mode testing..";

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

	GPIO_ButtonInit();

	// I2C pin inits
	I2C1_GPIOInits();

	// I2C peripheral configuration
	I2C1_Inits();

	// Enable the I2C peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	// ACK bit is made 1 after PE=1
	I2C_ManageAcking(I2C1, ENABLE);

	// I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	while(1);

	return 0;
}


void I2C1_EV_IRQHandler (void){
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler (void){
	I2C_ER_IRQHandling(&I2C1Handle);
}


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv){
	static uint8_t commandCode = 0;
	static uint8_t Cnt = 0;

	if(AppEv == I2C_EV_DATA_REQ){
		// Master wants some data. slave has to send it
		if(commandCode == 0x51) {
			// send the length information to the master
			I2C_SlaveSendData(pI2CHandle->pI2Cx, strlen((char*)Tx_buf));
		} else if(commandCode == 0x52) {
			// Send the contents of Tx_buf
			I2C_SlaveSendData(pI2CHandle->pI2Cx, Tx_buf[Cnt++]);
		}
	} else if(AppEv == I2C_EV_DATA_RCV) {
		//Data is waiting for the slave to read . slave has to read it
		commandCode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);
	} else if(AppEv == I2C_ERROR_AF) {
		// This happens only during slave transmission
		// Master has sent the NACK. so slave should understand that master doesn't need more data
		commandCode = 0xff;
		Cnt = 0;
	} else if(AppEv == I2C_EV_STOP) {
		// This happens only during slave reception
		// Master has ended the I2C communication with the slave
	}
}










