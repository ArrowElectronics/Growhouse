/***************************************************************************//**
 * @file rtcdrv_config.h
 * @brief RTCDRV configuration file
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef RTCDRV_CONFIG_H
#define RTCDRV_CONFIG_H

/***************************************************************************//**
 * @addtogroup EM_Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RTCDRV
 * @{
 ******************************************************************************/

/// @brief Define the number of timers the application needs.
#define EMDRV_RTCDRV_NUM_TIMERS     (5)

/// @brief Define to include wallclock functionality.
#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

/// @brief Define to enable integration with SLEEP driver.
//#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

/** @} (end addtogroup RTCDRV) */
/** @} (end addtogroup EM_Drivers) */

#endif // RTCDRV_CONFIG_H
