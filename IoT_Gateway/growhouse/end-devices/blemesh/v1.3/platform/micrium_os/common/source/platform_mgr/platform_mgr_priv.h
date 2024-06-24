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
*                                           PLATFORM MANAGER
*
* File : platform_mgr_priv.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _PLATFORM_MGR_PRIV_H_
#define  _PLATFORM_MGR_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  CPU_BOOLEAN  (*PLATFORM_MGR_PREDICATE)(const  PLATFORM_MGR_ITEM  *p_item,
                                                void                      *p_arg);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void                PlatformMgrInit      (RTOS_ERR                 *p_err);

PLATFORM_MGR_ITEM  *PlatformMgrItemAlloc (CPU_INT32U                item_size,
                                          RTOS_ERR                 *p_err);

void                PlatformMgrItemAdd   (const PLATFORM_MGR_ITEM  *p_item,
                                          RTOS_ERR                 *p_err);

PLATFORM_MGR_ITEM  *PlatformMgrItemGet   (PLATFORM_MGR_PREDICATE    predicate_fnct,
                                          void                     *p_predicate_data,
                                          RTOS_ERR                 *p_err);

void                PlatformMgrItemRem   (const PLATFORM_MGR_ITEM  *p_item,
                                          RTOS_ERR                 *p_err);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of platform mgr module include.                  */
