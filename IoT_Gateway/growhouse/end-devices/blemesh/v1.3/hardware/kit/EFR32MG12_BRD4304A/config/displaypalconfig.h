/***************************************************************************//**
 * @file displaypalconfig.h
 * @brief Configuration file for PAL (Platform Abstraction Layer)
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef DISPLAY_PAL_CONFIG_H
#define DISPLAY_PAL_CONFIG_H

/*
 * Select which oscillator should source the RTC clock.
 */
#undef  PAL_RTC_CLOCK_LFXO
#define  PAL_RTC_CLOCK_LFRCO
#undef  PAL_RTC_CLOCK_ULFRCO

/*
 * PAL SPI / USART configuration for the SLWSTK6000A.
 * Select which USART and location is connected to the device via SPI.
 */
#define PAL_SPI_USART_UNIT     (USART1)
#define PAL_SPI_USART_CLOCK    (cmuClock_USART1)
#define PAL_SPI_USART_LOCATION_TX (11)
#define PAL_SPI_USART_LOCATION_SCLK (11)

/*
 * Specify the SPI baud rate:
 */
#define PAL_SPI_BAUDRATE       (3500000) /* Max baudrate on EFR32MG12. */

/*
 * On the SLWSTK6000A, we can toggle some GPIO pins with hw only,
 * especially the GPIO port D pin 13 signal which is connected to the
 * polarity inversion (EXTCOMIN) pin on the Sharp Memory LCD. By defining
 * INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY the toggling of EXTCOMIN will
 * be handled by hardware, without software intervention, which saves power.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY

#endif /* DISPLAY_PAL_CONFIG_H */
