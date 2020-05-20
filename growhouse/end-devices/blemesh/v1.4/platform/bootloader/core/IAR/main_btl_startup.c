/***************************************************************************//**
 * @file
 * @brief CMSIS Compatible bootloader startup file in C for IAR EWARM
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

#include "em_device.h"        /* The correct device header file. */
#include "api/btl_interface.h"

#pragma language=extended
#pragma segment="CSTACK"

/* IAR start function */
extern void __iar_program_start(void);

/* CMSIS init function */
extern void SystemInit(void);

extern void SystemInit2 (void);

extern const MainBootloaderTable_t mainStageTable;
extern const ApplicationProperties_t appProperties;

/* Auto defined by linker */
extern unsigned char CSTACK$$Limit;

__weak void Reset_Handler(void)
{
  SystemInit();
  SystemInit2();
  __iar_program_start();
}

__weak void NMI_Handler(void)
{
  while (1) ;
}

__weak void HardFault_Handler(void)
{
  while (1) ;
}

__weak void MemManage_Handler(void)
{
  while (1) ;
}

__weak void BusFault_Handler(void)
{
  while (1) ;
}

__weak void UsageFault_Handler(void)
{
  while (1) ;
}

__weak void SVC_Handler(void)
{
  while (1) ;
}

__weak void DebugMon_Handler(void)
{
  while (1) ;
}

__weak void PendSV_Handler(void)
{
  while (1) ;
}

__weak void SysTick_Handler(void)
{
  while (1) ;
}

#if defined(BOOTLOADER_USE_INTERRUPTS)
#if defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
// EFR32xG1
__weak void EMU_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_PRI_IRQHandler(void)
{
  while (1) ;
}

__weak void WDOG0_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_IRQHandler(void)
{
  while (1) ;
}

__weak void MODEM_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_SEQ_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_RSM_IRQHandler(void)
{
  while (1) ;
}

__weak void BUFC_IRQHandler(void)
{
  while (1) ;
}

__weak void LDMA_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_EVEN_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void ACMP0_IRQHandler(void)
{
  while (1) ;
}

__weak void ADC0_IRQHandler(void)
{
  while (1) ;
}

__weak void IDAC0_IRQHandler(void)
{
  while (1) ;
}

__weak void I2C0_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_ODD_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER1_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void LEUART0_IRQHandler(void)
{
  while (1) ;
}

__weak void PCNT0_IRQHandler(void)
{
  while (1) ;
}

__weak void CMU_IRQHandler(void)
{
  while (1) ;
}

__weak void MSC_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYPTO_IRQHandler(void)
{
  while (1) ;
}

__weak void LETIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void AGC_IRQHandler(void)
{
  while (1) ;
}

__weak void PROTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void RTCC_IRQHandler(void)
{
  while (1) ;
}

__weak void SYNTH_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYOTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void RFSENSE_IRQHandler(void)
{
  while (1) ;
}

__weak void FPUEH_IRQHandler(void)
{
  while (1) ;
}

#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_2)
// EFR32xG12
__weak void EMU_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_PRI_IRQHandler(void)
{
  while (1) ;
}

__weak void WDOG0_IRQHandler(void)
{
  while (1) ;
}

__weak void WDOG1_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_IRQHandler(void)
{
  while (1) ;
}

__weak void MODEM_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_SEQ_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_RSM_IRQHandler(void)
{
  while (1) ;
}

__weak void BUFC_IRQHandler(void)
{
  while (1) ;
}

__weak void LDMA_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_EVEN_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void ACMP0_IRQHandler(void)
{
  while (1) ;
}

__weak void ADC0_IRQHandler(void)
{
  while (1) ;
}

__weak void IDAC0_IRQHandler(void)
{
  while (1) ;
}

__weak void I2C0_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_ODD_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER1_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void LEUART0_IRQHandler(void)
{
  while (1) ;
}

__weak void PCNT0_IRQHandler(void)
{
  while (1) ;
}

__weak void CMU_IRQHandler(void)
{
  while (1) ;
}

__weak void MSC_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYPTO0_IRQHandler(void)
{
  while (1) ;
}

__weak void LETIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void AGC_IRQHandler(void)
{
  while (1) ;
}

__weak void PROTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void RTCC_IRQHandler(void)
{
  while (1) ;
}

__weak void SYNTH_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYOTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void RFSENSE_IRQHandler(void)
{
  while (1) ;
}

__weak void FPUEH_IRQHandler(void)
{
  while (1) ;
}

__weak void SMU_IRQHandler(void)
{
  while (1) ;
}

__weak void WTIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void WTIMER1_IRQHandler(void)
{
  while (1) ;
}

__weak void PCNT1_IRQHandler(void)
{
  while (1) ;
}

__weak void PCNT2_IRQHandler(void)
{
  while (1) ;
}

__weak void USART2_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART2_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void I2C1_IRQHandler(void)
{
  while (1) ;
}

__weak void USART3_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART3_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void VDAC0_IRQHandler(void)
{
  while (1) ;
}

__weak void CSEN_IRQHandler(void)
{
  while (1) ;
}

__weak void LESENSE_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYPTO1_IRQHandler(void)
{
  while (1) ;
}

__weak void TRNG0_IRQHandler(void)
{
  while (1) ;
}

#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_3)
// EFR32xG13
__weak void EMU_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_PRI_IRQHandler(void)
{
  while (1) ;
}

__weak void WDOG0_IRQHandler(void)
{
  while (1) ;
}

__weak void WDOG1_IRQHandler(void)
{
  while (1) ;
}

__weak void FRC_IRQHandler(void)
{
  while (1) ;
}

__weak void MODEM_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_SEQ_IRQHandler(void)
{
  while (1) ;
}

__weak void RAC_RSM_IRQHandler(void)
{
  while (1) ;
}

__weak void BUFC_IRQHandler(void)
{
  while (1) ;
}

__weak void LDMA_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_EVEN_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART0_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void ACMP0_IRQHandler(void)
{
  while (1) ;
}

__weak void ADC0_IRQHandler(void)
{
  while (1) ;
}

__weak void IDAC0_IRQHandler(void)
{
  while (1) ;
}

__weak void I2C0_IRQHandler(void)
{
  while (1) ;
}

__weak void GPIO_ODD_IRQHandler(void)
{
  while (1) ;
}

__weak void TIMER1_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART1_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void LEUART0_IRQHandler(void)
{
  while (1) ;
}

__weak void PCNT0_IRQHandler(void)
{
  while (1) ;
}

__weak void CMU_IRQHandler(void)
{
  while (1) ;
}

__weak void MSC_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYPTO0_IRQHandler(void)
{
  while (1) ;
}

__weak void LETIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void AGC_IRQHandler(void)
{
  while (1) ;
}

__weak void PROTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void PRORTC_IRQHandler(void)
{
  while (1) ;
}

__weak void RTCC_IRQHandler(void)
{
  while (1) ;
}

__weak void SYNTH_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYOTIMER_IRQHandler(void)
{
  while (1) ;
}

__weak void RFSENSE_IRQHandler(void)
{
  while (1) ;
}

__weak void FPUEH_IRQHandler(void)
{
  while (1) ;
}

__weak void SMU_IRQHandler(void)
{
  while (1) ;
}

__weak void WTIMER0_IRQHandler(void)
{
  while (1) ;
}

__weak void USART2_RX_IRQHandler(void)
{
  while (1) ;
}

__weak void USART2_TX_IRQHandler(void)
{
  while (1) ;
}

__weak void I2C1_IRQHandler(void)
{
  while (1) ;
}

__weak void VDAC0_IRQHandler(void)
{
  while (1) ;
}

__weak void CSEN_IRQHandler(void)
{
  while (1) ;
}

__weak void LESENSE_IRQHandler(void)
{
  while (1) ;
}

__weak void CRYPTO1_IRQHandler(void)
{
  while (1) ;
}

__weak void TRNG0_IRQHandler(void)
{
  while (1) ;
}

#else
#error "Bootloader needs interrupts, but no ISRs are defined."
#endif
#endif

/* With IAR, the CSTACK is defined via project options settings */
#pragma data_alignment=256
#pragma location = ".intvec"
const void * const __vector_table[] =  {
  &CSTACK$$Limit,
  (void *) Reset_Handler,           /*  1 - Reset (start instruction) */
  (void *) NMI_Handler,             /*  2 - NMI */
  (void *) HardFault_Handler,       /*  3 - HardFault */
  (void *) MemManage_Handler,
  (void *) BusFault_Handler,
  (void *) UsageFault_Handler,
  (void *) 0,
  (void *) 0,
  (void *) 0,
  &mainStageTable,
  (void *) SVC_Handler,
  (void *) DebugMon_Handler,
  &appProperties,
  (void *) PendSV_Handler,
  (void *) SysTick_Handler,
#if defined(BOOTLOADER_USE_INTERRUPTS)
#if defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
  // EFR32xG1
  (void *) EMU_IRQHandler,          /* 0  - EMU         */
  (void *) FRC_PRI_IRQHandler,      /* 1  - FRC_PRI     */
  (void *) WDOG0_IRQHandler,        /* 2  - WDOG0       */
  (void *) FRC_IRQHandler,          /* 3  - FRC         */
  (void *) MODEM_IRQHandler,        /* 4  - MODEM       */
  (void *) RAC_SEQ_IRQHandler,      /* 5  - RAC_SEQ     */
  (void *) RAC_RSM_IRQHandler,      /* 6  - RAC_RSM     */
  (void *) BUFC_IRQHandler,         /* 7  - BUFC        */
  (void *) LDMA_IRQHandler,         /* 8  - LDMA        */
  (void *) GPIO_EVEN_IRQHandler,    /* 9  - GPIO_EVEN   */
  (void *) TIMER0_IRQHandler,       /* 10 - TIMER0      */
  (void *) USART0_RX_IRQHandler,    /* 11 - USART0_RX   */
  (void *) USART0_TX_IRQHandler,    /* 12 - USART0_TX   */
  (void *) ACMP0_IRQHandler,        /* 13 - ACMP0       */
  (void *) ADC0_IRQHandler,         /* 14 - ADC0        */
  (void *) IDAC0_IRQHandler,        /* 15 - IDAC0       */
  (void *) I2C0_IRQHandler,         /* 16 - I2C0        */
  (void *) GPIO_ODD_IRQHandler,     /* 17 - GPIO_ODD    */
  (void *) TIMER1_IRQHandler,       /* 18 - TIMER1      */
  (void *) USART1_RX_IRQHandler,    /* 19 - USART1_RX   */
  (void *) USART1_TX_IRQHandler,    /* 20 - USART1_TX   */
  (void *) LEUART0_IRQHandler,      /* 21 - LEUART0     */
  (void *) PCNT0_IRQHandler,        /* 22 - PCNT0       */
  (void *) CMU_IRQHandler,          /* 23 - CMU         */
  (void *) MSC_IRQHandler,          /* 24 - MSC         */
  (void *) CRYPTO_IRQHandler,       /* 25 - CRYPTO      */
  (void *) LETIMER0_IRQHandler,     /* 26 - LETIMER0    */
  (void *) AGC_IRQHandler,          /* 27 - AGC         */
  (void *) PROTIMER_IRQHandler,     /* 28 - PROTIMER    */
  (void *) RTCC_IRQHandler,         /* 29 - RTCC        */
  (void *) SYNTH_IRQHandler,        /* 30 - SYNTH       */
  (void *) CRYOTIMER_IRQHandler,    /* 31 - CRYOTIMER   */
  (void *) RFSENSE_IRQHandler,      /* 32 - RFSENSE     */
  (void *) FPUEH_IRQHandler,        /* 33 - FPUEH       */
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_2)
  // EFR32xG12
  (void *) EMU_IRQHandler,          /* 0 - EMU          */
  (void *) FRC_PRI_IRQHandler,      /* 1 - FRC_PRI      */
  (void *) WDOG0_IRQHandler,        /* 2 - WDOG0        */
  (void *) WDOG1_IRQHandler,        /* 3 - WDOG1        */
  (void *) FRC_IRQHandler,          /* 4 - FRC          */
  (void *) MODEM_IRQHandler,        /* 5 - MODEM        */
  (void *) RAC_SEQ_IRQHandler,      /* 6 - RAC_SEQ      */
  (void *) RAC_RSM_IRQHandler,      /* 7 - RAC_RSM      */
  (void *) BUFC_IRQHandler,         /* 8 - BUFC         */
  (void *) LDMA_IRQHandler,         /* 9 - LDMA         */
  (void *) GPIO_EVEN_IRQHandler,    /* 10 - GPIO_EVEN   */
  (void *) TIMER0_IRQHandler,       /* 11 - TIMER0      */
  (void *) USART0_RX_IRQHandler,    /* 12 - USART0_RX   */
  (void *) USART0_TX_IRQHandler,    /* 13 - USART0_TX   */
  (void *) ACMP0_IRQHandler,        /* 14 - ACMP0       */
  (void *) ADC0_IRQHandler,         /* 15 - ADC0        */
  (void *) IDAC0_IRQHandler,        /* 16 - IDAC0       */
  (void *) I2C0_IRQHandler,         /* 17 - I2C0        */
  (void *) GPIO_ODD_IRQHandler,     /* 18 - GPIO_ODD    */
  (void *) TIMER1_IRQHandler,       /* 19 - TIMER1      */
  (void *) USART1_RX_IRQHandler,    /* 20 - USART1_RX   */
  (void *) USART1_TX_IRQHandler,    /* 21 - USART1_TX   */
  (void *) LEUART0_IRQHandler,      /* 22 - LEUART0     */
  (void *) PCNT0_IRQHandler,        /* 23 - PCNT0       */
  (void *) CMU_IRQHandler,          /* 24 - CMU         */
  (void *) MSC_IRQHandler,          /* 25 - MSC         */
  (void *) CRYPTO0_IRQHandler,      /* 26 - CRYPTO0     */
  (void *) LETIMER0_IRQHandler,     /* 27 - LETIMER0    */
  (void *) AGC_IRQHandler,          /* 28 - AGC         */
  (void *) PROTIMER_IRQHandler,     /* 29 - PROTIMER    */
  (void *) RTCC_IRQHandler,         /* 30 - RTCC        */
  (void *) SYNTH_IRQHandler,        /* 31 - SYNTH       */
  (void *) CRYOTIMER_IRQHandler,    /* 32 - CRYOTIMER   */
  (void *) RFSENSE_IRQHandler,      /* 33 - RFSENSE     */
  (void *) FPUEH_IRQHandler,        /* 34 - FPUEH       */
  (void *) SMU_IRQHandler,          /* 35 - SMU         */
  (void *) WTIMER0_IRQHandler,      /* 36 - WTIMER0     */
  (void *) WTIMER1_IRQHandler,      /* 37 - WTIMER1     */
  (void *) PCNT1_IRQHandler,        /* 38 - PCNT1       */
  (void *) PCNT2_IRQHandler,        /* 39 - PCNT2       */
  (void *) USART2_RX_IRQHandler,    /* 40 - USART2_RX   */
  (void *) USART2_TX_IRQHandler,    /* 41 - USART2_TX   */
  (void *) I2C1_IRQHandler,         /* 42 - I2C1        */
  (void *) USART3_RX_IRQHandler,    /* 43 - USART3_RX   */
  (void *) USART3_TX_IRQHandler,    /* 44 - USART3_TX   */
  (void *) VDAC0_IRQHandler,        /* 45 - VDAC0       */
  (void *) CSEN_IRQHandler,         /* 46 - CSEN        */
  (void *) LESENSE_IRQHandler,      /* 47 - LESENSE     */
  (void *) CRYPTO1_IRQHandler,      /* 48 - CRYPTO1     */
  (void *) TRNG0_IRQHandler,        /* 49 - TRNG0       */
  (void *) 0,                       /* 50 - Reserved    */
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_3)
  // EFR32xG13
  (void *) EMU_IRQHandler,          /* 0 - EMU          */
  (void *) FRC_PRI_IRQHandler,      /* 1 - FRC_PRI      */
  (void *) WDOG0_IRQHandler,        /* 2 - WDOG0        */
  (void *) WDOG1_IRQHandler,        /* 3 - WDOG1        */
  (void *) FRC_IRQHandler,          /* 4 - FRC          */
  (void *) MODEM_IRQHandler,        /* 5 - MODEM        */
  (void *) RAC_SEQ_IRQHandler,      /* 6 - RAC_SEQ      */
  (void *) RAC_RSM_IRQHandler,      /* 7 - RAC_RSM      */
  (void *) BUFC_IRQHandler,         /* 8 - BUFC         */
  (void *) LDMA_IRQHandler,         /* 9 - LDMA         */
  (void *) GPIO_EVEN_IRQHandler,    /* 10 - GPIO_EVEN   */
  (void *) TIMER0_IRQHandler,       /* 11 - TIMER0      */
  (void *) USART0_RX_IRQHandler,    /* 12 - USART0_RX   */
  (void *) USART0_TX_IRQHandler,    /* 13 - USART0_TX   */
  (void *) ACMP0_IRQHandler,        /* 14 - ACMP0       */
  (void *) ADC0_IRQHandler,         /* 15 - ADC0        */
  (void *) IDAC0_IRQHandler,        /* 16 - IDAC0       */
  (void *) I2C0_IRQHandler,         /* 17 - I2C0        */
  (void *) GPIO_ODD_IRQHandler,     /* 18 - GPIO_ODD    */
  (void *) TIMER1_IRQHandler,       /* 19 - TIMER1      */
  (void *) USART1_RX_IRQHandler,    /* 20 - USART1_RX   */
  (void *) USART1_TX_IRQHandler,    /* 21 - USART1_TX   */
  (void *) LEUART0_IRQHandler,      /* 22 - LEUART0     */
  (void *) PCNT0_IRQHandler,        /* 23 - PCNT0       */
  (void *) CMU_IRQHandler,          /* 24 - CMU         */
  (void *) MSC_IRQHandler,          /* 25 - MSC         */
  (void *) CRYPTO0_IRQHandler,      /* 26 - CRYPTO0     */
  (void *) LETIMER0_IRQHandler,     /* 27 - LETIMER0    */
  (void *) AGC_IRQHandler,          /* 28 - AGC         */
  (void *) PROTIMER_IRQHandler,     /* 29 - PROTIMER    */
  (void *) PRORTC_IRQHandler,       /* 30 - PRORTC      */
  (void *) RTCC_IRQHandler,         /* 31 - RTCC        */
  (void *) SYNTH_IRQHandler,        /* 32 - SYNTH       */
  (void *) CRYOTIMER_IRQHandler,    /* 33 - CRYOTIMER   */
  (void *) RFSENSE_IRQHandler,      /* 34 - RFSENSE     */
  (void *) FPUEH_IRQHandler,        /* 35 - FPUEH       */
  (void *) SMU_IRQHandler,          /* 36 - SMU         */
  (void *) WTIMER0_IRQHandler,      /* 37 - WTIMER0     */
  (void *) USART2_RX_IRQHandler,    /* 38 - USART2_RX   */
  (void *) USART2_TX_IRQHandler,    /* 39 - USART2_TX   */
  (void *) I2C1_IRQHandler,         /* 40 - I2C1        */
  (void *) VDAC0_IRQHandler,        /* 41 - VDAC0       */
  (void *) CSEN_IRQHandler,         /* 42 - CSEN        */
  (void *) LESENSE_IRQHandler,      /* 43 - LESENSE     */
  (void *) CRYPTO1_IRQHandler,      /* 44 - CRYPTO1     */
  (void *) TRNG0_IRQHandler,        /* 45 - TRNG0       */
  (void *) 0,                       /* 46 - Reserved    */
#else
#error "Bootloader needs interrupts, but no ISRs are defined."
#endif
#endif // BOOTLOADER_USE_INTERRUPTS
};
