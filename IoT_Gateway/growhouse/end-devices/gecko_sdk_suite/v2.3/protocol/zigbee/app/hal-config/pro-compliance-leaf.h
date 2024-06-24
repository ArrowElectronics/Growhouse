// -----------------------------------------------------------------------------
// @file pro-compliance-leaf.h
// @brief Pro Compliance Leaf App HAL configuration
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
#undef BSP_SERIAL_APP_PORT
#define BSP_SERIAL_APP_PORT HAL_SERIAL_PORT_USART0
#define HAL_SERIAL_USART0_ENABLE 1
#define HAL_SERIAL_VCOM_ENABLE 1

#define HAL_SERIAL_APP_BAUD_RATE             115200
#define HAL_SERIAL_APP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_HWUART
#define HAL_SERIAL_APP_RXSTART               16
#define HAL_SERIAL_APP_RXSTOP                16
#define HAL_SERIAL_APP_RX_QUEUE_SIZE         64
#define HAL_SERIAL_APP_TX_QUEUE_SIZE         128

#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
