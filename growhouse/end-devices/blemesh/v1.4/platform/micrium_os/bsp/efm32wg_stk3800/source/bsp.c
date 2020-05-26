/***************************************************************************//**
 * @file
 * @brief Board Support Package - Silicon Labs - Efm32 Wonder Gecko Starter Kit
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

#define  BSP_MODULE
#include  <bsp/siliconlabs/generic/include/bsp_os.h>
#include  "../include/bsp_led.h"

#include  <common/include/lib_utils.h>
#include  <rtos_description.h>

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
#include  <usb/include/usb_ctrlr.h>
#endif

//                                                                 Third Party Library Includes
#include  <em_chip.h>
#include  <em_cmu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(RTOS_MODULE_USB_DEV_AVAIL)
BSP_HW_INFO_EXT(const USBD_DEV_CTRLR_HW_INFO, BSP_USBD_EFM32WG_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL)
BSP_HW_INFO_EXT(const USBH_PBHCI_HC_HW_INFO, BSP_USBH_EFM32WG_PBHCI_HW_Info);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
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
 *******************************************************************************************************/
void BSP_SystemInit(void)
{
  CHIP_Init();                                                  // Chip revision alignment and errata fixes
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);              // Select reference clock for High Freq. clock
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);

  CMU_ClockEnable(cmuClock_HFPER, DEF_TRUE);                    // Enable Peripheral clock signal
  CMU_ClockEnable(cmuClock_GPIO, DEF_TRUE);                     // Ebable GPIO clock signals (used for LEDs)

  BSP_LED_Init();                                               // Initialize the I/Os for the LED      controls.
}

/********************************************************************************************************
 *                                            BSP_OS_Init()
 *
 * Description : Initialize the Board Support Package (BSP). This function is called from the start task
 *               after initializing the kernel.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void BSP_OS_Init(void)
{
#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
  USB_CTRLR_HW_INFO_REG("usb0", &BSP_USBD_EFM32WG_HwInfo, &BSP_USBH_EFM32WG_PBHCI_HW_Info);
#endif
}
