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
*                                             BSP MODULE
*
*                                            Silicon Labs
*                                        SLSTKXXXXA_EFR32MG21
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

#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_utils.h>

#include  <bsp/include/bsp_micriumos.h>
#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#include  "../include/bsp_opt_def.h"
                                                                /* Third Party Library Includes                         */

#include  <em_chip.h>
#include  <em_cmu.h>
#include  <em_emu.h>
#include  <bsp.h>
#include  <bspconfig.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  BSP_HF_CLK_SEL
#define  BSP_HF_CLK_SEL                                     BSP_HF_CLK_HFRCO
#endif

#ifndef  BSP_LF_CLK_SEL
#define  BSP_LF_CLK_SEL                                     BSP_LF_CLK_LFRCO
#endif


#if defined(CMU_HFXOINIT_STK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_STK_DEFAULT               /* Use STK default HFXO configuration                   */
#elif defined(CMU_HFXOINIT_WSTK_DEFAULT)
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_WSTK_DEFAULT              /* Use Wireless STK default HFXO configuration          */
#else
#define BSP_HFXO_CONFIG  CMU_HFXOINIT_DEFAULT                   /* Use standard emlib HFXO configuration                */
#endif

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_HFXO) || (BSP_HF_CLK_SEL == BSP_HF_CLK_DPLL_48MHZ)
static const CMU_HFXOInit_TypeDef hfxoInit = BSP_HFXO_CONFIG;
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_SystemInit()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) This function MUST be called before any other BSP function is called.
*********************************************************************************************************
*/

void BSP_SystemInit(void)
{
    CHIP_Init();                                                /* Chip revision alignment and errata fixes             */

#if (BSP_HF_CLK_SEL == BSP_HF_CLK_DEFAULT)
#error "Missing definition of BSP_HF_CLK_SEL in bsp_cfg.h"

#elif (BSP_HF_CLK_SEL == BSP_HF_CLK_HFRCO)
    CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_48M0Hz);
    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);   /* Select reference clock for High Freq. clock          */
#endif

#if (BSP_LF_CLK_SEL == BSP_LF_CLK_LFXO)
    CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
#else
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, false);
#endif
}


/*
*********************************************************************************************************
*                                           BSP_PeriphInit()
*
* Description : Initializes peripheral such as I2C.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) This function must be called after KAL has been initialized.
*********************************************************************************************************
*/

void  BSP_PeriphInit (void)
{
#if ((CPU_CFG_TS_EN     == DEF_ENABLED) || \
     (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
    CPU_TS_TMR_FREQ fclk_freq;


    fclk_freq = SystemCoreClockGet();
    CPU_TS_TmrFreqSet (fclk_freq);
#endif

    BSP_LedsInit();                                             /* Initialize the I/Os for the LED controls             */
}
