/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/

/* Copyright (c) 2011 - 2014 ARM LIMITED
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of ARM nor the names of its contributors may be used
 *   to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include <stdint.h>
#include "api/btl_interface.h"

/*----------------------------------------------------------------------------
 * Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;
extern uint32_t __zero_table_start__;
extern uint32_t __zero_table_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

extern MainBootloaderTable_t mainStageTable;
extern ApplicationProperties_t appProperties;

/*----------------------------------------------------------------------------
 * Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )(void);

/*----------------------------------------------------------------------------
 * External References
 *----------------------------------------------------------------------------*/
#ifndef __START
extern void  _start(void) __attribute__((noreturn));    /* Pre Main (C library entry point) */
#else
extern int  __START(void) __attribute__((noreturn));    /* main entry point */
#endif

#ifndef __NO_SYSTEM_INIT
extern void SystemInit (void);            /* CMSIS System Initialization      */
#endif

extern void SystemInit2 (void);

/*----------------------------------------------------------------------------
 * Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void);                          /* Default empty handler */
void Reset_Handler(void);                            /* Reset Handler */

/*----------------------------------------------------------------------------
 * User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
#ifndef __STACK_SIZE
#define __STACK_SIZE  0x00001000
#endif
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

#ifndef __HEAP_SIZE
#define __HEAP_SIZE   0x00000C00
#endif
#if __HEAP_SIZE > 0
static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif

/*----------------------------------------------------------------------------
 * Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

/* Cortex-M Processor Exceptions */
void NMI_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler  (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));

/* Part Specific Interrupts */
#if defined(BOOTLOADER_USE_INTERRUPTS)
#if defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
// EFR32xG1
void EMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_PRI_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WDOG0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MODEM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_SEQ_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_RSM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void BUFC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LDMA_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_EVEN_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ACMP0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IDAC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_ODD_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LEUART0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PCNT0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MSC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LETIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void AGC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PROTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RTCC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SYNTH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYOTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RFSENSE_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FPUEH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_2)
// EFR32xG12
void EMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_PRI_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WDOG0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WDOG1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MODEM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_SEQ_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_RSM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void BUFC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LDMA_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_EVEN_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ACMP0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IDAC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_ODD_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LEUART0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PCNT0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MSC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LETIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void AGC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PROTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RTCC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SYNTH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYOTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RFSENSE_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FPUEH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PCNT1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PCNT2_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void VDAC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CSEN_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LESENSE_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TRNG0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_3)
// EFR32xG13
void EMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_PRI_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WDOG0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WDOG1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FRC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MODEM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_SEQ_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RAC_RSM_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void BUFC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LDMA_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_EVEN_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART0_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ACMP0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void IDAC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO_ODD_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LEUART0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PCNT0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void MSC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LETIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void AGC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PROTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void PRORTC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RTCC_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SYNTH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYOTIMER_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void RFSENSE_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void FPUEH_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void SMU_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_RX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_TX_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void VDAC0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CSEN_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void LESENSE_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO1_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
void TRNG0_IRQHandler(void) __attribute__ ((weak, alias("Default_Handler")));
#else
#error "Bootloader needs interrupts, but no ISRs are defined."
#endif
#endif

/*----------------------------------------------------------------------------
 * Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
const pFunc __Vectors[] __attribute__ ((section(".vectors"))) = {
  /* Cortex-M Exception Handlers */
  (pFunc) & __StackTop,                      /*      Initial Stack Pointer     */
  Reset_Handler,                            /*      Reset Handler             */
  NMI_Handler,                              /*      NMI Handler               */
  HardFault_Handler,                        /*      Hard Fault Handler        */
  MemManage_Handler,                        /*      MPU Fault Handler         */
  BusFault_Handler,                         /*      Bus Fault Handler         */
  UsageFault_Handler,                       /*      Usage Fault Handler       */
  Default_Handler,                          /*      Reserved                  */
  Default_Handler,                          /*      Reserved                  */
  Default_Handler,                          /*      Reserved                  */
  (pFunc) & mainStageTable,
  SVC_Handler,                              /*      SVCall Handler            */
  DebugMon_Handler,                         /*      Debug Monitor Handler     */
  (pFunc) & appProperties,
  PendSV_Handler,                           /*      PendSV Handler            */
  SysTick_Handler,                          /*      SysTick Handler           */

  /* External interrupts */
#if defined(BOOTLOADER_USE_INTERRUPTS)
#if defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_1)
  // EFR32xG1
  EMU_IRQHandler,                           /*  0  - EMU                      */
  FRC_PRI_IRQHandler,                       /*  1  - FRC_PRI                  */
  WDOG0_IRQHandler,                         /*  2  - WDOG0                    */
  FRC_IRQHandler,                           /*  3  - FRC                      */
  MODEM_IRQHandler,                         /*  4  - MODEM                    */
  RAC_SEQ_IRQHandler,                       /*  5  - RAC_SEQ                  */
  RAC_RSM_IRQHandler,                       /*  6  - RAC_RSM                  */
  BUFC_IRQHandler,                          /*  7  - BUFC                     */
  LDMA_IRQHandler,                          /*  8  - LDMA                     */
  GPIO_EVEN_IRQHandler,                     /*  9  - GPIO_EVEN                */
  TIMER0_IRQHandler,                        /*  10 - TIMER0                   */
  USART0_RX_IRQHandler,                     /*  11 - USART0_RX                */
  USART0_TX_IRQHandler,                     /*  12 - USART0_TX                */
  ACMP0_IRQHandler,                         /*  13 - ACMP0                    */
  ADC0_IRQHandler,                          /*  14 - ADC0                     */
  IDAC0_IRQHandler,                         /*  15 - IDAC0                    */
  I2C0_IRQHandler,                          /*  16 - I2C0                     */
  GPIO_ODD_IRQHandler,                      /*  17 - GPIO_ODD                 */
  TIMER1_IRQHandler,                        /*  18 - TIMER1                   */
  USART1_RX_IRQHandler,                     /*  19 - USART1_RX                */
  USART1_TX_IRQHandler,                     /*  20 - USART1_TX                */
  LEUART0_IRQHandler,                       /*  21 - LEUART0                  */
  PCNT0_IRQHandler,                         /*  22 - PCNT0                    */
  CMU_IRQHandler,                           /*  23 - CMU                      */
  MSC_IRQHandler,                           /*  24 - MSC                      */
  CRYPTO_IRQHandler,                        /*  25 - CRYPTO                   */
  LETIMER0_IRQHandler,                      /*  26 - LETIMER0                 */
  AGC_IRQHandler,                           /*  27 - AGC                      */
  PROTIMER_IRQHandler,                      /*  28 - PROTIMER                 */
  RTCC_IRQHandler,                          /*  29 - RTCC                     */
  SYNTH_IRQHandler,                         /*  30 - SYNTH                    */
  CRYOTIMER_IRQHandler,                     /*  31 - CRYOTIMER                */
  RFSENSE_IRQHandler,                       /*  32 - RFSENSE                  */
  FPUEH_IRQHandler,                         /*  33 - FPUEH                    */
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_2)
  // EFR32xG12
  EMU_IRQHandler,                           /*  0  - EMU                      */
  FRC_PRI_IRQHandler,                       /*  1  - FRC_PRI                  */
  WDOG0_IRQHandler,                         /*  2  - WDOG0                    */
  WDOG1_IRQHandler,                         /*  3  - WDOG1                    */
  FRC_IRQHandler,                           /*  4  - FRC                      */
  MODEM_IRQHandler,                         /*  5  - MODEM                    */
  RAC_SEQ_IRQHandler,                       /*  6  - RAC_SEQ                  */
  RAC_RSM_IRQHandler,                       /*  7  - RAC_RSM                  */
  BUFC_IRQHandler,                          /*  8  - BUFC                     */
  LDMA_IRQHandler,                          /*  9  - LDMA                     */
  GPIO_EVEN_IRQHandler,                     /*  10 - GPIO_EVEN                */
  TIMER0_IRQHandler,                        /*  11 - TIMER0                   */
  USART0_RX_IRQHandler,                     /*  12 - USART0_RX                */
  USART0_TX_IRQHandler,                     /*  13 - USART0_TX                */
  ACMP0_IRQHandler,                         /*  14 - ACMP0                    */
  ADC0_IRQHandler,                          /*  15 - ADC0                     */
  IDAC0_IRQHandler,                         /*  16 - IDAC0                    */
  I2C0_IRQHandler,                          /*  17 - I2C0                     */
  GPIO_ODD_IRQHandler,                      /*  18 - GPIO_ODD                 */
  TIMER1_IRQHandler,                        /*  19 - TIMER1                   */
  USART1_RX_IRQHandler,                     /*  20 - USART1_RX                */
  USART1_TX_IRQHandler,                     /*  21 - USART1_TX                */
  LEUART0_IRQHandler,                       /*  22 - LEUART0                  */
  PCNT0_IRQHandler,                         /*  23 - PCNT0                    */
  CMU_IRQHandler,                           /*  24 - CMU                      */
  MSC_IRQHandler,                           /*  25 - MSC                      */
  CRYPTO0_IRQHandler,                       /*  26 - CRYPTO0                  */
  LETIMER0_IRQHandler,                      /*  27 - LETIMER0                 */
  AGC_IRQHandler,                           /*  28 - AGC                      */
  PROTIMER_IRQHandler,                      /*  29 - PROTIMER                 */
  RTCC_IRQHandler,                          /*  30 - RTCC                     */
  SYNTH_IRQHandler,                         /*  31 - SYNTH                    */
  CRYOTIMER_IRQHandler,                     /*  32 - CRYOTIMER                */
  RFSENSE_IRQHandler,                       /*  33 - RFSENSE                  */
  FPUEH_IRQHandler,                         /*  34 - FPUEH                    */
  SMU_IRQHandler,                           /*  35 - SMU                      */
  WTIMER0_IRQHandler,                       /*  36 - WTIMER0                  */
  WTIMER1_IRQHandler,                       /*  37 - WTIMER1                  */
  PCNT1_IRQHandler,                         /*  38 - PCNT1                    */
  PCNT2_IRQHandler,                         /*  39 - PCNT2                    */
  USART2_RX_IRQHandler,                     /*  40 - USART2_RX                */
  USART2_TX_IRQHandler,                     /*  41 - USART2_TX                */
  I2C1_IRQHandler,                          /*  42 - I2C1                     */
  USART3_RX_IRQHandler,                     /*  43 - USART3_RX                */
  USART3_TX_IRQHandler,                     /*  44 - USART3_TX                */
  VDAC0_IRQHandler,                         /*  45 - VDAC0                    */
  CSEN_IRQHandler,                          /*  46 - CSEN                     */
  LESENSE_IRQHandler,                       /*  47 - LESENSE                  */
  CRYPTO1_IRQHandler,                       /*  48 - CRYPTO1                  */
  TRNG0_IRQHandler,                         /*  49 - TRNG0                    */
  Default_Handler,                          /*  50 - Reserved                 */
#elif defined(_EFR_DEVICE) && defined(_SILICON_LABS_32B_SERIES_1_CONFIG_3)
  // EFR32xG13
  EMU_IRQHandler,                           /*  0  - EMU                      */
  FRC_PRI_IRQHandler,                       /*  1  - FRC_PRI                  */
  WDOG0_IRQHandler,                         /*  2  - WDOG0                    */
  WDOG1_IRQHandler,                         /*  3  - WDOG1                    */
  FRC_IRQHandler,                           /*  4  - FRC                      */
  MODEM_IRQHandler,                         /*  5  - MODEM                    */
  RAC_SEQ_IRQHandler,                       /*  6  - RAC_SEQ                  */
  RAC_RSM_IRQHandler,                       /*  7  - RAC_RSM                  */
  BUFC_IRQHandler,                          /*  8  - BUFC                     */
  LDMA_IRQHandler,                          /*  9  - LDMA                     */
  GPIO_EVEN_IRQHandler,                     /*  10 - GPIO_EVEN                */
  TIMER0_IRQHandler,                        /*  11 - TIMER0                   */
  USART0_RX_IRQHandler,                     /*  12 - USART0_RX                */
  USART0_TX_IRQHandler,                     /*  13 - USART0_TX                */
  ACMP0_IRQHandler,                         /*  14 - ACMP0                    */
  ADC0_IRQHandler,                          /*  15 - ADC0                     */
  IDAC0_IRQHandler,                         /*  16 - IDAC0                    */
  I2C0_IRQHandler,                          /*  17 - I2C0                     */
  GPIO_ODD_IRQHandler,                      /*  18 - GPIO_ODD                 */
  TIMER1_IRQHandler,                        /*  19 - TIMER1                   */
  USART1_RX_IRQHandler,                     /*  20 - USART1_RX                */
  USART1_TX_IRQHandler,                     /*  21 - USART1_TX                */
  LEUART0_IRQHandler,                       /*  22 - LEUART0                  */
  PCNT0_IRQHandler,                         /*  23 - PCNT0                    */
  CMU_IRQHandler,                           /*  24 - CMU                      */
  MSC_IRQHandler,                           /*  25 - MSC                      */
  CRYPTO0_IRQHandler,                       /*  26 - CRYPTO0                  */
  LETIMER0_IRQHandler,                      /*  27 - LETIMER0                 */
  AGC_IRQHandler,                           /*  28 - AGC                      */
  PROTIMER_IRQHandler,                      /*  29 - PROTIMER                 */
  PRORTC_IRQHandler,                        /*  30 - PRORTC                   */
  RTCC_IRQHandler,                          /*  31 - RTCC                     */
  SYNTH_IRQHandler,                         /*  32 - SYNTH                    */
  CRYOTIMER_IRQHandler,                     /*  33 - CRYOTIMER                */
  RFSENSE_IRQHandler,                       /*  34 - RFSENSE                  */
  FPUEH_IRQHandler,                         /*  35 - FPUEH                    */
  SMU_IRQHandler,                           /*  36 - SMU                      */
  WTIMER0_IRQHandler,                       /*  37 - WTIMER0                  */
  USART2_RX_IRQHandler,                     /*  38 - USART2_RX                */
  USART2_TX_IRQHandler,                     /*  39 - USART2_TX                */
  I2C1_IRQHandler,                          /*  40 - I2C1                     */
  VDAC0_IRQHandler,                         /*  41 - VDAC0                    */
  CSEN_IRQHandler,                          /*  42 - CSEN                     */
  LESENSE_IRQHandler,                       /*  43 - LESENSE                  */
  CRYPTO1_IRQHandler,                       /*  44 - CRYPTO1                  */
  TRNG0_IRQHandler,                         /*  45 - TRNG0                    */
  Default_Handler,                          /*  46 - Reserved                 */
#else
#error "Bootloader needs interrupts, but no ISRs are defined."
#endif
#endif
};

/*----------------------------------------------------------------------------
 * Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
  uint32_t *pSrc, *pDest;
  uint32_t *pTable __attribute__((unused));

#ifndef __NO_SYSTEM_INIT
  SystemInit();
#endif
  SystemInit2();

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE

/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pTable = &__copy_table_start__;

  for (; pTable < &__copy_table_end__; pTable = pTable + 3) {
    pSrc  = (uint32_t*)*(pTable + 0);
    pDest = (uint32_t*)*(pTable + 1);
    for (; pDest < (uint32_t*)(*(pTable + 1) + *(pTable + 2)); ) {
      *pDest++ = *pSrc++;
    }
  }
#else

/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pSrc  = &__etext;
  pDest = &__data_start__;

  for (; pDest < &__data_end__; ) {
    *pDest++ = *pSrc++;
  }
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE

/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
  pTable = &__zero_table_start__;

  for (; pTable < &__zero_table_end__; pTable = pTable + 2) {
    pDest = (uint32_t*)*(pTable + 0);
    for (; pDest < (uint32_t*)(*(pTable + 0) + *(pTable + 1)); ) {
      *pDest++ = 0;
    }
  }
#elif defined (__STARTUP_CLEAR_BSS)

/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
  pDest = &__bss_start__;

  for (; pDest < &__bss_end__; ) {
    *pDest++ = 0ul;
  }
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __START
#define __START _start
#endif
  __START();
}

/*----------------------------------------------------------------------------
 * Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while (1) ;
}
