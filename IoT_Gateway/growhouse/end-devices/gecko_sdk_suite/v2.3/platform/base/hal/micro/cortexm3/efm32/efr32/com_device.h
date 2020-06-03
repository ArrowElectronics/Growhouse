/***************************************************************************//**
 * @file com_device.h
 * @brief Device specific COM settings.
 * @version 0.01.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 *
 ******************************************************************************/
#ifndef __COM_DEVICE_H__
#define __COM_DEVICE_H__

// Toggle COM ports to optimize code size
#if HAL_SERIAL_USART0_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_USART0) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_USART0)
#define COM_USART0_ENABLE
#endif
#if HAL_SERIAL_USART1_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_USART1) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_USART1)
#define COM_USART1_ENABLE
#endif
#if HAL_SERIAL_USART2_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_USART2) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_USART2)
#define COM_USART2_ENABLE
#endif
#if HAL_SERIAL_USART3_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_USART3) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_USART3)
#define COM_USART3_ENABLE
#endif
#if HAL_SERIAL_LEUART0_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_LEUART0) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_LEUART0)
#define COM_LEUART0_ENABLE
#endif
#if HAL_SERIAL_LEUART1_ENABLE                            \
  || (BSP_UARTNCP_USART_PORT == HAL_SERIAL_PORT_LEUART1) \
  || (BSP_SERIAL_APP_PORT == HAL_SERIAL_PORT_LEUART1)
#define COM_LEUART1_ENABLE
#endif
#if HAL_SERIAL_VUART_ENABLE
#define COM_VCP_ENABLE
#endif

// Define the number of com ports. This should never change
#define COM_PORTS 5
#define COM_UART_PORTS 4
#define COM_USB_PORTS 0
#define COM_VCP_PORTS 1

#if (COM_UART_PORTS + COM_USB_PORTS + COM_VCP_PORTS) != COM_PORTS
  #error COM ports not defined properly
#endif

#endif //__COM_DEVICE_H__
