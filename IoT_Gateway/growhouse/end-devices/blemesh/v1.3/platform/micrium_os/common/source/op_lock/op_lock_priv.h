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
*                                       OPERATION LOCKS (OpLocks)
*
* File : op_lock_priv.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _OP_LOCK_PRIV_H_
#define  _OP_LOCK_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/source/kal/kal_priv.h>
#include  <cpu/include/cpu.h>

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OP_LOCK_TIMEOUT_INFINITE                   KAL_TIMEOUT_INFINITE


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef  RTOS_MODULE_FS_AVAIL
typedef  CPU_INT32U  OP_LOCK_BITMAP;
#else
typedef  CPU_INT08U  OP_LOCK_BITMAP;
#endif

typedef  struct  op_lock_op_desc {                              /* ------------------ OP LOCK OP DESC ----------------- */
    OP_LOCK_BITMAP  Policy;
    OP_LOCK_BITMAP  Mask;
} OP_LOCK_OP_DESC;

typedef  struct  op_lock {                                      /* --------------------- OP LOCK ---------------------- */
    KAL_MON_HANDLE  Mon;
    OP_LOCK_BITMAP  OpCnt;
} OP_LOCK;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void  OpLockCreate (OP_LOCK          *p_op_lock,
                    RTOS_ERR         *p_err);

void  OpLockAcquire(OP_LOCK          *p_op_lock,
                    OP_LOCK_OP_DESC  *p_op_desc,
                    CPU_INT32U        timeout_ms,
                    RTOS_ERR         *p_err);

void  OpLockRelease(OP_LOCK          *p_op_lock,
                    OP_LOCK_OP_DESC  *p_op_desc);

void  OpLockDel    (OP_LOCK          *p_op_lock);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                      /* End of private op lock module include.                   */
