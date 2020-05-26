/***************************************************************************//**
 * @file
 * @brief Configuration file for PAL (Platform Abstraction Layer)
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef DISPLAYPALCONFIG_H
#define DISPLAYPALCONFIG_H

/*
 * Select which oscillator should source the RTC clock.
 */
#undef  PAL_RTCC_CLOCK_LFXO
#define PAL_RTCC_CLOCK_LFRCO
#undef  PAL_RTCC_CLOCK_ULFRCO

/*
 * PAL SPI / USART configuration.
 * Select which USART and location is connected to the device via SPI.
 */
#define PAL_SPI_USART_UNIT          (USART1)
#define PAL_SPI_USART_CLOCK         (cmuClock_USART1)
#define PAL_SPI_USART_LOCATION_TX   (11)
#define PAL_SPI_USART_LOCATION_SCLK (11)

/*
 * Specify the SPI baud rate:
 */
#define PAL_SPI_BAUDRATE       (3500000) /* Max baudrate on EFR32xG13. */

/*
 * On the WSTKs, we can toggle some GPIO pins with hw only,
 * especially the GPIO port D pin 13 signal which is connected to the
 * polarity inversion (EXTCOMIN) pin on the Sharp Memory LCD. By defining
 * INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY the toggling of EXTCOMIN will
 * be handled by hardware, without software intervention, which saves power.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY

#endif /* DISPLAYPALCONFIG_H */
