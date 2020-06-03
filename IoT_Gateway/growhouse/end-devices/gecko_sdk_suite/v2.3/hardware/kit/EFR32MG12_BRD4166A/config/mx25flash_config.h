/***************************************************************************//**
 * @file mx25_config.h
 * @brief MX25R8035F NOR flash configuration file
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef MX25_CONFIG_H
#define MX25_CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

#define MX25_PORT_MOSI         gpioPortK
#define MX25_PIN_MOSI          0
#define MX25_PORT_MISO         gpioPortK
#define MX25_PIN_MISO          2
#define MX25_PORT_SCLK         gpioPortF
#define MX25_PIN_SCLK          7
#define MX25_PORT_CS           gpioPortK
#define MX25_PIN_CS            1

#define MX25_USART             USART2
#define MX25_USART_CLK         cmuClock_USART2

#define MX25_USART_LOC_MOSI    USART_ROUTELOC0_TXLOC_LOC29
#define MX25_USART_LOC_MISO    USART_ROUTELOC0_RXLOC_LOC30
#define MX25_USART_LOC_SCLK    USART_ROUTELOC0_CLKLOC_LOC18

#endif // MX25_CONFIG_H
