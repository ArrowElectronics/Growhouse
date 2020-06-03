/***************************************************************************//**
 * @file rtcdrv_config.h
 * @brief RTCDRV configuration file.
 * @version 4.2.3
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_RTCDRV_CONFIG_H__
#define __SILICON_LABS_RTCDRV_CONFIG_H__

#ifdef BOARD_HEADER
  #include BOARD_HEADER
#endif
#ifdef HAL_CONFIG
  #include "hal-config.h"
  #if (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
    #define EMDRV_RTCDRV_USE_LFRCO 1
  #endif
#endif

/***************************************************************************//**
 * @addtogroup EM_Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RTCDRV
 * @{
 ******************************************************************************/

/// @brief Define the number of timers the application needs.
#define EMDRV_RTCDRV_NUM_TIMERS     (4)

/// @brief Define to include wallclock functionality.
#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

/// @brief Define to enable integration with SLEEP driver.
#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

/** @} (end addtogroup RTCDRV) */
/** @} (end addtogroup EM_Drivers) */

#endif /* __SILICON_LABS_RTCDRV_CONFIG_H__ */
