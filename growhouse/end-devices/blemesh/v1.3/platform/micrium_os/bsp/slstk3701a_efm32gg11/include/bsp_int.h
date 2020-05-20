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
*                                          INTERRUPT MODULE
*
*                                            Silicon Labs
*                                        SLSTK3701A_EFM32GG11
*
* File : bsp_int.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _BSP_INT_H_
#define  _BSP_INT_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include "em_device.h"

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                                   /* See Note #1.                                         */
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          INTERRUPT SOURCES
*********************************************************************************************************
*/

#define  BSP_INT_OFFSET                      16u
                                                                /* Interrupt Request Lines                              */
#define  BSP_INT_ID_EMU                (BSP_INT_OFFSET + EMU_IRQn)
#define  BSP_INT_ID_WDOG0              (BSP_INT_OFFSET + WDOG0_IRQn)
#define  BSP_INT_ID_LDMA               (BSP_INT_OFFSET + LDMA_IRQn)
#define  BSP_INT_ID_GPIO_EVEN          (BSP_INT_OFFSET + GPIO_EVEN_IRQn)
#define  BSP_INT_ID_SMU                (BSP_INT_OFFSET + SMU_IRQn)
#define  BSP_INT_ID_TIMER0             (BSP_INT_OFFSET + TIMER0_IRQn)
#define  BSP_INT_ID_USART0_RX          (BSP_INT_OFFSET + USART0_RX_IRQn)
#define  BSP_INT_ID_USART0_TX          (BSP_INT_OFFSET + USART0_TX_IRQn)
#define  BSP_INT_ID_ACMP0              (BSP_INT_OFFSET + ACMP0_IRQn)
#define  BSP_INT_ID_ADC0               (BSP_INT_OFFSET + ADC0_IRQn)
#define  BSP_INT_ID_IDAC0              (BSP_INT_OFFSET + IDAC0_IRQn)
#define  BSP_INT_ID_I2C0               (BSP_INT_OFFSET + I2C0_IRQn)
#define  BSP_INT_ID_I2C1               (BSP_INT_OFFSET + I2C1_IRQn)
#define  BSP_INT_ID_GPIO_ODD           (BSP_INT_OFFSET + GPIO_ODD_IRQn)
#define  BSP_INT_ID_TIMER1             (BSP_INT_OFFSET + TIMER1_IRQn)
#define  BSP_INT_ID_TIMER2             (BSP_INT_OFFSET + TIMER2_IRQn)
#define  BSP_INT_ID_TIMER3             (BSP_INT_OFFSET + TIMER3_IRQn)
#define  BSP_INT_ID_USART1_RX          (BSP_INT_OFFSET + USART1_RX_IRQn)
#define  BSP_INT_ID_USART1_TX          (BSP_INT_OFFSET + USART1_TX_IRQn)
#define  BSP_INT_ID_USART2_RX          (BSP_INT_OFFSET + USART2_RX_IRQn)
#define  BSP_INT_ID_USART2_TX          (BSP_INT_OFFSET + USART2_TX_IRQn)
#define  BSP_INT_ID_UART0_RX           (BSP_INT_OFFSET + UART0_RX_IRQn)
#define  BSP_INT_ID_UART0_TX           (BSP_INT_OFFSET + UART0_TX_IRQn)
#define  BSP_INT_ID_UART1_RX           (BSP_INT_OFFSET + UART1_RX_IRQn)
#define  BSP_INT_ID_UART1_TX           (BSP_INT_OFFSET + UART1_TX_IRQn)
#define  BSP_INT_ID_LEUART0            (BSP_INT_OFFSET + LEUART0_IRQn)
#define  BSP_INT_ID_LEUART1            (BSP_INT_OFFSET + LEUART1_IRQn)
#define  BSP_INT_ID_LETIMER0           (BSP_INT_OFFSET + LETIMER0_IRQn)
#define  BSP_INT_ID_PCNT0              (BSP_INT_OFFSET + PCNT0_IRQn)
#define  BSP_INT_ID_PCNT1              (BSP_INT_OFFSET + PCNT1_IRQn)
#define  BSP_INT_ID_PCNT2              (BSP_INT_OFFSET + PCNT2_IRQn)
#define  BSP_INT_ID_RTCC               (BSP_INT_OFFSET + RTCC_IRQn)
#define  BSP_INT_ID_CMU                (BSP_INT_OFFSET + CMU_IRQn)
#define  BSP_INT_ID_MSC                (BSP_INT_OFFSET + MSC_IRQn)
#define  BSP_INT_ID_CRYPTO0            (BSP_INT_OFFSET + CRYPTO0_IRQn)
#define  BSP_INT_ID_CRYOTIMER          (BSP_INT_OFFSET + CRYOTIMER_IRQn)
#define  BSP_INT_ID_FPUEH              (BSP_INT_OFFSET + FPUEH_IRQn)
#define  BSP_INT_ID_USART3_RX          (BSP_INT_OFFSET + USART3_RX_IRQn)
#define  BSP_INT_ID_USART3_TX          (BSP_INT_OFFSET + USART3_TX_IRQn)
#define  BSP_INT_ID_USART4_RX          (BSP_INT_OFFSET + USART4_RX_IRQn)
#define  BSP_INT_ID_USART4_TX          (BSP_INT_OFFSET + USART4_TX_IRQn)
#define  BSP_INT_ID_WTIMER0            (BSP_INT_OFFSET + WTIMER0_IRQn)
#define  BSP_INT_ID_WTIMER1            (BSP_INT_OFFSET + WTIMER1_IRQn)
#define  BSP_INT_ID_WTIMER2            (BSP_INT_OFFSET + WTIMER2_IRQn)
#define  BSP_INT_ID_WTIMER3            (BSP_INT_OFFSET + WTIMER3_IRQn)
#define  BSP_INT_ID_I2C2               (BSP_INT_OFFSET + I2C2_IRQn)
#define  BSP_INT_ID_VDAC0              (BSP_INT_OFFSET + VDAC0_IRQn)
#define  BSP_INT_ID_TIMER4             (BSP_INT_OFFSET + TIMER4_IRQn)
#define  BSP_INT_ID_TIMER5             (BSP_INT_OFFSET + TIMER5_IRQn)
#define  BSP_INT_ID_TIMER6             (BSP_INT_OFFSET + TIMER6_IRQn)
#define  BSP_INT_ID_USART5_RX          (BSP_INT_OFFSET + USART5_RX_IRQn)
#define  BSP_INT_ID_USART5_TX          (BSP_INT_OFFSET + USART5_TX_IRQn)
#define  BSP_INT_ID_CSEN               (BSP_INT_OFFSET + CSEN_IRQn)
#define  BSP_INT_ID_LESENSE            (BSP_INT_OFFSET + LESENSE_IRQn)
#define  BSP_INT_ID_EBI                (BSP_INT_OFFSET + EBI_IRQn)
#define  BSP_INT_ID_ACMP2              (BSP_INT_OFFSET + ACMP2_IRQn)
#define  BSP_INT_ID_ADC1               (BSP_INT_OFFSET + ADC1_IRQn)
#define  BSP_INT_ID_LCD                (BSP_INT_OFFSET + LCD_IRQn)
#define  BSP_INT_ID_SDIO               (BSP_INT_OFFSET + SDIO_IRQn)
#define  BSP_INT_ID_ETH                (BSP_INT_OFFSET + ETH_IRQn)
#define  BSP_INT_ID_CAN0               (BSP_INT_OFFSET + CAN0_IRQn)
#define  BSP_INT_ID_CAN1               (BSP_INT_OFFSET + CAN1_IRQn)
#define  BSP_INT_ID_USB                (BSP_INT_OFFSET + USB_IRQn)
#define  BSP_INT_ID_RTC                (BSP_INT_OFFSET + RTC_IRQn)
#define  BSP_INT_ID_WDOG1              (BSP_INT_OFFSET + WDOG1_IRQn)
#define  BSP_INT_ID_LETIMER1           (BSP_INT_OFFSET + LETIMER1_IRQn)
#define  BSP_INT_ID_TRNG0              (BSP_INT_OFFSET + TRNG0_IRQn)
#define  BSP_INT_ID_QSPI0              (BSP_INT_OFFSET + QSPI0_IRQn)

#define  BSP_INT_ID_MAX                      68u                /* # interrupts sources.                                */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                               /* End of 'extern'al C lang linkage.                    */
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
