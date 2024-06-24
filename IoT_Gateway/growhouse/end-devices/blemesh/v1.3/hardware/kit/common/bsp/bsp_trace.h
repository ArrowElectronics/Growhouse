/***************************************************************************//**
 * @file
 * @brief SWO Trace API (for eAProfiler)
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __BSP_TRACE_H
#define __BSP_TRACE_H

#include "em_device.h"
#if (defined(BSP_ETM_TRACE) && defined(ETM_PRESENT)) \
  || defined(GPIO_ROUTE_SWOPEN)                      \
  || defined(GPIO_ROUTEPEN_SWVPEN)

#include <stdint.h>
#include <stdbool.h>
#if defined(HAL_CONFIG)
#include "tracehalconfig.h"
#else
#include "traceconfig.h"
#endif

/***************************************************************************//**
 * @addtogroup BSP
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup BSPCOMMON API common for all kits
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BSP_ETM_TRACE) && defined(ETM_PRESENT)
void BSP_TraceEtmSetup(void);
#endif

#if defined(GPIO_ROUTE_SWOPEN) || defined(_GPIO_ROUTEPEN_SWVPEN_MASK)
bool BSP_TraceProfilerSetup(void);
void BSP_TraceSwoSetup(void);
#endif

#ifdef __cplusplus
}
#endif

/** @} (end group BSP) */
/** @} (end group BSP) */

#endif /* (defined(BSP_ETM_TRACE) && defined( ETM_PRESENT )) || defined( GPIO_ROUTE_SWOPEN ) */
#endif /* __BSP_TRACE_H */
