/*
 * 007SPI_cmd_Handling.c
 *
 *  Created on: May 9, 2025
 *      Author: weber
 */

#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

// 沒有 USB logic analyzer 時，用 semi hosting 看slave response，搭配printf
//extern void initialise_monitor_handles();

// command codes
#define COMMAND_LED_CTRL          	0x50
#define COMMAND_SENSOR_READ       	0x51
#define COMMAND_LED_READ          	0x52
#define COMMAND_PRINT				0x53
#define COMMAND_ID_READ         	0x54

#define LED_ON		1
#define LED_OFF 	0

// Arduino analog pins
#define ANALOG_PIN0		0
#define ANALOG_PIN1		1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4

// Arduino LED
#define LED_PIN 	9


/*
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 -> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode : 5
 */


void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
}

void SPI_GPIOInits(void){

	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);

	// NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);
}

void SPI2_Inits(){

	SPI_Handle_t SPI2handle;

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI; // Hardware slave management enabled for NSS pin

	SPI_Init(&SPI2handle);
}


void GPIO_ButtonInit(void){

	// this is btn gpio configuration
	GPIO_Handle_t GpioBtn;

	GpioBtn.pGPIOx = GPIOA;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GpioBtn);
}


uint8_t SPI_VerifyResponse(uint8_t ackbyte){
	if(ackbyte == 0xf5) {
		// ACK
		return 1;
	}
	return 0;
}


int main(void){

	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

	// 沒有 USB logic analyzer 時，用 semi hosting 看slave response，搭配printf
	//initialise_monitor_handles();

	printf("Application is running\n");

	// This function is used to initialize the GPIO button
	GPIO_ButtonInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI_GPIOInits();

	// This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	printf("SPI Init. done\n");

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1){
		// wait till button is pressed
		while(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		// Enable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);

		/***********************************************************************
		 * 			1. COMMAND_LED_CTRL		<pin no(1)>		<value(1)>
		 ***********************************************************************/
		uint8_t commandcode = COMMAND_LED_CTRL;
		uint8_t ackbyte;
		uint8_t args[2];

		// Send command
		SPI_SendData(SPI2, &commandcode, 1);

		// Do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		// Read the ACK byte received
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte)) {
			args[0] = LED_PIN;
			args[1] = LED_ON;
			// Send arguments
			SPI_SendData(SPI2, args, 2);
			// dummy read
			SPI_ReceiveData(SPI2,args,2);
			printf("COMMAND_LED_CTRL Executed\n");
		}



		/***********************************************************************
		 * 			2. COMMAND_SENSOR_READ   <analog pin number(1) >
		 ***********************************************************************/
		// wait till button is pressed
		while(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_SENSOR_READ;

		// Send command
		SPI_SendData(SPI2, &commandcode, 1);

		// Do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		// Read the ACK byte received
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte)) {
			args[0] = ANALOG_PIN0;
			// Send arguments
			SPI_SendData(SPI2, args, 1);

			// Do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			// insert some delay so that slave can ready with the data
			delay();

			// Send some dummy bits (1 byte) to fetch the response from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;
			// Read the sensor analog value received
			SPI_ReceiveData(SPI2, &analog_read, 1);
			printf("COMMAND_SENSOR_READ		 %d\n",analog_read);
		}
		//end of COMMAND_SENSOR_READ



		/***********************************************************************
		 * 				3.  COMMAND_LED_READ 	 <pin no(1) >
		 ***********************************************************************/
		// wait till button is pressed
		while(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_LED_READ;

		// Send command
		SPI_SendData(SPI2, &commandcode, 1);

		// Do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		// Read the ACK byte received
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		if(SPI_VerifyResponse(ackbyte)) {
			args[0] = LED_PIN;
			// Send arguments
			SPI_SendData(SPI2, args, 1);

			// Do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			// insert some delay so that slave can ready with the data
			delay();

			// Send some dummy bits (1 byte) to fetch the response from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t LED_status;
			// Read the LED status value received
			SPI_ReceiveData(SPI2, &LED_status, 1);
			printf("COMMAND_LED_READ	%d", LED_status);
		}



		/***********************************************************************
		 * 				4. COMMAND_PRINT 		<len(2)>  <message(len) >
		 ***********************************************************************/
		// wait till button is pressed
		while(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_PRINT;

		// Send command
		SPI_SendData(SPI2, &commandcode, 1);

		// Do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		// Read the ACK byte received
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		uint8_t message[] = "Hello ! How are you ??";
		if(SPI_VerifyResponse(ackbyte)) {
			args[0] = strlen((char*)message);
			// Send arguments
			SPI_SendData(SPI2, args, 1); // Send length

			// Do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			delay();

			// Send message
			for(int i = 0; i < args[0]; i++){
				SPI_SendData(SPI2, &message[i], 1);
				// Do dummy read to clear off the RXNE
				SPI_ReceiveData(SPI2, &dummy_read, 1);
			}

			printf("COMMAND_PRINT Executed \n");
		}




		/***********************************************************************
		 * 						5. COMMAND_ID_READ
		 ***********************************************************************/
		// wait till button is pressed
		while(GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = COMMAND_ID_READ;

		// Send command
		SPI_SendData(SPI2, &commandcode, 1);

		// Do dummy read to clear off the RXNE
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits (1 byte) to fetch the response from the slave
		SPI_SendData(SPI2, &dummy_write, 1);

		// Read the ACK byte received
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		uint8_t id[11];
		if(SPI_VerifyResponse(ackbyte)) {
			// read 10 bytes id from the slave
			for(int i = 0; i < 10; i++){
				// send dummy byte to fetch data from slave
				SPI_SendData(SPI2, &dummy_write, 1);
				SPI_ReceiveData(SPI2, &id[i], 1);
			}

			id[10] = '\0';

			printf("COMMAND_ID_READ 	%s \n", id);
		}



		// Confirm SPI is not busy
		while(SPI_GetFlagStatus(SPI2, SPI_BSY_FLAG));

		// Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("SPI Communication Closed\n");
	}

	return 0;
}
