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
*                                                   LOGGING
*
* File : logging.h
*
* @defgroup COMMON_LOGGING Logging API
* @ingroup  COMMON
* @brief      Logging API
*
* @addtogroup COMMON_LOGGING
* @{
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _LOGGING_H_
#define  _LOGGING_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>

#include  <rtos_cfg.h>


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

#if (RTOS_CFG_LOG_EN == DEF_ENABLED)
CPU_BOOLEAN  Log_DataIsAvail(void);

void         Log_Output     (void);                                        /* Output buffered messages to the log output.          */
#endif

#ifdef __cplusplus
}
#endif


/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                            MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of logging module include.                       */
