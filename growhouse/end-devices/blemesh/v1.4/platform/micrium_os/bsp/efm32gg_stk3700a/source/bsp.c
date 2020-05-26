/***************************************************************************//**
 * @file
 * @brief BSP Module - Silicon Labs - Efm32Gg-Stk3700A
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_utils.h>

#include  <bsp/siliconlabs/generic/include/bsp_os.h>
#include  "../include/bsp_clk.h"
#include  "../include/bsp_led.h"

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)
#include  <fs/include/fs_nand.h>
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
#include  <usb/include/usb_ctrlr.h>
#endif

//                                                                 Third Party Library Includes
#include  <em_chip.h>
#include  <em_cmu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)
BSP_HW_INFO_EXT(const FS_NAND_HW_INFO, BSP_FS_NAND_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_DEV_AVAIL)
BSP_HW_INFO_EXT(const USBD_DEV_CTRLR_HW_INFO, BSP_USBD_EFM32GG_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL)
BSP_HW_INFO_EXT(const USBH_PBHCI_HC_HW_INFO, BSP_USBH_EFM32GG_PBHCI_HW_Info);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           BSP_SystemInit()
 *
 * Description : Initialize the Board Support Package (BSP).
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) This function MUST be called before any other BSP function is called.
 *******************************************************************************************************/
void BSP_SystemInit(void)
{
  CHIP_Init();                                                  // Chip revision alignment and errata fixes

  BSP_ClkInit();                                                // Initialize clock settings for any peripherals

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);              // Select reference clock for High Freq. clock
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
}

/********************************************************************************************************
 *                                            BSP_OS_Init()
 *
 * Description : Initializes peripheral such as I2C.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) This function must be called after KAL has been initialized.
 *******************************************************************************************************/
void BSP_OS_Init(void)
{
  BSP_LED_Init();                                               // Initialize the I/Os for the LED controls

#if defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)
  FS_NAND_HW_INFO_REG("nand0", &BSP_FS_NAND_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
  USB_CTRLR_HW_INFO_REG("usb0", &BSP_USBD_EFM32GG_HwInfo, &BSP_USBH_EFM32GG_PBHCI_HW_Info);
#endif
}
