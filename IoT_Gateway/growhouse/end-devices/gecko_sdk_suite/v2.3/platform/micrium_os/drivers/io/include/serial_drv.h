/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   IO
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
*                                               SERIAL DRV
*
*                                               PUBLIC API
*
* File : serial_drv.h
*********************************************************************************************************
*/


#ifndef  _SERIAL_DRV_H_
#define  _SERIAL_DRV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <io/include/serial.h>


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

typedef  struct  serial_ctrlr_hw_info_ext_siliconlabs {
    CPU_INT08U  PortLocationTx;
    CPU_INT08U  PortLocationRx;
    CPU_INT08U  PortLocationClk;
    CPU_INT08U  PortLocationCs;
} SERIAL_CTRLR_HW_INFO_EXT_SILICONLABS;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               GLOBALS
*********************************************************************************************************
*********************************************************************************************************
*/

extern  const  SERIAL_CTRLR_DRV_API  Serial_CtrlrDrv_API_SiliconLabsGeckoSDK;

#endif
