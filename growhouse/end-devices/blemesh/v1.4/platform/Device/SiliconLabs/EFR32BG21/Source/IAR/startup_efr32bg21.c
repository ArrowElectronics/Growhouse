/**************************************************************************//**
 * @file
 * @brief CMSIS Compatible EFR32BG21 startup file in C for IAR EWARM
 * @version 5.7.2
 ******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/

#include "em_device.h"        /* The correct device header file. */

#pragma language=extended
#pragma segment="CSTACK"

/* IAR start function */
extern void __iar_program_start(void);
/* CMSIS init function */
extern void SystemInit(void);

/* Auto defined by linker */
extern unsigned char CSTACK$$Limit;

__weak void Reset_Handler(void)
{
  SystemInit();
  __iar_program_start();
}

__weak void NMI_Handler(void)
{
  while (1)
    ;
}

__weak void HardFault_Handler(void)
{
  while (1)
    ;
}

__weak void MemManage_Handler(void)
{
  while (1)
    ;
}

__weak void BusFault_Handler(void)
{
  while (1)
    ;
}

__weak void UsageFault_Handler(void)
{
  while (1)
    ;
}

__weak void SVC_Handler(void)
{
  while (1)
    ;
}

__weak void DebugMon_Handler(void)
{
  while (1)
    ;
}

__weak void PendSV_Handler(void)
{
  while (1)
    ;
}

__weak void SysTick_Handler(void)
{
  while (1)
    ;
}
__weak void SETAMPERHOST_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SEMBRX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SEMBTX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SMU_SECURE_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SMU_PRIVILEGED_IRQHandler(void)
{
  while (1)
    ;
}
__weak void EMU_IRQHandler(void)
{
  while (1)
    ;
}
__weak void TIMER0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void TIMER1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void TIMER2_IRQHandler(void)
{
  while (1)
    ;
}
__weak void TIMER3_IRQHandler(void)
{
  while (1)
    ;
}
__weak void RTCC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART0_RX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART0_TX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART1_RX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART1_TX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART2_RX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void USART2_TX_IRQHandler(void)
{
  while (1)
    ;
}
__weak void ICACHE0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void BURTC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void LETIMER0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SYSCFG_IRQHandler(void)
{
  while (1)
    ;
}
__weak void LDMA_IRQHandler(void)
{
  while (1)
    ;
}
__weak void LFXO_IRQHandler(void)
{
  while (1)
    ;
}
__weak void LFRCO_IRQHandler(void)
{
  while (1)
    ;
}
__weak void ULFRCO_IRQHandler(void)
{
  while (1)
    ;
}
__weak void GPIO_ODD_IRQHandler(void)
{
  while (1)
    ;
}
__weak void GPIO_EVEN_IRQHandler(void)
{
  while (1)
    ;
}
__weak void I2C0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void I2C1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void EMUDG_IRQHandler(void)
{
  while (1)
    ;
}
__weak void EMUSE_IRQHandler(void)
{
  while (1)
    ;
}
__weak void AGC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void BUFC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void FRC_PRI_IRQHandler(void)
{
  while (1)
    ;
}
__weak void FRC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void MODEM_IRQHandler(void)
{
  while (1)
    ;
}
__weak void PROTIMER_IRQHandler(void)
{
  while (1)
    ;
}
__weak void RAC_RSM_IRQHandler(void)
{
  while (1)
    ;
}
__weak void RAC_SEQ_IRQHandler(void)
{
  while (1)
    ;
}
__weak void PRORTC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SYNTH_IRQHandler(void)
{
  while (1)
    ;
}
__weak void ACMP0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void ACMP1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void WDOG0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void WDOG1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void HFXO00_IRQHandler(void)
{
  while (1)
    ;
}
__weak void HFRCO0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void HFRCOEM23_IRQHandler(void)
{
  while (1)
    ;
}
__weak void CMU_IRQHandler(void)
{
  while (1)
    ;
}
__weak void AES_IRQHandler(void)
{
  while (1)
    ;
}
__weak void IADC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void MSC_IRQHandler(void)
{
  while (1)
    ;
}
__weak void DPLL0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SW0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SW1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SW2_IRQHandler(void)
{
  while (1)
    ;
}
__weak void SW3_IRQHandler(void)
{
  while (1)
    ;
}
__weak void KERNEL0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void KERNEL1_IRQHandler(void)
{
  while (1)
    ;
}
__weak void M33CTI0_IRQHandler(void)
{
  while (1)
    ;
}
__weak void M33CTI1_IRQHandler(void)
{
  while (1)
    ;
}

/* With IAR, the CSTACK is defined via project options settings */
#pragma data_alignment=256
#pragma location = ".intvec"
const void * const __vector_table[] = {
  &CSTACK$$Limit,
  (void *) Reset_Handler,             /* -15 - Reset (start instruction) */
  (void *) NMI_Handler,               /* -14 - NMI */
  (void *) HardFault_Handler,         /* -13 - HardFault */
  (void *) MemManage_Handler,         /* -12 - MemManager */
  (void *) BusFault_Handler,          /* -11 - BusFault */
  (void *) UsageFault_Handler,        /* -10 - UsageFalt */
  (void *) 0,                         /* -09 - RESERVED */
  (void *) 0,                         /* -08 - RESERVED */
  (void *) 0,                         /* -07 - RESERVED */
  (void *) 0,                         /* -06 RESERVED */
  (void *) SVC_Handler,               /* -05 SVC */
  (void *) DebugMon_Handler,          /* -04 - Debug */
  (void *) 0,                         /* -03 - RESERVED */
  (void *) PendSV_Handler,            /* -02 - Pending SV */
  (void *) SysTick_Handler,           /* -01 - SysTick */
  (void *) SETAMPERHOST_IRQHandler,   /* -16 - SETAMPERHOST */
  (void *) SEMBRX_IRQHandler,         /* -15 - SEMBRX */
  (void *) SEMBTX_IRQHandler,         /* -14 - SEMBTX */
  (void *) SMU_SECURE_IRQHandler,     /* -13 - SMU_SECURE */
  (void *) SMU_PRIVILEGED_IRQHandler, /* -12 - SMU_PRIVILEGED */
  (void *) EMU_IRQHandler,            /* -11 - EMU */
  (void *) TIMER0_IRQHandler,         /* -10 - TIMER0 */
  (void *) TIMER1_IRQHandler,         /* -9 - TIMER1 */
  (void *) TIMER2_IRQHandler,         /* -8 - TIMER2 */
  (void *) TIMER3_IRQHandler,         /* -7 - TIMER3 */
  (void *) RTCC_IRQHandler,           /* -6 - RTCC */
  (void *) USART0_RX_IRQHandler,      /* -5 - USART0_RX */
  (void *) USART0_TX_IRQHandler,      /* -4 - USART0_TX */
  (void *) USART1_RX_IRQHandler,      /* -3 - USART1_RX */
  (void *) USART1_TX_IRQHandler,      /* -2 - USART1_TX */
  (void *) USART2_RX_IRQHandler,      /* -1 - USART2_RX */
  (void *) USART2_TX_IRQHandler,      /* 00 - USART2_TX */
  (void *) ICACHE0_IRQHandler,        /* 01 - ICACHE0 */
  (void *) BURTC_IRQHandler,          /* 02 - BURTC */
  (void *) LETIMER0_IRQHandler,       /* 03 - LETIMER0 */
  (void *) SYSCFG_IRQHandler,         /* 04 - SYSCFG */
  (void *) LDMA_IRQHandler,           /* 05 - LDMA */
  (void *) LFXO_IRQHandler,           /* 06 - LFXO */
  (void *) LFRCO_IRQHandler,          /* 07 - LFRCO */
  (void *) ULFRCO_IRQHandler,         /* 08 - ULFRCO */
  (void *) GPIO_ODD_IRQHandler,       /* 09 - GPIO_ODD */
  (void *) GPIO_EVEN_IRQHandler,      /* 10 - GPIO_EVEN */
  (void *) I2C0_IRQHandler,           /* 11 - I2C0 */
  (void *) I2C1_IRQHandler,           /* 12 - I2C1 */
  (void *) EMUDG_IRQHandler,          /* 13 - EMUDG */
  (void *) EMUSE_IRQHandler,          /* 14 - EMUSE */
  (void *) AGC_IRQHandler,            /* 15 - AGC */
  (void *) BUFC_IRQHandler,           /* 16 - BUFC */
  (void *) FRC_PRI_IRQHandler,        /* 17 - FRC_PRI */
  (void *) FRC_IRQHandler,            /* 18 - FRC */
  (void *) MODEM_IRQHandler,          /* 19 - MODEM */
  (void *) PROTIMER_IRQHandler,       /* 20 - PROTIMER */
  (void *) RAC_RSM_IRQHandler,        /* 21 - RAC_RSM */
  (void *) RAC_SEQ_IRQHandler,        /* 22 - RAC_SEQ */
  (void *) PRORTC_IRQHandler,         /* 23 - PRORTC */
  (void *) SYNTH_IRQHandler,          /* 24 - SYNTH */
  (void *) ACMP0_IRQHandler,          /* 25 - ACMP0 */
  (void *) ACMP1_IRQHandler,          /* 26 - ACMP1 */
  (void *) WDOG0_IRQHandler,          /* 27 - WDOG0 */
  (void *) WDOG1_IRQHandler,          /* 28 - WDOG1 */
  (void *) HFXO00_IRQHandler,         /* 29 - HFXO00 */
  (void *) HFRCO0_IRQHandler,         /* 30 - HFRCO0 */
  (void *) HFRCOEM23_IRQHandler,      /* 31 - HFRCOEM23 */
  (void *) CMU_IRQHandler,            /* 32 - CMU */
  (void *) AES_IRQHandler,            /* 33 - AES */
  (void *) IADC_IRQHandler,           /* 34 - IADC */
  (void *) MSC_IRQHandler,            /* 35 - MSC */
  (void *) DPLL0_IRQHandler,          /* 36 - DPLL0 */
  (void *) SW0_IRQHandler,            /* 37 - SW0 */
  (void *) SW1_IRQHandler,            /* 38 - SW1 */
  (void *) SW2_IRQHandler,            /* 39 - SW2 */
  (void *) SW3_IRQHandler,            /* 40 - SW3 */
  (void *) KERNEL0_IRQHandler,        /* 41 - KERNEL0 */
  (void *) KERNEL1_IRQHandler,        /* 42 - KERNEL1 */
  (void *) M33CTI0_IRQHandler,        /* 43 - M33CTI0 */
  (void *) M33CTI1_IRQHandler,        /* 44 - M33CTI1 */
};
