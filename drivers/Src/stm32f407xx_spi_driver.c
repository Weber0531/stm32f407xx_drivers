/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: May 6, 2025
 *      Author: weber
 */


#include "stm32f407xx_spi_driver.h"


/*********************************************************************
 * @fn      		  - SPI_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given SPIx peripheral
 *
 * @param[in]         - base address of the SPIx peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi){
	if (EnorDi == ENABLE) {
		if (pSPIx == SPI1) {
			SPI1_PCLK_EN();
		} else if (pSPIx == SPI2) {
			SPI2_PCLK_EN();
		} else if (pSPIx == SPI3) {
			SPI3_PCLK_EN();
		}
	} else {
		if (pSPIx == SPI1) {
			SPI1_PCLK_DI();
		} else if (pSPIx == SPI2) {
			SPI2_PCLK_DI();
		} else if (pSPIx == SPI3) {
			SPI3_PCLK_DI();
		}
	}
}


/*********************************************************************
 * @fn      		  - SPI_Init
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_Init(SPI_Handle_t *pSPIHandle){

	// Enable the peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	// first let's configure the SPI_CR1 register

	uint32_t tempreg = 0;

	// 1. configure the device mode
	tempreg |= (pSPIHandle->SPIConfig.SPI_DeviceMode << 2);

	// 2. Configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD) {
		// BIDI mode should be cleared
		tempreg &= ~(1 << 15);
	} else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD) {
		// BIDI mode should be set
		tempreg |= (1 << 15);
	} else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY) {
		// BIDI mode should be cleared
		tempreg &= ~(1 << 15);
		// RXONLY bit must be set
		tempreg |= (1 << 10);
	}

	// 3. Configure the SPI serial clock speed (baud rate)
	tempreg |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << 3);

	// 4.  Configure the DFF
	tempreg |= (pSPIHandle->SPIConfig.SPI_DFF << 11);

	// 5.  Configure the CPOL
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPOL << 1);

	// 6.  Configure the CPHA
	tempreg |= (pSPIHandle->SPIConfig.SPI_CPHA << 0);

	pSPIHandle->pSPIx->CR1 = tempreg;
}


/*********************************************************************
 * @fn      		  - SPI_DeInit
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_DeInit(SPI_RegDef_t *pSPIx){
	if(pSPIx == SPI1) {
		SPI1_REG_RESET();
	} else if(pSPIx == SPI2) {
		SPI2_REG_RESET();
	} else if(pSPIx == SPI3) {
		SPI3_REG_RESET();
	}
}


/*********************************************************************
 * @fn      		  - SPI_GetFlagStatus
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - 0 or 1
 *
 * @Note              -

 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName){
	if(pSPIx->SR & FlagName) {
		return FLAG_SET;
	}
	return FLAG_RESET;
}


/*********************************************************************
 * @fn      		  - SPI_SendData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              - This is blocking call

 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len){
	while(Len > 0) {
		// 1. wait until TXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		// 2. check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16 bits DFF
			//1. load the data in to the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len--;
			Len--;
			(uint16_t*)pTxBuffer++;
		} else {
			// 8 bits DFF
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}
}


/*********************************************************************
 * @fn      		  - SPI_ReceiveData
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len){
	while(Len > 0) {
		// 1. wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// 2. check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) {
			// 16 bits DFF
			//1. load the data from the DR to RxBuffer address
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			(uint16_t*)pRxBuffer++;
		} else {
			// 8 bits DFF
			*pRxBuffer = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}


/*********************************************************************
 * @fn      		  - SPI_PeripheralControl
 *
 * @brief             - Enable or disable the SPIx peripheral
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi){
	if(EnorDi == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}


/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 *
 * @brief             - Enable or disable the SSI bit when SSM = 1
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi){
	if(EnorDi == ENABLE) {
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	} else {
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}


/*********************************************************************
 * @fn      		  - SPI_SSOEConfig
 *
 * @brief             - Enable or disable the SSOE bit when SSM = 1
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi){
	if(EnorDi == ENABLE) {
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	} else {
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}



/*********************************************************************
 * @fn      		  - SPI_IRQInterruptConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi){
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64) // 32 to 63
		{
			//program ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber < 96) // 64 to 95
		{
			//program ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);

		}else if(IRQNumber > 31 && IRQNumber < 64) // 32 to 63
		{
			//program ICER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));

		}else if(IRQNumber >= 64 && IRQNumber < 96) // 64 to 95
		{
			//program ICER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}



/*********************************************************************
 * @fn      		  - SPI_IRQPriorityConfig
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority){
	// 1. find out the IPR register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}


/*********************************************************************
 * @fn      		  - SPI_IRQHandling
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle){

}



/*********************************************************************
 * @fn      		  - SPI_SendDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->Txstate;

	if(state != SPI_BUSY_IN_TX) {
		// 1. Save the Tx buffer address and Len information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		// 2. Mark the SPI state as busy in transmission, so that no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		// 3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}

	return state;
}




/*********************************************************************
 * @fn      		  - SPI_ReceiveDataIT
 *
 * @brief             -
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len){
	uint8_t state = pSPIHandle->Rxstate;

	if(state != SPI_BUSY_IN_RX) {
		// 1. Save the Rx buffer address and Len information in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		// 2. Mark the SPI state as busy in reception, so that no other code can take over same SPI peripheral until reception is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		// 3. Enable the RXNEIE control bit to get interrupt whenever RXNE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
	}

	return state;
}











