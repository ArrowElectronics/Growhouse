/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
*
*                               (c) Copyright 2017; Silicon Laboratories Inc.
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
*                                               RTOS TYPES
*
* File : rtos_types.h
*
* @defgroup COMMON_TYPES Types API
* @ingroup  COMMON
* @brief      Types API
*
* @addtogroup COMMON_TYPES
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

#ifndef  _RTOS_TYPES_H_
#define  _RTOS_TYPES_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/kal.h>
#include  <cpu/include/cpu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                           RTOS TASK DATA TYPE
* @{
*********************************************************************************************************
*/

typedef  KAL_TASK_PRIO  RTOS_TASK_PRIO;

typedef  struct  rtos_task_cfg {
    RTOS_TASK_PRIO   Prio;                                      /**< Task priority.                                       */
    CPU_STK_SIZE     StkSizeElements;                           /**< Size of the stack, in CPU_STK elements.              */
    CPU_STK         *StkPtr;                                    /**< Pointer to base of the stack .                       */
} RTOS_TASK_CFG;

/** @} */

/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                          MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos types module include.                    */
