/***************************************************************************//**
 * @file
 * @brief Provide EXP header configuration parameters.
 * @version 5.5.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef EXPCONFIG_H
#define EXPCONFIG_H

#define BSP_EXP_HEADER3_PIN      6
#define BSP_EXP_HEADER3_PORT     gpioPortC
#define BSP_EXP_HEADER4_PIN      0
#define BSP_EXP_HEADER4_PORT     gpioPortD
#define BSP_EXP_HEADER5_PIN      7
#define BSP_EXP_HEADER5_PORT     gpioPortC
#define BSP_EXP_HEADER6_PIN      1
#define BSP_EXP_HEADER6_PORT     gpioPortD
#define BSP_EXP_HEADER7_PIN      0
#define BSP_EXP_HEADER7_PORT     gpioPortE
#define BSP_EXP_HEADER8_PIN      2
#define BSP_EXP_HEADER8_PORT     gpioPortD
#define BSP_EXP_HEADER9_PIN      1
#define BSP_EXP_HEADER9_PORT     gpioPortE
#define BSP_EXP_HEADER10_PIN     3
#define BSP_EXP_HEADER10_PORT    gpioPortD
#define BSP_EXP_HEADER11_PIN     11
#define BSP_EXP_HEADER11_PORT    gpioPortB
#define BSP_EXP_HEADER12_PIN     4
#define BSP_EXP_HEADER12_PORT    gpioPortD
#define BSP_EXP_HEADER13_PIN     3
#define BSP_EXP_HEADER13_PORT    gpioPortF
#define BSP_EXP_HEADER14_PIN     5
#define BSP_EXP_HEADER14_PORT    gpioPortD
#define BSP_EXP_HEADER15_PIN     7
#define BSP_EXP_HEADER15_PORT    gpioPortD
#define BSP_EXP_HEADER16_PIN     6
#define BSP_EXP_HEADER16_PORT    gpioPortD

#define BSP_EXP_I2C              I2C0
#define BSP_EXP_I2C_ROUTE_LOC    _I2C_ROUTE_LOCATION_LOC1
#define BSP_EXP_I2C_SCL_PIN      BSP_EXP_HEADER15_PIN
#define BSP_EXP_I2C_SCL_PORT     BSP_EXP_HEADER15_PORT
#define BSP_EXP_I2C_SDA_PIN      BSP_EXP_HEADER16_PIN
#define BSP_EXP_I2C_SDA_PORT     BSP_EXP_HEADER16_PORT

#define BSP_EXP_SPI              USART1
#define BSP_EXP_SPI_ROUTE_LOC    _USART_ROUTE_LOCATION_LOC1
#define BSP_EXP_SPI_CLK_PIN      BSP_EXP_HEADER8_PIN
#define BSP_EXP_SPI_CLK_PORT     BSP_EXP_HEADER8_PORT
#define BSP_EXP_SPI_CS_PIN       BSP_EXP_HEADER10_PIN
#define BSP_EXP_SPI_CS_PORT      BSP_EXP_HEADER10_PORT
#define BSP_EXP_SPI_MISO_PIN     BSP_EXP_HEADER6_PIN
#define BSP_EXP_SPI_MISO_PORT    BSP_EXP_HEADER6_PORT
#define BSP_EXP_SPI_MOSI_PIN     BSP_EXP_HEADER4_PIN
#define BSP_EXP_SPI_MOSI_PORT    BSP_EXP_HEADER4_PORT

#define BSP_EXP_USART            USART2
#define BSP_EXP_USART_ROUTE_LOC  _USART_ROUTE_LOCATION_LOC1
#define BSP_EXP_USART_CTS_PIN    BSP_EXP_HEADER3_PIN
#define BSP_EXP_USART_CTS_PORT   BSP_EXP_HEADER3_PORT
#define BSP_EXP_USART_RTS_PIN    BSP_EXP_HEADER5_PIN
#define BSP_EXP_USART_RTS_PORT   BSP_EXP_HEADER5_PORT
#define BSP_EXP_USART_RX_PIN     BSP_EXP_HEADER14_PIN
#define BSP_EXP_USART_RX_PORT    BSP_EXP_HEADER14_PORT
#define BSP_EXP_USART_TX_PIN     BSP_EXP_HEADER12_PIN
#define BSP_EXP_USART_TX_PORT    BSP_EXP_HEADER12_PORT

#endif /* EXPCONFIG_H */
