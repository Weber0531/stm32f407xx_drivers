/*
 * 014UART_Tx.c
 *
 *  Created on: May 26, 2025
 *      Author: weber
 */


#include "string.h"
#include "stm32f407xx.h"

char msg[1024] = "UART Tx testing...\n\r";

USART_Handle_t usart2_handle;

void delay(void){
	for(uint32_t i = 0; i < 500000 / 2; i++);
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

void USART2_GPIOInit(void){
	GPIO_Handle_t usart_gpios;

	usart_gpios.pGPIOx = GPIOA;

	usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
	usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
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
	usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&usart2_handle);

}

int main(void)
{
	GPIO_ButtonInit();

	USART2_GPIOInit();

	USART2_Init();

	USART_PeripheralControl(USART2, ENABLE);

	while(1){
		// wait till button is pressed
		while(!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));

		// to avoid button de-bouncing related issues 200ms of delay
		delay();

		USART_SendData(&usart2_handle, (uint8_t*)msg, strlen(msg));
	}

	return 0;
}
