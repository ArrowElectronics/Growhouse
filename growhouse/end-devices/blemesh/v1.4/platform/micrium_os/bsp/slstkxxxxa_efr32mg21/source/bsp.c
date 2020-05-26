/***************************************************************************//**
 * @file
 * @brief BSP Module - Silicon Labs - Slstkxxxxa_Efr32Mg21
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
#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#include  "../include/bsp_opt_def.h"
//                                                                 Third Party Library Includes

#include  <em_chip.h>
#include  <em_cmu.h>
#include  <em_emu.h>
#include  <bsp.h>
#include  <bspconfig.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  BSP_HF_CLK_SEL
#define  BSP_HF_CLK_SEL                                     BSP_HF_CLK_HFRCO
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

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

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_DEFAULT)
#error "Missing definition of BSP_HF_CLK_SEL in bsp_cfg.h"

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFRCO)
  CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_48M0Hz);
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);     // Select reference clock for High Freq. clock
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

  fclk_freq = SystemCoreClockGet();
  CPU_TS_TmrFreqSet(fclk_freq);
#endif

  BSP_LedsInit();                                               // Initialize the I/Os for the LED controls
}
