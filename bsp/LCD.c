/*
 * LCD.c
 *
 *  Created on: May 27, 2025
 *      Author: weber
 */


#include "LCD.h"

static void write_4_bits(uint8_t value);
static void lcd_enable(void);

void lcd_send_command(uint8_t cmd){
	/* RS = 0 , For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, RESET);

	/* RnW = 0, Writing to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, RESET);

	write_4_bits(cmd >> 4); // Higher nibble
	write_4_bits(cmd & 0x0F); // Lower nibble
}


/*
 * This function sends a character to the LCD
 * Here we used 4 bit parallel data transmission.
 * First higher nibble of the data will be sent on to the data lines D4,D5,D6,D7
 * Then lower nibble of the data will be set on to the data lines D4,D5,D6,D7
 */
void lcd_send_char(uint8_t data){
	/* RS = 1 , For LCD user data */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, SET);

	/* RnW = 0, Writing to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, RESET);

	write_4_bits(data >> 4); // Higher nibble
	write_4_bits(data & 0x0F); // Lower nibble
}

void lcd_init(void){
	// 1. Configure the GPIO pins which are used for LCD connections

	GPIO_Handle_t lcd_signal;

	lcd_signal.pGPIOx = LCD_GPIO_PORT;
	lcd_signal.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	lcd_signal.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	lcd_signal.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	lcd_signal.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&lcd_signal);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, RESET);


	// 2. Do the LCD initialization

	mdelay(40);

	/* RS = 0 , For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, RESET);

	/* RnW = 0, Writing to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, RESET);

	write_4_bits(0x3);

	mdelay(5);

	write_4_bits(0x3);

	udelay(150);

	write_4_bits(0x3);
	write_4_bits(0x2);
}


/* writes 4 bits of data/command on to D4,D5,D6,D7 lines */
static void write_4_bits(uint8_t value){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, (value >> 0) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, (value >> 1) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, (value >> 2) & 0x1);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, (value >> 3) & 0x1);

	lcd_enable();
}


// do the high to low transition on the enable line
static void lcd_enable(void){
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, SET);
	udelay(10);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, RESET);
	udelay(100); // execution time > 37 microseconds
}




