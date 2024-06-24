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
*                                           PUBLIC SPI API
*
* File : spi.h
*
* @defgroup IO_SPI SPI API
* @ingroup  IO
* @brief      SPI API
*
* @addtogroup IO_SPI
* @{
*********************************************************************************************************
*/

#ifndef  _SPI_H_
#define  _SPI_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/platform_mgr.h>
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

                                                                /* ------------------ SPI BUS HANDLE ------------------ */
typedef  struct  spi_bus_handle * SPI_BUS_HANDLE;

                                                                /* ------------------- SPI INIT CFG ------------------- */
typedef  struct  spi_init_cfg {
    CPU_SIZE_T   SlaveHandleQty;                                /* Max quantity of slave handle.                        */
    MEM_SEG     *MemSegPtr;                                     /* Ptr to mem segment from where to alloc internal data.*/
} SPI_INIT_CFG;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

extern  const  SPI_BUS_HANDLE  SPI_BusHandleNull;

extern  const  SPI_INIT_CFG    SPI_InitCfgDflt;


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

void            SPI_ConfigureMemSeg        (       MEM_SEG         *p_seg);

void            SPI_ConfigureSlaveHandleQty(       CPU_SIZE_T       handle_nbr);

SPI_BUS_HANDLE  SPI_BusAdd                 (const  CPU_CHAR        *name,
                                                   RTOS_ERR        *p_err);

SPI_BUS_HANDLE  SPI_BusHandleGetFromName   (const  CPU_CHAR        *name);

void            SPI_BusStart               (       SPI_BUS_HANDLE   bus_handle,
                                                   RTOS_ERR        *p_err);

void            SPI_BusStop                (       SPI_BUS_HANDLE   bus_handle,
                                                   RTOS_ERR        *p_err);

void            SPI_BusLoopBackEn          (       SPI_BUS_HANDLE   bus_handle,
                                                   CPU_BOOLEAN      en,
                                                   RTOS_ERR        *p_err);

#ifdef  __cplusplus
}
#endif

#endif

/** @} */
