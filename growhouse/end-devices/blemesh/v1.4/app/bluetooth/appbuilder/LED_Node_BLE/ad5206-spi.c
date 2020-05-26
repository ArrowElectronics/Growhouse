/*
 * ad5206-spi.c
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   ad5206-spi.c
 *   @brief  Implementation of communication protocol for digital potentiometer
 *           over SPI interface.
 *
 ******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include <stdio.h>
#include "hal-config.h"
#include "ad5206.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "spidrv.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
static SPIDRV_HandleData_t master_adc;
static SPIDRV_Handle_t SPI_master_adc = &master_adc;

/******************************************************************************/
/* Function Definition                                                        */
/******************************************************************************/

/***************************************************************************//**
 * @brief SPI interface init function
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
Ecode_t spi_init(void)
{
       SPIDRV_Init_t SPI_config = SPIDRV_MASTER_USART1;
       SPI_config.clockMode = spidrvClockMode3;
       SPI_config.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC11;
       SPI_config.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC11;
       SPI_config.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC11;
       SPI_config.portLocationCs = _USART_ROUTELOC0_CSLOC_LOC12;
       SPI_config.csControl = spidrvCsControlApplication;
       GPIO_PinModeSet(AD5206_PORT_CS, AD5206_PIN_CS, gpioModePushPull, 1);

       return SPIDRV_Init(SPI_master_adc, &SPI_config);
}

void spi_deinit()
{
	SPIDRV_DeInit(SPI_master_adc);
	GPIO_PinModeSet(AD5206_PORT_CS, AD5206_PORT_CS, gpioModeDisabled, 1);
}

/***************************************************************************//**
 * @brief Function to set digital potentiometer channel for perticular value
 *
 * @param -  reg - digital potentiometer register value.
 * @param -  valie - value to set on digital potentiometer channel
 *
 * @return None.
 *******************************************************************************/
Ecode_t set_led_level(uint8_t reg, uint8_t value)
{
	uint8_t cmd = reg & 0x07;
	uint8_t tx_buffer[8] = { 0 };
	Ecode_t returnCode = false;

	printf("set_led_level Address 0x%x and Value : %d\r\n",cmd, value);

	/* Chip select Low for dpot */
	GPIO_PinOutClear(AD5206_PORT_CS, AD5206_PIN_CS);

	/* Generate Tx buffer */
	tx_buffer[0] = cmd;
	tx_buffer[1] = value;

	// Transmit data
	returnCode = SPIDRV_MTransmitB(SPI_master_adc, (void *) tx_buffer, 2);
	if (returnCode != ECODE_EMDRV_SPIDRV_OK) {
		printf("Failed to set_led_level Address 0x%x Value : 0x%d, Status : %lu\r\n",cmd, value, returnCode);
		returnCode = false;
	}
	else {
		returnCode = true;
	}

	/* Chip select High for dpot */
	GPIO_PinOutSet(AD5206_PORT_CS, AD5206_PIN_CS);

	return returnCode;
}
