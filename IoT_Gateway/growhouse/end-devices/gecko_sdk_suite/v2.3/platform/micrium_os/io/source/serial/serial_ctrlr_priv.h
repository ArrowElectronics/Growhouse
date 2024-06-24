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
*                                   IO SERIAL CONTROLLER DECLARATIONS
*
* File : serial_ctrlr_priv.h
*********************************************************************************************************
*/

#ifndef  _SERIAL_CTRLR_PRIV_H_
#define  _SERIAL_CTRLR_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <io/include/serial.h>



/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  SERIAL_ABORT_DIR_RX   DEF_BIT_00
#define  SERIAL_ABORT_DIR_TX   DEF_BIT_01

#define  SERIAL_IOCTRL_CMD_LOOPBACK        1
#define  SERIAL_IOCTRL_CMD_HW_FLOW_CTRL    2
#define  SERIAL_IOCTRL_CMD_BREAK_SIGNAL    3
#define  SERIAL_IOCTRL_CMD_DTR             4
#define  SERIAL_IOCTRL_CMD_RTS             5


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ------------------ SERIAL PARITY ------------------ */
typedef  enum  serial_parity {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
} SERIAL_PARITY;

                                                                /* ----------------- SERIAL STOP BIT ----------------- */
typedef  enum  serial_stop_bit {
    SERIAL_STOP_BIT_NONE,
    SERIAL_STOP_BIT_1,
    SERIAL_STOP_BIT_1_5,
    SERIAL_STOP_BIT_2
} SERIAL_STOP_BIT;

                                                                /* ----------------- SERIAL CFG STRUCT ---------------- */
typedef  struct  serial_cfg {
    CPU_INT32U       Baudrate;                                  /* Baudrate, in bps.                                    */
    CPU_INT08U       FrameSize;                                 /* Transfer frame size  (in bits).                      */
    SERIAL_PARITY    ParityMode;                                /* Parity mode.                                         */
    SERIAL_STOP_BIT  StopBit;                                   /* Number of stop bits.                                 */
    CPU_INT08U       Mode;                                      /* CPHA (bit 0) and CPOL (bit 1)                        */
    CPU_BOOLEAN      LSB_First;                                 /* Data output bit order                                */
} SERIAL_CFG;


typedef  struct  serial_handle *  SERIAL_HANDLE;


/*
*********************************************************************************************************
*                                           ISR HANDLER FUNCTION
*********************************************************************************************************
*/

struct  serial_ctrlr_drv_api {
                                                                /* Init driver                                          */
    void         (*Init)       (       SERIAL_DRV         *p_ser_drv,
                                       SERIAL_HANDLE       handle,
                                       MEM_SEG            *p_mem_seg,
                                       RTOS_ERR           *p_err);

                                                                /* Starts SPI hardware                                  */
    void         (*Start)      (       SERIAL_DRV         *p_ser_drv,
                                       CPU_INT08U          mode,
                                       RTOS_ERR           *p_err);

                                                                /* Stops SPI hardware                                   */
    void         (*Stop)       (       SERIAL_DRV         *p_ser_drv,
                                       RTOS_ERR           *p_err);

                                                                /* Validate ctrlr supports cfg.                         */
    CPU_BOOLEAN  (*CfgChk)     (       SERIAL_DRV         *p_ser_drv,
                                const  SERIAL_CFG         *p_serial_cfg,
                                       RTOS_ERR           *p_err);

                                                                /* Configure SPI bus parameters                         */
    void         (*Cfg)        (       SERIAL_DRV         *p_ser_drv,
                                const  SERIAL_CFG         *p_serial_cfg,
                                       RTOS_ERR           *p_err);

                                                                /* Enables slave-select                                 */
    void         (*SlaveSel)   (       SERIAL_DRV         *p_ser_drv,
                                const  SERIAL_SLAVE_INFO  *p_slave_info,
                                       RTOS_ERR           *p_err);

                                                                /* Disables slave-select                                */
    void         (*SlaveDesel) (       SERIAL_DRV         *p_ser_drv,
                                const  SERIAL_SLAVE_INFO  *p_slave_info,
                                       RTOS_ERR           *p_err);


                                                                /* Transfers data over the SPI bus                      */
    void         (*Rx)         (       SERIAL_DRV         *p_ser_drv,
                                       CPU_INT08U         *p_buf,
                                       CPU_INT32U          buf_len,
                                       RTOS_ERR           *p_err);

                                                                /* Transfers data over the SPI bus                      */
    void         (*Tx)         (       SERIAL_DRV         *p_ser_drv,
                                const  CPU_INT08U         *p_buf,
                                       CPU_INT32U          buf_len,
                                       RTOS_ERR           *p_err);

                                                                /* Transmits single byte 'len' times.                   */
    void         (*ByteTx)     (       SERIAL_DRV         *p_ser_drv,
                                       CPU_INT08U          byte,
                                       CPU_INT32U          len,
                                       RTOS_ERR           *p_err);

                                                                /* Transfers data over the SPI bus                      */
    void         (*Abort)      (       SERIAL_DRV         *p_ser_drv,
                                       CPU_INT08U          dir,
                                       RTOS_ERR           *p_err);

                                                                /* Handles driver-specific I/O control commands         */
    void         (*IO_Ctrl)    (       SERIAL_DRV         *p_ser_drv,
                                       CPU_INT32S          cmd,
                                       void               *p_arg,
                                       RTOS_ERR           *p_err);

                                                                /* Retrieves required buffer alignment.                 */
    CPU_SIZE_T   (*ReqAlignGet)(       SERIAL_DRV         *p_ser_drv);
};

                                                                /* ----------------- SERIAL DRV STRUCT ---------------- */
struct  serial_drv {
           void                  *DataPtr;

    const  SERIAL_CTRLR_HW_INFO  *HW_InfoPtr;
    const  SERIAL_CTRLR_DRV_API  *DrvAPI_Ptr;
    const  SERIAL_CTRLR_BSP_API  *BSP_API_Ptr;
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void  Serial_EventRxCmpl(SERIAL_HANDLE   handle,
                         RTOS_ERR        err);

void  Serial_EventTxCmpl(SERIAL_HANDLE   handle,
                         RTOS_ERR        err);

#endif
