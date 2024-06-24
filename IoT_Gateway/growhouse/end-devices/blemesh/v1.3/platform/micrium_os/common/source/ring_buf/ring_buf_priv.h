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
*                                       RING BUFFER DATA STRUCTURE
*
* File : ring_buf_priv.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RING_BUF_PRIV_H_
#define  _RING_BUF_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       COMPILE-TIME NOT INIT MACRO
*********************************************************************************************************
*/

#define  RING_BUF_INIT_NULL()       \
{                                   \
    /* .StartPtr   = */ DEF_NULL,   \
    /* .Size       = */ 0u,         \
    /* .RdIx       = */ 0u,         \
    /* .RdIxNext   = */ 0u,         \
    /* .WrIx       = */ 0u,         \
    /* .WrIxPrev   = */ 0u,         \
    /* .EndValidIx = */ 0u,         \
    /* .IsReading  = */ DEF_NO,     \
    /* .RdIxDirty  = */ DEF_NO      \
}


/*
*********************************************************************************************************
*                                           INIT CHK MACRO
*********************************************************************************************************
*/

#define  RING_BUF_IS_NULL(p_ring_buf)         (((p_ring_buf)->StartPtr == DEF_NULL) ? DEF_YES : DEF_NO)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  struct  ring_buf {
    CPU_INT08U   *StartPtr;                                     /* Ptr to start of ring buf data.                       */
    CPU_INT32U    Size;                                         /* Size of ring buf alloc'd or to allocate.             */
    CPU_INT16U    RdIx;                                         /* Ix to where consumer will take/ is taking data.      */
    CPU_INT16U    RdIxNext;                                     /* Ix to where consumer will take next data.            */
    CPU_INT16U    WrIx;                                         /* Ix to where producer(s) will put  data.              */
    CPU_INT16U    WrIxPrev;                                     /* Ix to where producer was before writing to buf.      */
    CPU_INT16U    EndValidIx;                                   /* Ix to end of valid data, in the ring buf.            */
    CPU_BOOLEAN   IsReading;                                    /* Flag indicating if a read is currently in progress.  */
    CPU_BOOLEAN   RdIxDirty;                                    /* Flag indicating if the rd ix needs to be refreshed.  */
} RING_BUF;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void          RingBufCreate     (RING_BUF    *p_ring_buf,
                                 MEM_SEG     *p_mem_seg,
                                 CPU_INT16U   buf_size,
                                 RTOS_ERR    *p_err);

void         *RingBufWrAlloc    (RING_BUF    *p_ring_buf,
                                 CPU_INT16U   size);

void          RingBufWrCommit   (RING_BUF    *p_ring_buf);

CPU_BOOLEAN   RingBufWr         (RING_BUF    *p_ring_buf,
                                 CPU_INT16U   size,
                                 void        *p_start_data);

CPU_BOOLEAN   RingBufDataIsAvail(RING_BUF  *p_ring_buf);

CPU_BOOLEAN   RingBufRdStart    (RING_BUF    *p_ring_buf);

void         *RingBufRd         (RING_BUF    *p_ring_buf,
                                 CPU_SIZE_T   size);

void          RingBufRdEnd      (RING_BUF    *p_ring_buf);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                      /* End of private ring buf module include.                  */
