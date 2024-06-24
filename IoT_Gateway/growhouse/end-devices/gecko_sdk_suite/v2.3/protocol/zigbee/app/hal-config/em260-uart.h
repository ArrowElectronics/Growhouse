// -----------------------------------------------------------------------------
// @file em260-uart.h
// @brief EZSP UART NCP HAL configuration
//
// @section License
// <b>(C) Copyright 2016 Silicon Laboratories, www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#ifndef APP_HAL_CONFIG_H
#define APP_HAL_CONFIG_H
#include "hal-config-types.h"

#define HAL_SERIAL_VUART_ENABLE 1
#define HAL_SERIAL_USART0_ENABLE 1

#define HAL_UARTNCP_BAUD_RATE             115200
#define HAL_UARTNCP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_HW
#define HAL_UARTNCP_RXSTART               16
#define HAL_UARTNCP_RXSTOP                16
#define HAL_UARTNCP_RX_QUEUE_SIZE         64
#define HAL_UARTNCP_TX_QUEUE_SIZE         128

#define HAL_VCOM_ENABLE                               (1)
#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
