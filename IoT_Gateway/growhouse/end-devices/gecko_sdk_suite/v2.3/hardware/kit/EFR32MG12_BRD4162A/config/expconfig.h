/***************************************************************************//**
 * @file
 * @brief Provide EXP header configuration parameters.
 * @version 5.5.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef EXPCONFIG_H
#define EXPCONFIG_H

#define BSP_EXP_HEADER3_PIN   8
#define BSP_EXP_HEADER3_PORT  gpioPortD
#define BSP_EXP_HEADER4_PIN   6
#define BSP_EXP_HEADER4_PORT  gpioPortA
#define BSP_EXP_HEADER5_PIN   9
#define BSP_EXP_HEADER5_PORT  gpioPortD
#define BSP_EXP_HEADER6_PIN   7
#define BSP_EXP_HEADER6_PORT  gpioPortA
#define BSP_EXP_HEADER7_PIN   10
#define BSP_EXP_HEADER7_PORT  gpioPortD
#define BSP_EXP_HEADER8_PIN   8
#define BSP_EXP_HEADER8_PORT  gpioPortA
#define BSP_EXP_HEADER9_PIN   11
#define BSP_EXP_HEADER9_PORT  gpioPortD
#define BSP_EXP_HEADER10_PIN  9
#define BSP_EXP_HEADER10_PORT gpioPortA
#define BSP_EXP_HEADER11_PIN  12
#define BSP_EXP_HEADER11_PORT gpioPortD
#define BSP_EXP_HEADER12_PIN  6
#define BSP_EXP_HEADER12_PORT gpioPortB
#define BSP_EXP_HEADER13_PIN  9
#define BSP_EXP_HEADER13_PORT gpioPortC
#define BSP_EXP_HEADER14_PIN  7
#define BSP_EXP_HEADER14_PORT gpioPortB
#define BSP_EXP_HEADER15_PIN  10
#define BSP_EXP_HEADER15_PORT gpioPortC
#define BSP_EXP_HEADER16_PIN  11
#define BSP_EXP_HEADER16_PORT gpioPortC

#define BSP_EXP_I2C            I2C0
#define BSP_EXP_I2C_SCL_LOC    _I2C_ROUTELOC0_SCLLOC_LOC14
#define BSP_EXP_I2C_SCL_PIN    BSP_EXP_HEADER15_PIN
#define BSP_EXP_I2C_SCL_PORT   BSP_EXP_HEADER15_PORT
#define BSP_EXP_I2C_SDA_LOC    _I2C_ROUTELOC0_SDALOC_LOC16
#define BSP_EXP_I2C_SDA_PIN    BSP_EXP_HEADER16_PIN
#define BSP_EXP_I2C_SDA_PORT   BSP_EXP_HEADER16_PORT

#define BSP_EXP_SPI            USART2
#define BSP_EXP_SPI_CLK_LOC    _USART_ROUTELOC0_CLKLOC_LOC1
#define BSP_EXP_SPI_CLK_PIN    BSP_EXP_HEADER8_PIN
#define BSP_EXP_SPI_CLK_PORT   BSP_EXP_HEADER8_PORT
#define BSP_EXP_SPI_CS_LOC     _USART_ROUTELOC0_CSLOC_LOC1
#define BSP_EXP_SPI_CS_PIN     BSP_EXP_HEADER10_PIN
#define BSP_EXP_SPI_CS_PORT    BSP_EXP_HEADER10_PORT
#define BSP_EXP_SPI_MISO_LOC   _USART_ROUTELOC0_RXLOC_LOC1
#define BSP_EXP_SPI_MISO_PIN   BSP_EXP_HEADER6_PIN
#define BSP_EXP_SPI_MISO_PORT  BSP_EXP_HEADER6_PORT
#define BSP_EXP_SPI_MOSI_LOC   _USART_ROUTELOC0_TXLOC_LOC1
#define BSP_EXP_SPI_MOSI_PIN   BSP_EXP_HEADER4_PIN
#define BSP_EXP_SPI_MOSI_PORT  BSP_EXP_HEADER4_PORT

#define BSP_EXP_USART          USART3
#define BSP_EXP_USART_CTS_LOC  _USART_ROUTELOC1_CTSLOC_LOC28
#define BSP_EXP_USART_CTS_PIN  BSP_EXP_HEADER3_PIN
#define BSP_EXP_USART_CTS_PORT BSP_EXP_HEADER3_PORT
#define BSP_EXP_USART_RTS_LOC  _USART_ROUTELOC1_RTSLOC_LOC28
#define BSP_EXP_USART_RTS_PIN  BSP_EXP_HEADER5_PIN
#define BSP_EXP_USART_RTS_PORT BSP_EXP_HEADER5_PORT
#define BSP_EXP_USART_RX_LOC   _USART_ROUTELOC0_RXLOC_LOC10
#define BSP_EXP_USART_TX_PIN   BSP_EXP_HEADER12_PIN
#define BSP_EXP_USART_TX_PORT  BSP_EXP_HEADER12_PORT
#define BSP_EXP_USART_TX_LOC   _USART_ROUTELOC0_TXLOC_LOC10
#define BSP_EXP_USART_RX_PIN   BSP_EXP_HEADER14_PIN
#define BSP_EXP_USART_RX_PORT  BSP_EXP_HEADER14_PORT

#endif /* EXPCONFIG_H */
