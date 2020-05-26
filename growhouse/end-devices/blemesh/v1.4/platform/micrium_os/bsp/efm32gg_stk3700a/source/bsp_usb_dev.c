/***************************************************************************//**
 * @file
 * @brief USB Board Support Package (BSP) - Silicon Labs - Efm32Gg-Stk3700A
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
#include  <kernel/include/os.h>
#include  <rtos_description.h>

#ifdef  RTOS_MODULE_USB_DEV_AVAIL

#include  <usb/include/device/usbd_core.h>
#include  <drivers/usb/include/usbd_drv.h>

#include  <em_cmu.h>
#include  <em_gpio.h>
#include  <em_device.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------- EFM32 USB ROUTE REGISTER ADDRESS ---------
#define  EFM32_REG_USB_BASE_ADDR           (CPU_INT32U)(0x400C4000u)
#define  EFM32_REG_USB_ROUTE             (*(CPU_REG32 *)(EFM32_REG_USB_BASE_ADDR + 0x18u))

//                                                                 -------- EFM32 USB I/O ROUTE REGISTER BITS ---------
#define  EFM32_USB_ROUTE_BIT_DMPUPEN             DEF_BIT_02
#define  EFM32_USB_ROUTE_BIT_VBUSENPEN           DEF_BIT_01
#define  EFM32_USB_ROUTE_BIT_PHYPEN              DEF_BIT_00

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- USB DEVICE FNCTS -----------------
static void BSP_USBD_EFM32GG_STK3700A_Init(USBD_DRV *p_drv);

static void BSP_USBD_EFM32GG_STK3700A_Conn(void);

static void BSP_USBD_EFM32GG_STK3700A_Disconn(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           USB DEVICE CFGS
 *******************************************************************************************************/

static USBD_DRV *BSP_USBD_EFM32GG_DrvPtr;                       // USB device driver specific info.

//                                                                 ---------- USB DEVICE ENDPOINTS INFO TBL -----------
static USBD_DRV_EP_INFO BSP_USBD_EFM32GG_EP_InfoTbl[] = {
  { USBD_EP_INFO_TYPE_CTRL                                                                            | USBD_EP_INFO_DIR_OUT, 0u, 64u },
  { USBD_EP_INFO_TYPE_CTRL                                                                            | USBD_EP_INFO_DIR_IN, 0u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 1u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 1u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 2u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 2u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 3u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 3u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 4u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 4u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 5u, 64u },
  { USBD_EP_INFO_TYPE_CTRL | USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN, 5u, 64u },
  { DEF_BIT_NONE, 0u, 0u }
};

//                                                                 ---------- USB DEVICE DRIVER INFORMATION -----------
static USBD_DRV_INFO BSP_USBD_EFM32GG_DrvInfoPtr = {
  .BaseAddr = 0x40100000u,
  .MemAddr = 0x00000000u,
  .MemSize = 0u,
  .Spd = USBD_DEV_SPD_FULL,
  .EP_InfoTbl = BSP_USBD_EFM32GG_EP_InfoTbl
};

//                                                                 ------------- USB DEVICE BSP STRUCTURE -------------
static USBD_DRV_BSP_API BSP_USBD_EFM32GG_STK3700A_BSP_API_Ptr = {
  BSP_USBD_EFM32GG_STK3700A_Init,
  BSP_USBD_EFM32GG_STK3700A_Conn,
  BSP_USBD_EFM32GG_STK3700A_Disconn
};

//                                                                 ----------- USB DEVICE HW INFO STRUCTURE -----------
const USBD_DEV_CTRLR_HW_INFO BSP_USBD_EFM32GG_HwInfo = {
  .DrvAPI_Ptr = &USBD_DrvAPI_EFM32_OTG_FS,
  .DrvInfoPtr = &BSP_USBD_EFM32GG_DrvInfoPtr,
  .BSP_API_Ptr = &BSP_USBD_EFM32GG_STK3700A_BSP_API_Ptr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                             FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      BSP_USBD_EFM32GG_STK3700A_Init()
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
static void BSP_USBD_EFM32GG_STK3700A_Init(USBD_DRV *p_drv)
{
  BSP_USBD_EFM32GG_DrvPtr = p_drv;

  if (CMU_ClockSelectGet(cmuClock_HF) != cmuSelect_HFXO) {
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  }

  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);

  //                                                               Enable USB clock
  CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB
                       | CMU_HFCORECLKEN0_USBC;

  CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_HFCLK);

  DEF_BIT_SET(EFM32_REG_USB_ROUTE, (EFM32_USB_ROUTE_BIT_VBUSENPEN
                                    | EFM32_USB_ROUTE_BIT_PHYPEN));

  NVIC_DisableIRQ(USB_IRQn);
}

/********************************************************************************************************
 *                                      BSP_USBD_EFM32GG_STK3700A_Conn()
 *
 * Description : Connect pull-up on DP.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_EFM32GG_STK3700A_Conn(void)
{
  NVIC_EnableIRQ(USB_IRQn);
}

/********************************************************************************************************
 *                                    BSP_USBD_EFM32GG_STK3700A_Disconn()
 *
 * Description : Disconnect pull-up on DP.
 *
 * Argument(s) : none.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_USBD_EFM32GG_STK3700A_Disconn(void)
{
  NVIC_DisableIRQ(USB_IRQn);
}

/********************************************************************************************************
 *                                           USB_IRQHandler()
 *
 * Description : Provide p_drv pointer to the driver ISR Handler.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
void USB_IRQHandler(void)
{
  USBD_DRV *p_drv;

  OSIntEnter();
  p_drv = BSP_USBD_EFM32GG_DrvPtr;
  p_drv->API_Ptr->ISR_Handler(p_drv);                           // Call the USB Device driver ISR.
  OSIntExit();
}

#endif // End of RTOS_MODULE_USB_DEV_AVAIL
