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
* File : platform_mgr.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _PLATFORM_MGR_H_
#define  _PLATFORM_MGR_H_


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


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           INIT CFG STRUCTURES
*********************************************************************************************************
*/

typedef  struct  platform_mgr_init_cfg {
    CPU_SIZE_T  PoolBlkQtyInit;
    CPU_SIZE_T  PoolBlkQtyMax;
} PLATFORM_MGR_INIT_CFG;


typedef  enum  platform_mgr_item_type {
    PLATFORM_MGR_ITEM_TYPE_GENERIC,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NAND,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_NOR,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_CARD,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SD_SPI,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_RAM_DISK,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SCSI,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_VDI,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_NET_ETHER,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_NET_WIFI,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_USB_HOST_COMPANION,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SERIAL_CTRLR,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_IO_SD_CTRLR,
    PLATFORM_MGR_ITEM_TYPE_HW_INFO_CAN
} PLATFORM_MGR_ITEM_TYPE;

typedef  struct  platform_mgr_item {
    PLATFORM_MGR_ITEM_TYPE   Type;
    const  CPU_CHAR         *StrID;
} PLATFORM_MGR_ITEM;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern  const  PLATFORM_MGR_INIT_CFG  PlatformMgr_InitCfgDflt;
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

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void                PlatformMgQtyConfigure   (PLATFORM_MGR_INIT_CFG    *p_cfg);
#endif

void                PlatformMgrItemInitAdd   (const PLATFORM_MGR_ITEM  *p_item);

PLATFORM_MGR_ITEM  *PlatformMgrItemGetByName (const  CPU_CHAR          *id_str,
                                              RTOS_ERR                 *p_err);

#ifdef __cplusplus
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of platform mgr module include.                  */
