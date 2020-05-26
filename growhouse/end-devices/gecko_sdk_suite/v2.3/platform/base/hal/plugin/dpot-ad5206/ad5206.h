/*
 * ad5206.h
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 */

#ifndef _AD5206_
#define _AD5206_

#include "spidrv.h"
/**********************
 *       Defines
 **********************/

//GPIO for chip selection
#define DPOT_PORT_CS            gpioPortC
#define DPOT_PIN_CS		        10

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

#define MAX_LED_LEVEL			100
#define RDAC_MAPPING			2.55f

#define LED_CHANNELS			6

/***********************
 *    Global Variables
 ************************/
static SPIDRV_HandleData_t master_adc;
static SPIDRV_Handle_t SPI_master_adc = &master_adc;


typedef struct LED_Configuration
{
	/* First byte is for either configuration is written or not */
	uint8_t Is_LED_Configuration_Set;

	/* First byte is for either configuration is written or not */
	uint8_t Is_ZIGBEE_Provisioned;

	/* First byte is for either configuration is written or not */
	uint8_t Is_BLE_Provisioned;

	/* LED Configuration data */
	uint8_t LED_Configuration_Data[LED_CHANNELS];

	/* LED Intensity data */
	int LED_Channel_intensity[LED_CHANNELS];
}LED_Configuration;

/**********************
 *       Functions
 **********************/

Ecode_t spi_init();
void spi_deinit();
Ecode_t set_led_level(uint8_t reg, uint8_t value);
Ecode_t halAD5204DriverSetLedLevel(uint8_t Channel, uint8_t value);
void updateLED_Configuration_intoFlash(struct LED_Configuration *LED_ConfigurationBlock);
void getLED_configuration_FromFlash(struct LED_Configuration *LED_ConfigurationBlock);
void scheduleLEDconfiguraionsendEvent(void);
void StartLED_blinking(void);

#endif
