/***************************************************************************//**
 * @file dmadrv_config.h
 * @brief DMADRV configuration file
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

#ifndef DMADRV_CONFIG_H
#define DMADRV_CONFIG_H

#include "em_device.h"

/***************************************************************************//**
 * @addtogroup emdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup DMADRV
 * @{
 ******************************************************************************/

/// DMADRV DMA interrupt priority configuration option.
/// Set DMA interrupt priority. Range is 0..7, 0 is highest priority.
#ifndef EMDRV_DMADRV_DMA_IRQ_PRIORITY
#define EMDRV_DMADRV_DMA_IRQ_PRIORITY 4
#endif

/// DMADRV DMA channel priority configuration option.
/// Set DMA channel priority. Range 0..EMDRV_DMADRV_DMA_CH_COUNT.
/// On LDMA, this will configure channel 0 to CH_PRIORITY - 1 as fixed priority,
/// and CH_PRIORITY to CH_COUNT as round-robin.
/// On DMA, this will have no impact, since high priority is unuseable with
/// peripherals.
#ifndef EMDRV_DMADRV_DMA_CH_PRIORITY
#define EMDRV_DMADRV_DMA_CH_PRIORITY 0
#endif

/// DMADRV channel count configuration option.
/// Number of DMA channels to support. A lower DMA channel count will reduce
/// ram memory footprint.
#ifndef EMDRV_DMADRV_DMA_CH_COUNT
#define EMDRV_DMADRV_DMA_CH_COUNT DMA_CHAN_COUNT
#endif

/// DMADRV native API configuration option.
/// Use the native emlib api of the DMA controller, but still use DMADRV
/// housekeeping functions as AllocateChannel/FreeChannel etc.
#if defined(DOXY_DOC_ONLY)
#define EMDRV_DMADRV_USE_NATIVE_API
#else
//#define EMDRV_DMADRV_USE_NATIVE_API
#endif

/** @} (end addtogroup DMADRV) */
/** @} (end addtogroup emdrv) */

#endif // DMADRV_CONFIG_H
