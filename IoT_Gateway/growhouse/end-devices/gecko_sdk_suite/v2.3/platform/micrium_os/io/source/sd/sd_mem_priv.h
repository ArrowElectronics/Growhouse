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
*                                       IO SD MEM DECLARATIONS
*
* File : sd_mem_priv.h
*********************************************************************************************************
*/

#ifndef  _SD_MEM_PRIV_H_
#define  _SD_MEM_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/sd.h>
#include  <io/source/sd/sd_priv.h>



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

typedef  struct  sd_mem_drv_api {
    void  *(*CardConn)    (SD_FNCT_HANDLE   fnct_handle);

    void   (*CardDisconn) (void            *p_fnct_data);
} SD_MEM_DRV_API;


/*
*********************************************************************************************************
*                                           ISR HANDLER FUNCTION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef  __cplusplus
extern "C" {
#endif

void  SD_MEM_SingleBlkRd(SD_FNCT_HANDLE   fnct_handle,
                         CPU_INT08U      *p_dest,
                         CPU_INT32U       blk_addr,
                         RTOS_ERR        *p_err);

void  SD_MEM_MultBlkRd  (SD_FNCT_HANDLE   fnct_handle,
                         CPU_INT08U      *p_dest,
                         CPU_INT32U       blk_addr,
                         CPU_INT32U       blk_cnt,
                         RTOS_ERR        *p_err);

void  SD_MEM_SingleBlkWr(SD_FNCT_HANDLE   fnct_handle,
                         CPU_INT08U      *p_dest,
                         CPU_INT32U       blk_addr,
                         RTOS_ERR        *p_err);

void  SD_MEM_MultBlkWr  (SD_FNCT_HANDLE   fnct_handle,
                         CPU_INT08U      *p_dest,
                         CPU_INT32U       blk_addr,
                         CPU_INT32U       blk_cnt,
                         RTOS_ERR        *p_err);

void  SD_MEM_BlkInfoGet (SD_FNCT_HANDLE   fnct_handle,
                         CPU_INT32U      *p_blk_len,
                         CPU_INT32U      *p_blk_size,
                         RTOS_ERR        *p_err);

#ifdef  __cplusplus
}
#endif

#endif                                                          /* _SD_MEM_PRIV_H_                                      */
