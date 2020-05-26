/*
 * ad5206-dpot.c
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   ad5206-dpot.c
 *   @brief  Implementation control logic for digital potentiometer
 *           over SPI.
 *
 ******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ad5206.h"
#include "arm_math.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
static bool init_done = false;

/******************************************************************************/
/* Function Definition                                                        */
/******************************************************************************/

/***************************************************************************//**
 * @brief Initialize digital potentiometer
 *
 * @param  None.
 *
 * @return None.
 *******************************************************************************/
void dpotAd5206Init(void)
{
    Ecode_t ret = 0;
    /* Initialization of SPI bus for dpot */
    ret = spi_init();
    if (ret) {
        printf("SPI Initialization failed Return : %lu",ret);
        init_done = false;
        spi_deinit();
        return;
    }
    printf("SPI Initialization Success");
    init_done = true;
}


/***************************************************************************//**
 * @brief Set LED level over Digital Potentiometer
 *
 * @param -  channel - LED channel
 * @param -  value - LED channel intensity value.
 *
 * @return status code.
 *******************************************************************************/
Ecode_t halAD5204DriverSetLedLevel(uint8_t Channel, uint16_t value)
{
	Ecode_t status = false;

	/* Value Check, it should be less than or equal to 65535s */
	if (value > MAX_LED_LEVEL)
		value = MAX_LED_LEVEL;

	/* Mapping of LED level 0 to 65535 into RDACs level 0 to 255. */
	value = (int)((float)value * RDAC_MAPPING);

	if (init_done) {
		switch (Channel) {
		case DPOT_RDAC_CHANNEL1:
			/* LED number 1 is mapped with Channel 4(w4) */
			status = set_led_level(RDAC3_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL2:
			/* LED number 2 is mapped with Channel 2(w2) */
			status = set_led_level(RDAC1_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL3:
			/* LED number 3 is mapped with Channel 1(w1) */
			status = set_led_level(RDAC0_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL4:
			/* LED number 4 is mapped with Channel 3(w3) */
			status = set_led_level(RDAC2_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL5:
			/* LED number 5 is mapped with Channel 5(w5) */
			status = set_led_level(RDAC4_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL6:
			/* LED number 6 is mapped with Channel 6(w6) */
			status = set_led_level(RDAC5_ADDR, value);
			break;
		default:
			printf("Maximum only 6 channel supported");
			status = false;
			break;
		}
	} else {
		dpotAd5206Init();
	}
	return status;
}
