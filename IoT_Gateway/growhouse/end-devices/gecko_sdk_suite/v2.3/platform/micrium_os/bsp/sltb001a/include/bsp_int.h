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
*                                            SILICON LABS
*                                              EFR32MG
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

#define  BSP_INT_ID_EMU                (BSP_INT_OFFSET + EMU_IRQn)
#define  BSP_INT_ID_WDOG0              (BSP_INT_OFFSET + WDOG0_IRQn)
#define  BSP_INT_ID_LDMA               (BSP_INT_OFFSET + LDMA_IRQn)
#define  BSP_INT_ID_GPIO_EVEN          (BSP_INT_OFFSET + GPIO_EVEN_IRQn)
#define  BSP_INT_ID_TIMER0             (BSP_INT_OFFSET + TIMER0_IRQn)
#define  BSP_INT_ID_USART0_RX          (BSP_INT_OFFSET + USART0_RX_IRQn)
#define  BSP_INT_ID_USART0_TX          (BSP_INT_OFFSET + USART0_TX_IRQn)
#define  BSP_INT_ID_ACMP0              (BSP_INT_OFFSET + ACMP0_IRQn)
#define  BSP_INT_ID_ADC0               (BSP_INT_OFFSET + ADC0_IRQn)
#define  BSP_INT_ID_IDAC0              (BSP_INT_OFFSET + IDAC0_IRQn)
#define  BSP_INT_ID_I2C0               (BSP_INT_OFFSET + I2C0_IRQn)
#define  BSP_INT_ID_GPIO_ODD           (BSP_INT_OFFSET + GPIO_ODD_IRQn)
#define  BSP_INT_ID_TIMER1             (BSP_INT_OFFSET + TIMER1_IRQn)
#define  BSP_INT_ID_USART1_RX          (BSP_INT_OFFSET + USART1_RX_IRQn)
#define  BSP_INT_ID_USART1_TX          (BSP_INT_OFFSET + USART1_TX_IRQn)
#define  BSP_INT_ID_LEUART0            (BSP_INT_OFFSET + LEUART0_IRQn)
#define  BSP_INT_ID_PCNT0              (BSP_INT_OFFSET + PCNT0_IRQn)
#define  BSP_INT_ID_CMU                (BSP_INT_OFFSET + CMU_IRQn)
#define  BSP_INT_ID_MSC                (BSP_INT_OFFSET + MSC_IRQn)
#define  BSP_INT_ID_CRYPTO             (BSP_INT_OFFSET + CRYPTO_IRQn)
#define  BSP_INT_ID_LETIMER0           (BSP_INT_OFFSET + LETIMER0_IRQn)
#define  BSP_INT_ID_RTCC               (BSP_INT_OFFSET + RTCC_IRQn)
#define  BSP_INT_ID_CRYOTIMER          (BSP_INT_OFFSET + CRYOTIMER_IRQn)
#define  BSP_INT_ID_FPUEH              (BSP_INT_OFFSET + FPUEH_IRQn)

#define  BSP_INT_ID_MAX                      34u                /* # interrupts sources.                                */


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
