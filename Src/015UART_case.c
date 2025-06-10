/*
 * 015UART_case.c
 *
 *  Created on: May 27, 2025
 *      Author: weber
 */


#include <stdio.h>
#include <string.h>
#include "stm32f407xx.h"

// we have 3 different messages that we transmit to arduino
char *msg[3] = {"hihihihihihi123", "Hello How are you ?" , "Today is Monday !"};

// reply from arduino will be stored here
char rx_buf[1024] ;

USART_Handle_t usart2_handle;

// This flag indicates reception completion
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;

void USART2_GPIOInit(void){
	GPIO_Handle_t usart_gpios;

	usart_gpios.pGPIOx = GPIOA;

	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
	usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// USART2 TX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	GPIO_Init(&usart_gpios);

	// USART2 RX
	usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&usart_gpios);
}


void USART2_Init(void){
	usart2_handle.pUSARTx = USART2;

	usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&usart2_handle);
}


void GPIO_ButtonInit(void){
	GPIO_Handle_t GPIOBtn;

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&GPIOBtn);
}

void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
}


int main(void){
	uint32_t cnt = 0;

	USART2_GPIOInit();

	USART2_Init();

	USART_IRQInterruptConfig(IRQ_NO_USART2, ENABLE);
	USART_IRQPriorityConfig(IRQ_NO_USART2, NVIC_IRQ_PRI15);

	USART_PeripheralControl(USART2, ENABLE);

	printf("Application is running\n");

	while(1){
		//wait till button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		// Next message index ; make sure that cnt value doesn't cross 2
		cnt = cnt % 3;

		// First lets enable the reception in interrupt mode
		// this code enables the receive interrupt
		while(USART_ReceiveDataIT(&usart2_handle, (uint8_t*)rx_buf, strlen(msg[cnt])) != USART_READY);

		// Send the msg indexed by cnt in blocking mode
		USART_SendData(&usart2_handle, (uint8_t*)msg[cnt], strlen(msg[cnt]));

		printf("Transmitted : %s\n",msg[cnt]);

    	// Now lets wait until all the bytes are received from the arduino .
    	// When all the bytes are received rxCmplt will be SET in application callback
		while(rxCmplt != SET);

		// just make sure that last byte should be null otherwise %s fails while printing
		rx_buf[strlen(msg[cnt]) + 1] = '\0';

    	// Print what we received from the arduino
    	printf("Received    : %s\n",rx_buf);

    	// invalidate the flag
    	rxCmplt = RESET;

    	// move on to next message indexed in msg[]
    	cnt++;
	}


	return 0;
}

void USART2_IRQHandler(void){
	USART_IRQHandling(&usart2_handle);
}

void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event){
	if(event == USART_EVENT_RX_CMPLT) {
		rxCmplt = SET;
	}
}
