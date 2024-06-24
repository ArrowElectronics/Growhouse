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
*                                       IO SERIAL DECLARATIONS
*
* File : serial_priv.h
*********************************************************************************************************
*/

#ifndef  _SERIAL_PRIV_H_
#define  _SERIAL_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/source/serial/serial_ctrlr_priv.h>
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

extern  const  SERIAL_HANDLE  Serial_HandleNull;


/*
*********************************************************************************************************
*                                           ISR HANDLER FUNCTION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                   SPI MODULE CONFIGURATION STRUCT
*********************************************************************************************************
*/



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


void           Serial_Init             (       RTOS_ERR           *p_err);

SERIAL_HANDLE  Serial_CtrlrAdd         (const  CPU_CHAR           *name,
                                               CPU_INT08U          mode,
                                               RTOS_ERR           *p_err);

SERIAL_HANDLE  Serial_HandleGetFromName(const  CPU_CHAR           *name);

void           Serial_Start            (       SERIAL_HANDLE       handle,
                                               CPU_INT08U          mode,
                                               RTOS_ERR           *p_err);

void           Serial_Stop             (       SERIAL_HANDLE       handle,
                                               RTOS_ERR           *p_err);

CPU_BOOLEAN    Serial_CfgChk           (       SERIAL_HANDLE       handle,
                                               SERIAL_CFG         *p_cfg,
                                               RTOS_ERR           *p_err);

void           Serial_Cfg              (       SERIAL_HANDLE       handle,
                                               SERIAL_CFG         *p_cfg,
                                               RTOS_ERR           *p_err);

void           Serial_SlaveSel         (       SERIAL_HANDLE       handle,
                                        const  SERIAL_SLAVE_INFO  *p_slave_info,
                                               RTOS_ERR           *p_err);

void           Serial_SlaveDesel       (       SERIAL_HANDLE       handle,
                                        const  SERIAL_SLAVE_INFO  *p_slave_info,
                                               RTOS_ERR           *p_err);

void           Serial_TxDummyByteSet   (       SERIAL_HANDLE       handle,
                                               CPU_INT08U          tx_dummy_byte);

void           Serial_Rx               (       SERIAL_HANDLE       handle,
                                               CPU_INT08U         *p_buf,
                                               CPU_INT32U          buf_len,
                                               CPU_INT32U          timeout_ms,
                                               RTOS_ERR           *p_err);

void           Serial_Tx               (       SERIAL_HANDLE       handle,
                                        const  CPU_INT08U         *p_buf,
                                               CPU_INT32U          buf_len,
                                               CPU_INT32U          timeout_ms,
                                               RTOS_ERR           *p_err);

void           Serial_Xfer             (       SERIAL_HANDLE       handle,
                                               CPU_INT08U         *p_rx_buf,
                                        const  CPU_INT08U         *p_tx_buf,
                                               CPU_INT32U          buf_len,
                                               CPU_INT32U          timeout_ms,
                                               RTOS_ERR           *p_err);

void           Serial_LoopbackEn       (       SERIAL_HANDLE       handle,
                                               CPU_BOOLEAN         en,
                                               RTOS_ERR           *p_err);


#ifdef  __cplusplus
}
#endif

#endif                                                          /* _SPI_H_                                              */
