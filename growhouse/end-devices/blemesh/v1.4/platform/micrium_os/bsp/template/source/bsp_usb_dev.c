/***************************************************************************//**
 * @file
 * @brief Board Support Package (BSP) - USB-Device
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define    BSP_USB_MODULE

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>
#include  <rtos_description.h>

#ifdef  RTOS_MODULE_USB_DEV_AVAIL

#include  <usb/include/device/usbd_core.h>
#include  <drivers/usb/include/usbd_drv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- USB DEVICE FNCTS -----------------
static void BSP_USBD_Template_Init(USBD_DRV *p_drv);

static void BSP_USBD_Template_Conn(void);

static void BSP_USBD_Template_Disconn(void);

static void BSP_USBD_Template_ISR_Handler(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           USB DEVICE CFGS
 *******************************************************************************************************/

static USBD_DRV *BSP_USBD_Template_DrvPtr;                      // USB device driver specific info.

//                                                                 ---------- USB DEVICE ENDPOINTS INFO TBL -----------
static USBD_DRV_EP_INFO BSP_USBD_Template_EP_InfoTbl[] = {
  { USBD_EP_INFO_TYPE_CTRL                                                   | USBD_EP_INFO_DIR_OUT, 0u, 64u },
  { USBD_EP_INFO_TYPE_CTRL                                                   | USBD_EP_INFO_DIR_IN, 0u, 64u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 1u, 1024u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 1u, 1024u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 2u, 1024u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 2u, 1024u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 3u, 1024u },
  { USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 3u, 1024u },
  { DEF_BIT_NONE, 0u, 0u }
};

//                                                                 ---------- USB DEVICE DRIVER INFORMATION -----------
static USBD_DRV_INFO BSP_USBD_Template_DrvInfoPtr = {
  .BaseAddr = 0x00000000u,                                      // USB Controller register base address.
  .MemAddr = 0x00000000u,                                       // USB Controller deddicated mem base addr. 0 if none.
  .MemSize = 0u,                                                // USB Controller deddicated mem size. 0 if none.
  .Spd = USBD_DEV_SPD_HIGH,                                     // USB Controller maximum speed.
  .EP_InfoTbl = BSP_USBD_Template_EP_InfoTbl
};

//                                                                 ------------- USB DEVICE BSP STRUCTURE -------------
static USBD_DRV_BSP_API BSP_USBD_Template_BSP_API_Ptr = {
  BSP_USBD_Template_Init,
  BSP_USBD_Template_Conn,
  BSP_USBD_Template_Disconn
};

//                                                                 ----------- USB DEVICE HW INFO STRUCTURE -----------
const USBD_DEV_CTRLR_HW_INFO BSP_USBD_Template_HwInfo = {
  .DrvAPI_Ptr = DEF_NULL,                                       // TODO: Insert ptr to appropriate driver API structure.
  .DrvInfoPtr = &BSP_USBD_Template_DrvInfoPtr,
  .BSP_API_Ptr = &BSP_USBD_Template_BSP_API_Ptr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       BSP_USBD_Template_Init()
 *
 * Description : USB device controller board-specific initialization.
 *
 *                   (1) Enable    USB dev ctrl registers  and bus clock.
 *                   (2) Configure USB dev ctrl interrupts.
 *                   (3) Disable   USB dev transceiver Pull-up resistor in D+ line.
 *                   (4) Disable   USB dev transceiver clock.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_Template_Init(USBD_DRV *p_drv)
{
  BSP_USBD_Template_DrvPtr = p_drv;
  BSP_USBD_Template_ISR_Handler(); // Removed unused function warning.

  //                                                               TODO: Add initialization of USB controller related clk, IO, power, int, etc here.
}

/********************************************************************************************************
 *                                       BSP_USBD_Template_Conn()
 *
 * Description : Connect pull-up on DP.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_Template_Conn(void)
{
  //                                                               TODO: Add code needed to be executed on a connect request.
}

/********************************************************************************************************
 *                                     BSP_USBD_Template_Disconn()
 *
 * Description : Disconnect pull-up on DP.
 *
 * Argument(s) : none.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_Template_Disconn(void)
{
  //                                                               TODO: Add code needed to be executed on a disconnect request.
}

/********************************************************************************************************
 *                                     BSP_USBD_Template_ISR_Handler()
 *
 * Description : Provide p_drv pointer to the driver ISR Handler.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_Template_ISR_Handler(void)
{
  //                                                               TODO: Add code needed to acknowledge interrupt.
  BSP_USBD_Template_DrvPtr->API_Ptr->ISR_Handler(BSP_USBD_Template_DrvPtr);
}
#endif
