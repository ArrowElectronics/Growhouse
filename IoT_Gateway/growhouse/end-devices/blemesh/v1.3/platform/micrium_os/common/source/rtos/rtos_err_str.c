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

/*
*********************************************************************************************************
*
*                                               ERROR STRINGS
*
* File : rtos_err_str.c
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_path.h>
#include  <rtos_err_cfg.h>

#include  <common/include/lib_def.h>


#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)                        /* Do not compile if error strings are turned off.      */

#include  <cpu/include/cpu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               LOCAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_ERR_STR_MODULE                                    /* This define must be present when including rtos_err.h*/

#undef   RTOS_ERR_DECLARE
#include  <common/source/preprocessor/preprocessor_priv.h>
#define  RTOS_ERR_DECLARE(err, str)  PP_STRINGIFY(err),

const  CPU_CHAR  *  const  RTOS_ErrStrTbl[] = {
#include  <common/include/rtos_err.h>
};

#undef   RTOS_ERR_DECLARE
#include  <common/source/preprocessor/preprocessor_priv.h>
#define  RTOS_ERR_DECLARE(err, str)  str,

const  CPU_CHAR  *  const  RTOS_ErrDescStrTbl[] = {
#include  <common/include/rtos_err.h>
};

#endif
