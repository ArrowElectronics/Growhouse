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

/**
*********************************************************************************************************
*
*                                               SPI SLAVE API
*
* File : spi_slave.h
*
* @addtogroup IO_SPI
* @{
*********************************************************************************************************
*/

#ifndef  _SPI_SLAVE_H_
#define  _SPI_SLAVE_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <io/include/spi.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ------------------- SPI OPTIONS -------------------- */
#define  SPI_OPT_NONE                             DEF_BIT_NONE
#define  SPI_OPT_NON_BLOCKING                     DEF_BIT_00


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           SPI SLAVE HANDLE
*********************************************************************************************************
*/

typedef  struct  spi_slave_handle  *SPI_SLAVE_HANDLE;


/*
*********************************************************************************************************
*                                               SPI OPTIONS
*********************************************************************************************************
*/
typedef  CPU_INT32U  SPI_OPT;


/**
*********************************************************************************************************
*                                           SPI SLAVE INFORMATION
*********************************************************************************************************
*/

typedef  struct  spi_slave_info {
    CPU_INT08U   Mode;                                          /**< CPHA (bit 0) and CPOL (bit 1)                        */
    CPU_INT08U   BitsPerFrame;                                  /**< Transfer frame size (usually a multiple of 8)        */
    CPU_BOOLEAN  LSbFirst;                                      /**< Data output bit order                                */
    CPU_INT32U   SClkFreqMax;                                   /**< Max serial clock frequency in Hz                     */
    CPU_INT16U   SlaveID;                                       /**< Slave numeral ID.                                    */
    CPU_INT08U   TxDummyByte;                                   /**< Dummy byte to transmit during read only operations.  */
    CPU_BOOLEAN  ActiveLow;                                     /**< Flag that indicates if slave is active low or high.  */
} SPI_SLAVE_INFO;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

extern  const  SPI_SLAVE_HANDLE  SPI_SlaveHandleNull;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
* Note(s) : (1) Some SPI driver may provide support for additional capabilities. If the underlying driver
*               does not provide support for any of these capabilities, these functions will return an
*               error.
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef  __cplusplus
extern "C" {
#endif

SPI_SLAVE_HANDLE  SPI_SlaveOpen (       SPI_BUS_HANDLE     bus_handle,
                                 const  SPI_SLAVE_INFO    *p_slave_info,
                                        RTOS_ERR          *p_err);

void              SPI_SlaveClose(       SPI_SLAVE_HANDLE   slave_handle,
                                        RTOS_ERR          *p_err);


void              SPI_SlaveSel  (       SPI_SLAVE_HANDLE   slave_handle,
                                        CPU_INT32U         timeout_ms,
                                        SPI_OPT            opt,
                                        RTOS_ERR          *p_err);

void              SPI_SlaveDesel(       SPI_SLAVE_HANDLE   slave_handle,
                                        RTOS_ERR          *p_err);

void              SPI_SlaveRx   (       SPI_SLAVE_HANDLE   slave_handle,
                                        CPU_INT08U        *p_buf,
                                        CPU_INT32U         buf_len,
                                        CPU_INT32U         timeout_ms,
                                        RTOS_ERR          *p_err);

void              SPI_SlaveTx   (       SPI_SLAVE_HANDLE   slave_handle,
                                        CPU_INT08U        *p_buf,
                                        CPU_INT32U         buf_len,
                                        CPU_INT32U         timeout_ms,
                                        RTOS_ERR          *p_err);

void              SPI_SlaveXfer (       SPI_SLAVE_HANDLE   slave_handle,
                                        CPU_INT08U        *p_buf_rx,
                                        CPU_INT08U        *p_buf_tx,
                                        CPU_INT32U         buf_len,
                                        CPU_INT32U         timeout_ms,
                                        RTOS_ERR          *p_err);

#ifdef  __cplusplus
}
#endif


#endif

/** @} */
