/*
 * ad5206-spi.c
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************/
/**
 *   @file   ad5206-spi.c
 *   @brief  Initialization and De-initialization of SPI and data
 * 	     transfer API using SPI protocol.
 *
 **************************************************************************/

/***************************************************************************/
/* Include Files                                                           */
/***************************************************************************/
#include PLATFORM_HEADER
#include <math.h>
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"
#include "spidrv.h"
#include "ad5206.h"

/***************************************************************************/
/**
 * @brief SPI initialization function
 *
 * @param None.
 *
 * @return Ecode_t : Error code
 **************************************************************************/
Ecode_t spi_init()
{
	SPIDRV_Init_t SPI_config = SPIDRV_MASTER_USART1;
	SPI_config.clockMode = spidrvClockMode3;
	SPI_config.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC11;
	SPI_config.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC11;
	SPI_config.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC11;
	SPI_config.portLocationCs = _USART_ROUTELOC0_CSLOC_LOC12;
	SPI_config.csControl = spidrvCsControlApplication;
	GPIO_PinModeSet(DPOT_PORT_CS, DPOT_PIN_CS, gpioModePushPull, 1);

	return SPIDRV_Init(SPI_master_adc, &SPI_config);
}

/***************************************************************************/
/**
 * @brief Spi deintialization function
 *
 * @param None.
 *
 * @return None.
 **************************************************************************/
void spi_deinit()
{
	SPIDRV_DeInit(SPI_master_adc);
	GPIO_PinModeSet(DPOT_PORT_CS, DPOT_PIN_CS, gpioModeDisabled, 1);
}

/***************************************************************************/
/**
 * @brief Set LED intensity level through writing RDACs register of dpot IC.
 *
 * @param - uint8_t reg - RDAC register address as per channel
 *
 *          uint8_t value - Intensity level for particular channel
 *
 * @return Ecode_t - Error code
 * 		     1 - For success
 * 		     0 - For failure
 **************************************************************************/
Ecode_t set_led_level(uint8_t reg, uint8_t value)
{
	uint8_t cmd = reg & 0x07;
	uint8_t tx_buffer[8] = { 0 };
	Ecode_t returnCode = false;

	emberAfCorePrintln("set_led_level Address 0x%x and Value : %d",cmd, value);

	// Chip select Low
	GPIO_PinOutClear(DPOT_PORT_CS, DPOT_PIN_CS);

	// Generate Tx buffer
	tx_buffer[0] = cmd;
	tx_buffer[1] = value;

	// Transmit data
	returnCode = SPIDRV_MTransmitB(SPI_master_adc, (void *) tx_buffer, 2);
	if (returnCode != ECODE_EMDRV_SPIDRV_OK) {
		emberAfCorePrintln("Failed to set_led_level Address 0x%x Value : 0x%d, Status : %d",cmd, value, returnCode);
		returnCode = false;
	}
	else {
		returnCode = true;
	}

	// Chip select High
	GPIO_PinOutSet(DPOT_PORT_CS, DPOT_PIN_CS);

	return returnCode;
}
