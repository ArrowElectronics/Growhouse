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
*                                              BSP MODULE
*
* File : bsp_micriumos.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _BSP_MICRIUMOS_H_
#define  _BSP_MICRIUMOS_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/toolchains.h>
#include  <common/include/rtos_path.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*
* Note(s) : (1) A bug exists in some version of IAR where the compiler can make some optimizations using
*               a weak definition even though a strong definition exists somewhere else. Using the
*               volatile type qualifier in the weak definition prevents this optimization.
*********************************************************************************************************
*********************************************************************************************************
*/

#if (RTOS_TOOLCHAIN != RTOS_TOOLCHAIN_IAR)
#define  BSP_HW_INFO_EXT(type, name)  PP_WEAK(type, name, {0})
#else
                                                                /* See note (1).                                        */
#define  BSP_HW_INFO_EXT(type, name)  PP_WEAK(volatile type, name, {0})
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif

void  BSP_SystemInit(void);

void  BSP_TickInit  (void);

void  BSP_PeriphInit(void);

#ifdef __cplusplus
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
