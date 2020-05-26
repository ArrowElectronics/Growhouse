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
*                                           PUBLIC SD CARD INFO
*
* File : sd_card.h
*********************************************************************************************************
*/

#ifndef  _SD_CARD_H_
#define  _SD_CARD_H_


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
#include  <io/include/sd_card.h>
#include  <io/include/sd.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef         struct  sd_card_drv            SD_CARD_DRV;
typedef  const  struct  sd_card_ctrlr_drv_api  SD_CARD_CTRLR_DRV_API;


/*
*********************************************************************************************************
*                                           ISR HANDLER FUNCTION
*********************************************************************************************************
*/

typedef  void  (*SD_CARD_CTRLR_ISR_HANDLE_FNCT)(SD_CARD_DRV  *p_ser_drv);


/**
*********************************************************************************************************
*                                       SD card signal voltage
*********************************************************************************************************
*/

typedef  enum  sd_card_bus_signal_volt {
    SD_CARD_BUS_SIGNAL_VOLT_AUTO,
    SD_CARD_BUS_SIGNAL_VOLT_3_3,                                /**< 3.3 volts.                                         */
    SD_CARD_BUS_SIGNAL_VOLT_1_8                                 /**< 1.8 volts.                                         */
} SD_CARD_BUS_SIGNAL_VOLT;


/**
*********************************************************************************************************
*                                           BSP API structure
*********************************************************************************************************
*/

typedef  struct  sd_card_ctrlr_bsp_api {
                                                                /** Init BSP.                                           */
    CPU_BOOLEAN  (*Init  )         (SD_CARD_CTRLR_ISR_HANDLE_FNCT   isr_fnct,
                                    SD_CARD_DRV                    *p_sd_card_drv);

                                                                /** Enables serial controller clock.                    */
    CPU_BOOLEAN  (*ClkEn )         (void);

                                                                /** Init IO pin multiplexing.                           */
    CPU_BOOLEAN  (*IO_Cfg)         (void);

                                                                /** Init interrupt control.                             */
    CPU_BOOLEAN  (*IntCfg)         (void);

                                                                /** Init power control.                                 */
    CPU_BOOLEAN  (*PwrCfg)         (void);

                                                                /** Starts communication.                               */
    CPU_BOOLEAN  (*Start )         (void);

                                                                /** Stops communication.                                */
    CPU_BOOLEAN  (*Stop  )         (void);

                                                                /** Gets SD controller clock frequency, in hertz.       */
    CPU_INT32U   (*ClkFreqGet)     (void);

                                                                /** Sets the voltage for the signaling.                 */
    CPU_BOOLEAN  (*SignalVoltSet)  (SD_CARD_BUS_SIGNAL_VOLT         volt);

                                                                /** Allows to override capabilities from ctrlr drv.     */
    void         (*CapabilitiesGet)(SD_HOST_CAPABILITIES           *p_capabilities);

                                                                /** Pointer to extended BSP API. Refer to driver doc.   */
    void          *BSP_API_ExtPtr;
} SD_CARD_CTRLR_BSP_API;


/**
*********************************************************************************************************
*                               SD Card controller hardware information
*********************************************************************************************************
*/

typedef  struct  sd_card_ctrlr_hw_info {
           CPU_ADDR                  BaseAddr;                  /**< Controller's registers base address.               */
           SD_CARD_BUS_SIGNAL_VOLT   CardSignalVolt;            /**< Bus Voltage required by the SD Card.               */
           SD_CARD_DETECT_MODE       CardDetectMode;            /**< Mode to detect insertion/removal of SD Card.       */
    const  void                     *InfoExtPtr;                /**< Extended (driver specific) hardware information.   */
} SD_CARD_CTRLR_HW_INFO;


/**
*********************************************************************************************************
*                                   SD Card controller information
*********************************************************************************************************
*/

typedef  struct  sd_card_ctrlr_drv_info {
           SD_CARD_CTRLR_HW_INFO   HW_Info;                      /**< Hardware information structure.                   */
    const  SD_CARD_CTRLR_BSP_API  *BSP_API_Ptr;                  /**< Pointer to BSP API structure.                     */
    const  SD_CARD_CTRLR_DRV_API  *DrvAPI_Ptr;                   /**< Pointer to driver API structure.                  */
} SD_CARD_CTRLR_DRV_INFO;

#endif
