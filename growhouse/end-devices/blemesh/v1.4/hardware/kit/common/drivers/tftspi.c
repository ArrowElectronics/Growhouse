/***************************************************************************//**
 * @file
 * @brief SPI controller interface for SSD2119 display
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

#include "em_device.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "tftspi.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup Tft TFT
 * @brief Driver for SSD2119 TFT in Direct Drive or Adress Mapped mode
 * @details
 * @{
 ******************************************************************************/

/** Set default values for tft synchronous mode init structure */
static const USART_InitSync_TypeDef inittft = {
  usartEnable,     /* Enable RX/TX when init completed. */
  48000000,        /* Use 48MHz reference clock */
  1000000,         /* 7 Mbits/s. */
  usartDatabits9,  /* 9 databits. */
  true,            /* Master mode. */
  true,            /* Send most significant bit first. */
  usartClockMode3, /* Clock idle low, sample on rising edge. */
  false,           /* Not USART PRS input mode. */
  usartPrsRxCh0,   /* PRS channel 0. */
  false,           /* No AUTOTX mode. */
  false            /* No AUTOCS mode */
};

/**************************************************************************//**
 * @brief Initialize SPI interface to TFT-LCD SSD2119 controller
 * @note To enable access, be sure to call the function
 *       @ref BSP_PeripheralAccess() before using this interface.
 *****************************************************************************/
void SPI_TFT_Init(void)
{
  /* Enabling clock to USART */
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_USART1, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* IO configuration (USART 1, Location #1) */
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0); /* TX - MOSI */
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);    /* RX - MISO */
  GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0); /* CLK */
  GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 1); /* CS  */

  /* Ensure out of reset configuration */
  USART_Reset(USART1);

  /* Initialize USART1, in SPI master mode. */
  USART_InitSync(USART1, &inittft);

  USART1->ROUTE = USART_ROUTE_TXPEN
                  | USART_ROUTE_RXPEN
                  | USART_ROUTE_CLKPEN
                  | USART_ROUTE_LOCATION_LOC1;
}

/**************************************************************************//**
 * @brief Write registers to SSD2119 controller
 * @param [in] reg
 *      Register to write to
 * @param [in] data
 *      16-bit data to write into register
 * @note
 *      It is not possible to read back register value through SSD2119 SPI
 *      interface.
 *****************************************************************************/
void SPI_TFT_WriteRegister(uint8_t reg, uint16_t data)
{
  /* Enable chip select */
  GPIO_PinOutClear(gpioPortD, 3);

  /* Select register first */
  USART1->CTRL = USART1->CTRL & ~USART_CTRL_BIT8DV;

  USART_Tx(USART1, reg & 0xFF);
  USART_Rx(USART1);

  /* Write data */
  USART1->CTRL = USART1->CTRL | USART_CTRL_BIT8DV;
  USART_Tx(USART1, (data & 0xff00) >> 8);
  USART_Rx(USART1);
  USART_Tx(USART1, (data & 0x00ff));
  USART_Rx(USART1);

  /* Clear chip select */
  GPIO_PinOutSet(gpioPortD, 3);
}

/** @} (end group Tft) */
/** @} (end group kitdrv) */
