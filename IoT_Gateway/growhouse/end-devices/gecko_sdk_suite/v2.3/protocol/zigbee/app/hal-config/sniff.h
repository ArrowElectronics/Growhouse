// -----------------------------------------------------------------------------
// @file sniff.h
// @brief Sniff HAL configuration
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
#define BSP_SERIAL_APP_PORT HAL_SERIAL_PORT_VUART

// undefine UARTNCP to prevent auto-enabling USART
#undef BSP_UARTNCP_USART_PORT
#define HAL_SERIAL_VUART_ENABLE 1

#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
