/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
*
*                               (c) Copyright 2017; Silicon Laboratories Inc.
*                                           https://www.micrium.com
*********************************************************************************************************
* Licensing:
*           YOUR USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS OF A MICRIUM SOFTWARE LICENSE.
*   If you are not willing to accept the terms of an appropriate Micrium Software License, you must not
*   download or use this software for any reason.
*   Information about Micrium software licensing is available at https://www.micrium.com/licensing/
*   It is your obligation to select an appropriate license based on your intended use of the Micrium OS.
*   Unless you have executed a Micrium Commercial License, your use of the Micrium OS is limited to
*   evaluation, educational or personal non-commercial uses. The Micrium OS may not be redistributed or
*   disclosed to any third party without the written consent of Silicon Laboratories Inc.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software. For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           RTOS OPTIONS DEFINES
*
* File : rtos_opt_def.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RTOS_OPT_DEF_H_
#define  _RTOS_OPT_DEF_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CPU ARCHITECTURES
*********************************************************************************************************
*/

#define  RTOS_CPU_SEL_NONE                          0u

#define  RTOS_CPU_SEL_SILABS_GECKO_AUTO             1u

                                                                /* Arm                                                  */
#define  RTOS_CPU_SEL_ARM_CORTEX_M0                 11u
#define  RTOS_CPU_SEL_ARM_CORTEX_M0P                12u
#define  RTOS_CPU_SEL_ARM_CORTEX_M1                 13u
#define  RTOS_CPU_SEL_ARM_CORTEX_M3                 14u
#define  RTOS_CPU_SEL_ARM_CORTEX_M4                 15u
#define  RTOS_CPU_SEL_ARM_CORTEX_M7                 16u
#define  RTOS_CPU_SEL_ARM_CORTEX_M33                17u
#define  RTOS_CPU_SEL_ARM_V7_M                      18u
#define  RTOS_CPU_SEL_ARM_V6_M                      19u
#define  RTOS_CPU_SEL_ARM_V8_M                      20u

                                                                /* Emulation                                            */
#define  RTOS_CPU_SEL_EMUL_WIN32                    21u
#define  RTOS_CPU_SEL_EMUL_POSIX                    22u

                                                                /* Empty                                                */
#define  RTOS_CPU_SEL_EMPTY                         30u


/*
*********************************************************************************************************
*                                               TOOLCHAINS
*********************************************************************************************************
*/

#define  RTOS_TOOLCHAIN_NONE                    0u

#define  RTOS_TOOLCHAIN_ARMCC                   1u
#define  RTOS_TOOLCHAIN_GNU                     2u
#define  RTOS_TOOLCHAIN_IAR                     3u
#define  RTOS_TOOLCHAIN_WIN32                   4u

#define  RTOS_TOOLCHAIN_AUTO                    0xFFFFFFFFu     /* Automatic detection of toolchain.                    */


/*
*********************************************************************************************************
*                                           INTERRUPT CONTROLLERS
*********************************************************************************************************
*/

#define  RTOS_INT_CONTROLLER_NONE               0u
#define  RTOS_INT_CONTROLLER_ARMV7_M            1u
#define  RTOS_INT_CONTROLLER_ARMV6_M            2u
#define  RTOS_INT_CONTROLLER_ARMV8_M            3u

#define  RTOS_INT_CONTROLLER_AUTO               0xFFFFFFFFu     /* Automatic detection of interrupt controller.         */


/*
*********************************************************************************************************
*                                               MODULES DEFINES
*********************************************************************************************************
*/

#define  RTOS_CFG_MODULE_NONE               0x0000u
#define  RTOS_CFG_MODULE_ALL                0xFFFFu

#define  RTOS_CFG_MODULE_APP                0x0001u
#define  RTOS_CFG_MODULE_BSP                0x0002u
#define  RTOS_CFG_MODULE_ALL_APP            0x0003u

#define  RTOS_CFG_MODULE_CAN                0x0010u
#define  RTOS_CFG_MODULE_COMMON             0x0020u
#define  RTOS_CFG_MODULE_CPU                0x0040u
#define  RTOS_CFG_MODULE_FS                 0x0080u
#define  RTOS_CFG_MODULE_KERNEL             0x0100u
#define  RTOS_CFG_MODULE_NET                0x0200u
#define  RTOS_CFG_MODULE_NET_APP            0x0400u
#define  RTOS_CFG_MODULE_USBD               0x0800u
#define  RTOS_CFG_MODULE_USBH               0x1000u
#define  RTOS_CFG_MODULE_IO                 0x2000u
#define  RTOS_CFG_MODULE_CANOPEN            0x4000u
#define  RTOS_CFG_MODULE_PROBE              0x8000u
#define  RTOS_CFG_MODULE_ALL_PRODUCTS       0xFFF0u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos opt def module include.                  */
