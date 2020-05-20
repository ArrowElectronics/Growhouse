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
*                                           INTERNAL SPI API
*
* File : spi_priv.h
*********************************************************************************************************
*/

#ifndef  _SPI_PRIV_H_
#define  _SPI_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <io/source/serial/serial_ctrlr_priv.h>

#include  <io/include/serial.h>
#include  <io/include/spi.h>
#include  <io/include/spi_slave.h>


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

                                                                /* -------------- SPI MODULE ROOT STRUCT -------------- */
typedef  struct  spi {
    MEM_DYN_POOL   SlaveHandleMemPool;                          /* Pool of slave handle.                                */
    SLIST_MEMBER  *BusHandleListHeadPtr;                        /* Ptr to head of bus handle list.                      */
} SPI;


                                                                /* ------------------ SPI BUS STATE ------------------- */
typedef  enum  spi_bus_state {
    SPI_BUS_STATE_FAULT        = -1,                            /* Fault.                                               */
    SPI_BUS_STATE_STOPPED      =  0,                            /* Bus stopped.                                         */
    SPI_BUS_STATE_IDLE         =  1,                            /* Bus/driver has been started                          */
    SPI_BUS_STATE_ACTIVE       =  2                             /* Slave select has been enabled & bus is rdy for comm  */
} SPI_BUS_STATE;


/*
****************************************************************************************************
*                                           SPI BUS HANDLE STRUCT
****************************************************************************************************
*/

struct  spi_bus_handle {
           SLIST_MEMBER      ListNode;                          /* Bus handle list node.                                */
           SERIAL_HANDLE     SerialHandle;                      /* Handle on serial controller.                         */

    const  CPU_CHAR         *NamePtr;                           /* Pointer to platform manager item name.               */
           SPI_BUS_STATE     State;                             /* Current SPI bus state.                               */
           KAL_LOCK_HANDLE   Lock;                              /* Handle on SPI bus lock.                              */

           SERIAL_CFG        SerCfgCur;                         /* Current serial controller cfgs.                      */
};


/*
*********************************************************************************************************
*                                           SPI SLAVE HANDLE
*********************************************************************************************************
*/

struct  spi_slave_handle {
    CPU_INT16U       SlaveID;                                   /* Numeral ID of slave.                                 */
    CPU_BOOLEAN      ActiveLow;                                 /* Flag that indicates if slave is active low or high.  */

    KAL_LOCK_HANDLE  LockHandle;                                /* Handle on slave lock.                                */
    CPU_BOOLEAN      IsSlaveSelActive;                          /* Flag indicating if slave currently active.           */
    SPI_BUS_HANDLE   BusHandle;                                 /* Handle on SPI bus.                                   */
    SERIAL_CFG       SerialCfg;                                 /* Slave's serial cfgs.                                 */
    CPU_INT08U       TxDummyByte;                               /* Byte to send on RX only operations.                  */
};


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

extern  SPI  *SPI_Ptr;

extern  SPI_INIT_CFG  SPI_InitCfg;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void  SPI_Init         (       RTOS_ERR        *p_err);

void  SPI_BusSlaveSel  (       SPI_BUS_HANDLE   bus_handle,
                               CPU_INT16U       slave_id,
                               CPU_BOOLEAN      active_low,
                               SERIAL_CFG      *p_ser_cfg,
                               CPU_INT08U       tx_dummy_byte,
                               CPU_INT32U       timeout_ms,
                               CPU_BOOLEAN      is_nonblocking,
                               RTOS_ERR        *p_err);

void  SPI_BusSlaveDesel(       SPI_BUS_HANDLE   bus_handle,
                               CPU_INT16U       slave_id,
                               CPU_BOOLEAN      active_low,
                               RTOS_ERR        *p_err);

void  SPI_BusXfer      (       SPI_BUS_HANDLE   bus_handle,
                               CPU_INT08U      *p_buf_rx,
                        const  CPU_INT08U      *p_buf_tx,
                               CPU_INT32U       buf_len,
                               CPU_INT32U       timeout_ms,
                               RTOS_ERR        *p_err);

void  SPI_BusRx        (       SPI_BUS_HANDLE   bus_handle,
                               CPU_INT08U      *p_buf,
                               CPU_INT32U       buf_len,
                               CPU_INT32U       timeout_ms,
                               RTOS_ERR        *p_err);

void  SPI_BusTx        (       SPI_BUS_HANDLE   bus_handle,
                        const  CPU_INT08U      *p_buf,
                               CPU_INT32U       buf_len,
                               CPU_INT32U       timeout_ms,
                               RTOS_ERR        *p_err);
#endif
