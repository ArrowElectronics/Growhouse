/***************************************************************************//**
 * @file
 * @brief BSP Module - Silicon Labs - Slstk3701A_Efm32Gg11
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
#include  "../include/bsp_led.h"

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
#include  <fs/include/fs_nor_quad_spi.h>
#endif

#if defined(RTOS_MODULE_IO_SD_AVAIL)
#include  <io/include/sd.h>
#include  <io/include/sd_card.h>
#endif

#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
#include  <net/include/net_if_ether.h>
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL) || defined(RTOS_MODULE_USB_DEV_AVAIL)
#include  <usb/include/usb_ctrlr.h>
#endif

#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
#include  <can/include/can_bus.h>
#endif

#include  "../include/bsp_opt_def.h"
//                                                                 Third Party Library Includes

#include  <em_chip.h>
#include  <em_cmu.h>
#include  <em_emu.h>
#include  <bspconfig.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  BSP_HF_CLK_SEL
#define  BSP_HF_CLK_SEL                                     BSP_HF_CLK_DPLL_48MHZ
#endif

#ifndef  BSP_LF_CLK_SEL
#define  BSP_LF_CLK_SEL                                     BSP_LF_CLK_LFRCO
#endif

#if defined(CMU_HFXOINIT_STK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_STK_DEFAULT               // Use STK default HFXO configuration
#elif defined(CMU_HFXOINIT_WSTK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_WSTK_DEFAULT              // Use Wireless STK default HFXO configuration
#else
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_DEFAULT                   // Use standard emlib HFXO configuration
#endif

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_HFXO) || (BSP_HF_CLK_SEL == BSP_HF_CLK_DPLL_48MHZ)
static const CMU_HFXOInit_TypeDef hfxoInit = BSP_HFXO_CONFIG;
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
//                                                                 Configurations for EFM32xG1x and EFR32xG1x devices
#if defined(EMU_DCDCINIT_STK_DEFAULT)                           // Use STK DCDC Configuration
static const EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
#elif defined(EMU_DCDCINIT_WSTK_DEFAULT)                        // Use Wireless STK DCDC Configuration
static const EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_WSTK_DEFAULT;
#else                                                           // No DCDC Configuration found so we assume that the
#define BSP_DCDC_POWEROFF                                       // board does not use the DCDC
#endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(RTOS_MODULE_IO_SD_AVAIL)
BSP_HW_INFO_EXT(const SD_CARD_CTRLR_DRV_INFO, BSP_SD_SDHC_BSP_DrvInfo);
#endif

#if defined(RTOS_MODULE_USB_DEV_AVAIL)
BSP_HW_INFO_EXT(const USBD_DEV_CTRLR_HW_INFO, BSP_USBD_EFM32_HwInfo);
#endif

#if defined(RTOS_MODULE_USB_HOST_AVAIL)
BSP_HW_INFO_EXT(const USBH_PBHCI_HC_HW_INFO, BSP_USBH_EFM32_PBHCI_HW_Info);
#endif

#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
BSP_HW_INFO_EXT(const NET_IF_ETHER_HW_INFO, BSP_NetEther_HwInfo);
#endif

#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
BSP_HW_INFO_EXT(const FS_NOR_QUAD_SPI_CTRLR_INFO, BSP_FS_NOR_MX25R_QuadSPI_HwInfo);
#endif

#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
BSP_HW_INFO_EXT(const CAN_CTRLR_DRV_INFO, BSP_CAN0_BSP_DrvInfo);
BSP_HW_INFO_EXT(const CAN_CTRLR_DRV_INFO, BSP_CAN1_BSP_DrvInfo);
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

#if defined(_SILICON_LABS_32B_SERIES_1)
#if defined(BSP_DCDC_POWEROFF)
  EMU_DCDCPowerOff();                                           // DCDC is not used to we power it off
#else
  EMU_DCDCInit(&dcdcInit);                                      // Initialize DCDC regulator
#endif
#endif

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_DEFAULT)
#error "Missing definition of BSP_HF_CLK_SEL in bsp_cfg.h"

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFRCO)
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);             // Select reference clock for High Freq. clock

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFXO)
  CMU_HFXOInit(&hfxoInit);                                      // Initialize HFXO with board specific parameters
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);              // Select reference clock for High Freq. clock

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_DPLL_48MHZ)
#define DPLL_N 479
#define DPLL_M 499
  CMU_HFXOInit(&hfxoInit);                                      // Initialize HFXO with board specific parameters
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
  //                                                               Use HFRCO with DPLL to generate 48 MHz
  //                                                               The CMU_DPLLLock() could probably be used.
  CMU_HFRCOBandSet(cmuHFRCOFreq_48M0Hz);
  CMU->HFRCOCTRL |= CMU_HFRCOCTRL_FINETUNINGEN;
  //                                                               Setup DPLL frequency. The formula is Fdco = Fref*(N+1)/(M+1)
  CMU->DPLLCTRL1 = (DPLL_N << _CMU_DPLLCTRL1_N_SHIFT) | (DPLL_M << _CMU_DPLLCTRL1_M_SHIFT);
  //                                                               Enable DPLL
  CMU->OSCENCMD = CMU_OSCENCMD_DPLLEN;
  //                                                               Set HFClock from HFRCO
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

#else
#error "The definition of BSP_HF_CLK_SEL in bsp_cfg.h is not handled."

#endif

#if (BSP_LF_CLK_SEL == BSP_LF_CLK_LFXO)
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
#else
  CMU_OscillatorEnable(cmuOsc_LFRCO, true, false);
#endif
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
#if ((CPU_CFG_TS_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
  CPU_TS_TMR_FREQ fclk_freq;
#endif
  BSP_LED_Init();                                               // Initialize the I/Os for the LED controls

  //                                                               ------------ REGISTER IO-SD CONTROLLERS ------------
#if defined(RTOS_MODULE_IO_SD_AVAIL)
  IO_SD_CARD_CTRLR_REG("sd0", &BSP_SD_SDHC_BSP_DrvInfo);
#endif

  //                                                               ------------- REGISTER USB CONTROLLERS -------------
#if defined(RTOS_MODULE_USB_DEV_AVAIL) || defined(RTOS_MODULE_USB_HOST_AVAIL)
  USB_CTRLR_HW_INFO_REG("usb0", &BSP_USBD_EFM32_HwInfo, &BSP_USBH_EFM32_PBHCI_HW_Info);
#endif

  //                                                               ---------- REGISTER ETHERNET CONTROLLERS -----------
#if defined(RTOS_MODULE_NET_IF_ETHER_AVAIL)
  NET_CTRLR_ETHER_REG("eth0", &BSP_NetEther_HwInfo);
#endif

  //                                                               ----------- REGISTER MEMORY CONTROLLERS ------------
#if defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL)
  FS_NOR_QUAD_SPI_HW_INFO_REG("nor0", &BSP_FS_NOR_MX25R_QuadSPI_HwInfo);
#endif

#if ((CPU_CFG_TS_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
  fclk_freq = SystemCoreClockGet();
  CPU_TS_TmrFreqSet(fclk_freq);
#endif

#if defined(RTOS_MODULE_CAN_BUS_AVAIL)
  CAN_CTRLR_REG("can0", &BSP_CAN0_BSP_DrvInfo);
  CAN_CTRLR_REG("can1", &BSP_CAN1_BSP_DrvInfo);
#endif
}
