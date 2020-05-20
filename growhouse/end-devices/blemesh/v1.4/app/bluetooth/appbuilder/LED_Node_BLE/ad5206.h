/*
 * ad5206.h
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   ad5206.h
 *   @brief  Header for Implementation of communication protocol with
 *   		 digital potentiometer over SPI interface.
 *
 ******************************************************************************/

#ifndef AD5206_H_
#define AD5206_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "ecode.h"
#include "spidrv.h"

/******************************************************************************/
/* MACROs		                                                              */
/******************************************************************************/

#define DPOT_RDAC_CHANNEL1      1
#define DPOT_RDAC_CHANNEL2      2
#define DPOT_RDAC_CHANNEL3      3
#define DPOT_RDAC_CHANNEL4      4
#define DPOT_RDAC_CHANNEL5      5
#define DPOT_RDAC_CHANNEL6      6

#define RDAC0_ADDR              0x0
#define RDAC1_ADDR              0x1
#define RDAC2_ADDR              0x2
#define RDAC3_ADDR              0x3
#define RDAC4_ADDR              0x4
#define RDAC5_ADDR              0x5

#define MAX_LED_LEVEL			65535
#define RDAC_MAPPING			0.003890991f

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Function Declaration                                                       */
/******************************************************************************/

Ecode_t spi_init();
void spi_deinit();
Ecode_t set_led_level(uint8_t reg, uint8_t value);
Ecode_t halAD5204DriverSetLedLevel(uint8_t Channel, uint16_t value);
void dpotAd5206Init(void);

#endif /* AD5206_H_ */
