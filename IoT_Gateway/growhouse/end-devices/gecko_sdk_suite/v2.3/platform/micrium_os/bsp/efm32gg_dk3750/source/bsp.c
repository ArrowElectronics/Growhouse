/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing:
*   The licensor of this EXAMPLE CODE is Silicon Laboratories Inc.
*
*   Silicon Laboratories Inc. grants you a personal, worldwide, royalty-free, fully paid-up license to
*   use, copy, modify and distribute the EXAMPLE CODE software, or portions thereof, in any of your
*   products.
*
*   Your use of this EXAMPLE CODE is at your own risk. This EXAMPLE CODE does not come with any
*   warranties, and the licensor disclaims all implied warranties concerning performance, accuracy,
*   non-infringement, merchantability and fitness for your application.
*
*   The EXAMPLE CODE is provided "AS IS" and does not come with any support.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: https://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                            SILICON LABS
*                                           EFM32GG-DK3750
*
* File : bsp.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  BSP_MODULE
#include  <bsp/include/bsp_micriumos.h>
#include  <common/include/lib_utils.h>
#include  <rtos_description.h>

#include  <bsp/siliconlabs/efm32gg_dk3750/source/bsp_mcu.h>

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
#include  <usb/include/usb_ctrlr.h>
#endif

#ifdef RTOS_MODULE_IO_SERIAL_AVAIL
#include  <io/include/serial.h>
#endif

#ifdef RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
#include  <fs/include/fs_sd_spi.h>
#endif

#include  <em_chip.h>
#include  <em_cmu.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

#if defined(RTOS_MODULE_USB_DEV_AVAIL)
BSP_HW_INFO_EXT(const  USBD_DEV_CTRLR_HW_INFO, BSP_USBD_EFM32GG_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL)
BSP_HW_INFO_EXT(const  USBH_PBHCI_HC_HW_INFO,  BSP_USBH_EFM32GG_PBHCI_HW_Info);
#endif

#ifdef RTOS_MODULE_IO_SERIAL_AVAIL
BSP_HW_INFO_EXT(const  SERIAL_CTRLR_DRV_INFO, BSP_Serial_SiliconLabs_Bus0_DrvInfo);
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            BSP_SystemInit()
*
* Description : Initialize the Board Support Package (BSP). This function is called by the startup
*               module before initializing the kernel.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_SystemInit (void)
{
    CHIP_Init();                                                /* Chip revision alignment and errata fixes             */
    CMU_ClockSelectSet(cmuClock_HF, /*cmuSelect_HFXO*/cmuSelect_HFRCO);            /* Select reference clock for High Freq. clock          */
    CMU_OscillatorEnable(cmuOsc_LFXO, true, false);

    BSP_GeckoInit(BSP_INIT_DK_EBI);                                  /* Allows to enable modules on DK3750 board.            */
}


/*
*********************************************************************************************************
*                                           BSP_PeriphInit()
*
* Description : Initialize the Board Support Package (BSP). This function is called from the start task
*               after initializing the kernel.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  BSP_PeriphInit (void)
{
#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
    USB_CTRLR_HW_INFO_REG("usb0", &BSP_USBD_EFM32GG_HwInfo, &BSP_USBH_EFM32GG_PBHCI_HW_Info);
#endif

#ifdef RTOS_MODULE_IO_SERIAL_AVAIL
    IO_SERIAL_CTRLR_REG("spi0", &BSP_Serial_SiliconLabs_Bus0_DrvInfo);

#ifdef RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL
    FS_SD_SPI_HW_INFO_REG("sd0", "spi0", 0u);
#endif
#endif
}
