/*
 * stm32f407xx_spi_driver.h
 *
 *  Created on: May 6, 2025
 *      Author: weber
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"

/*
 *  Configuration structure for SPIx peripheral
 */
typedef struct
{
	uint8_t SPI_DeviceMode;		// possible value from @SPI_Device_Mode
	uint8_t SPI_BusConfig;		// possible value from @SPI_BusConfig
	uint8_t SPI_SclkSpeed;		// possible value from @SPI_SCLK_Speed
	uint8_t SPI_DFF;			// possible value from @SPI_Data_Frame_Format
	uint8_t SPI_CPHA;			// possible value from @SPI_Clock_Phase
	uint8_t SPI_CPOL;			// possible value from @SPI_Clock_Polarity
	uint8_t SPI_SSM;			// possible value from @SPI_Software_Slave_Management
}SPI_Config_t;


/*
 *  Handle structure for SPIx peripheral
 */
typedef struct
{
	SPI_RegDef_t *pSPIx;		// This holds the base address of SPIx(x:0,1,2) peripheral
	SPI_Config_t SPIConfig;
}SPI_Handle_t;


/*
 *  @SPI_Device_Mode
 *  SPI possible device mode
 */
#define SPI_DEVICE_MODE_SLAVE			0
#define SPI_DEVICE_MODE_MASTER			1


/*
 *  @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD				1
#define SPI_BUS_CONFIG_HD				2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY	3


/*
 *  @SPI_SCLK_Speed
 *  SPI possible SCLK speed
 */
#define SPI_SCLK_SPEED_DIV2				0
#define SPI_SCLK_SPEED_DIV4				1
#define SPI_SCLK_SPEED_DIV8				2
#define SPI_SCLK_SPEED_DIV16			3
#define SPI_SCLK_SPEED_DIV32			4
#define SPI_SCLK_SPEED_DIV64			5
#define SPI_SCLK_SPEED_DIV128			6
#define SPI_SCLK_SPEED_DIV256			7


/*
 *  @SPI_Data_Frame_Format
 *  SPI possible data frame format
 */
#define SPI_DFF_8BITS		0
#define SPI_DFF_16BITS		1


/*
 *  @SPI_Clock_Phase
 *  SPI possible clock phase
 */
#define SPI_CPHA_LOW		0
#define SPI_CPHA_HIGH		1


/*
 *  @SPI_Clock_Polarity
 *  SPI possible clock polarity
 */
#define SPI_CPOL_LOW		0
#define SPI_CPOL_HIGH		1


/*
 *  @SPI_Software_Slave_Management
 *  SPI possible software slave management
 */
#define SPI_SSM_DI				0
#define SPI_SSM_EN				1


/*
 * SPI related status flags definitions
 */
#define SPI_TXE_FLAG	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	(1 << SPI_SR_RXNE)
#define SPI_BSY_FLAG	(1 << SPI_SR_BSY)



/************************************************************************************************
 *  					APIs supported by this driver
 *  	For more information about the APIs check the function definitions
 ************************************************************************************************/

/*
 *  Peripheral Clock setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);


/*
 *  Init and De-init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);


/*
 *  Data Send and Receive
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);


/*
 *  IRQ configuration and ISR handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);


/*
 * Other Peripheral Control APIs
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);







#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
