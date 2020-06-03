/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
*
*                               (c) Copyright 2013; Silicon Laboratories Inc.
*                                           https://www.micrium.com
*********************************************************************************************************
* Licensing:
*           YOUR USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS OF A MICRIUM SOFTWARE LICENSE.
*   If you are not willing to accept the terms of an appropriate Micrium Software License, you must not
*   download or use this software for any reason.
*   Information about Micrium software licensing is available at https://www.micrium.com/licensing/
*   It is your obligation to select an appropriate license based on your intended use of the Micrium OS.
*   Unless you have executed a Micrium Commercial License, your use of the Micrium OS is limited to
*   evaluation, educational or personal non-commercial uses. The Micrium OS may not be redistributed or
*   disclosed to any third party without the written consent of Silicon Laboratories Inc.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software. For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*
*                                       COMMON BASIC OPERATIONS
*
* File : common.h
*
* @defgroup COMMON Common API
* @brief      Common API
*
* @defgroup LIB LIB API
* @ingroup  COMMON
* @brief      LIB API
*
* @defgroup COMMON_CORE Common Core API
* @ingroup  COMMON
* @brief      Common Core API
*
* @addtogroup COMMON_CORE
* @{
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _COMMON_H_
#define  _COMMON_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <rtos_cfg.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                           LOGGING CFG STRUCTURE
* @brief   Logging configuration
*********************************************************************************************************
*/

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
typedef  struct  common_cfg_logging {
    CPU_INT16U  AsyncBufSize;                                   /**< Size of buf used for asynchronous logging.           */
} COMMON_CFG_LOGGING;
#endif


/**
*********************************************************************************************************
*                                           INIT CFG STRUCTURE
* @brief   Initialization configuration
*********************************************************************************************************
*/
typedef  struct  common_init_cfg {
    MEM_SEG             *CommonMemSegPtr;                       /**< Ptr to common module's mem seg.                      */

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
    COMMON_CFG_LOGGING   LoggingCfg;                            /**< Logging sub-module cfg.                              */
    MEM_SEG             *LoggingMemSegPtr;                      /**< Ptr to logging sub-module's mem seg.                 */
#endif
} COMMON_INIT_CFG;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern  const  COMMON_INIT_CFG  Common_InitCfgDflt;
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif

#if ((RTOS_CFG_LOG_EN                      == DEF_ENABLED) && \
     (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void  Common_ConfigureLogging       (COMMON_CFG_LOGGING  *p_cfg);
#endif

#if ((RTOS_CFG_LOG_EN                      == DEF_ENABLED) && \
     (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED))
void  Common_ConfigureMemSegLogging (MEM_SEG             *p_mem_seg);
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void  Common_ConfigureMemSeg        (MEM_SEG             *p_mem_seg);
#endif

void  Common_Init                   (RTOS_ERR            *p_err);

#ifdef __cplusplus
}
#endif


/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                          MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of common module include.                        */
