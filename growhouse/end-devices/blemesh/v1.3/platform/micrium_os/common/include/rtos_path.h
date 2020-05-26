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
*                                               RTOS PATH
*
* File : rtos_path.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RTOS_PATH_H_
#define  _RTOS_PATH_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_description.h>
#if (RTOS_CPU_SEL == RTOS_CPU_SEL_SILABS_GECKO_AUTO)
#include "em_device.h"
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           CONFIGURATION ERRORS
*********************************************************************************************************
*********************************************************************************************************
*/

#if ((!defined(RTOS_CPU_SEL)))
#error  "RTOS_CPU_SEL not defined, please #define RTOS_CPU_SEL to a known MCU. Supported MCUs can be found in common/include/rtos_opt_def.h."
#endif

#if (RTOS_CPU_SEL == RTOS_CPU_SEL_NONE)
#error  "RTOS_CPU_SEL must be #define'd to a known MCU. Supported MCUs can be found in common/include/rtos_opt_def.h."
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/
                                                                /* Auto selection of toolchain.                         */
#if ((!defined(RTOS_TOOLCHAIN_SEL)) || \
     (RTOS_TOOLCHAIN_SEL == RTOS_TOOLCHAIN_AUTO))

    #if (defined(__IAR_SYSTEMS_ICC__))
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_IAR

    #elif defined(_MSC_VER)
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_WIN32

    #elif (defined(__ARMCC_VERSION))
        #define RTOS_TOOLCHAIN      RTOS_TOOLCHAIN_ARMCC

    #elif (defined(__GNUC__) || defined(__GNUG__))
        #if (defined(__arm__))                                  /* GNU for ARM arch.                                    */
            #define RTOS_TOOLCHAIN  RTOS_TOOLCHAIN_GNU
        #else                                                   /* Generic GNU toolchain.                               */
            #define RTOS_TOOLCHAIN  RTOS_TOOLCHAIN_GNU
        #endif
    #else
        #error Unable to detect toolchain. Set RTOS_CFG_TOOLCHAIN.
    #endif

#else
    #define RTOS_TOOLCHAIN          RTOS_TOOLCHAIN_SEL
#endif

#define CORTEX_M0        0x00
#define CORTEX_M0P       0x00
#define CORTEX_M3        0x03
#define CORTEX_M4        0x04
#define CORTEX_M7        0x07
#define CORTEX_M33        33u


/*
*********************************************************************************************************
*                                   CPU, INTERRUPT & TOOLCHAIN NAMES
*********************************************************************************************************
*/


                                                                /* CPU Selector.                                        */
#if (RTOS_CPU_SEL == RTOS_CPU_SEL_SILABS_GECKO_AUTO)

    #if ((__CORTEX_M == CORTEX_M0) || (__CORTEX_M == CORTEX_M0P))
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V6_M
    #elif ((__CORTEX_M == CORTEX_M3) || \
           (__CORTEX_M == CORTEX_M4) || \
           (__CORTEX_M == CORTEX_M7))
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V7_M
    #elif (__CORTEX_M == CORTEX_M33)
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_ARM_V8_M
    #else
        #define  RTOS_CPU_PORT_NAME         RTOS_CPU_SEL_NONE
    #endif

#ifndef  RTOS_INT_CONTROLLER_SEL
#define  RTOS_INT_CONTROLLER_SEL        RTOS_INT_CONTROLLER_AUTO
#endif

#elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M3)   || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M4)   || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M7)   || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V7_M))
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_ARM_V7_M

#elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0)   || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0P)  || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M1)   || \
       (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V6_M))
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_ARM_V6_M

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMUL_POSIX)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMUL_POSIX

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMUL_WIN32)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMUL_WIN32

#elif (RTOS_CPU_SEL == RTOS_CPU_SEL_EMPTY)
#define  RTOS_CPU_PORT_NAME                 RTOS_CPU_SEL_EMPTY
#else
#error  Unsupported RTOS_CPU_SEL specified.
#endif


                                                                /* Toolchain Selector.                                  */
#if ((RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_ARMCC)  || \
     (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_GNU)    || \
     (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_IAR)    || \
     (RTOS_TOOLCHAIN == RTOS_TOOLCHAIN_WIN32))
#define  RTOS_TOOLCHAIN_NAME            RTOS_TOOLCHAIN
#else
#error  Unsupported RTOS_TOOLCHAIN specified.
#endif


                                                                /* CPU Interrupt controller Selector.                   */
#if (RTOS_INT_CONTROLLER_SEL == RTOS_INT_CONTROLLER_AUTO)

    #if (RTOS_CPU_SEL == RTOS_CPU_SEL_SILABS_GECKO_AUTO)

        #if ((__CORTEX_M == CORTEX_M0) || (__CORTEX_M == CORTEX_M0P))
            #define  RTOS_INT_CONTROLLER_NAME       RTOS_INT_CONTROLLER_ARMV6_M
        #elif ((__CORTEX_M == CORTEX_M3) || \
               (__CORTEX_M == CORTEX_M4) || \
               (__CORTEX_M == CORTEX_M7))
            #define  RTOS_INT_CONTROLLER_NAME       RTOS_INT_CONTROLLER_ARMV7_M
        #elif (__CORTEX_M == CORTEX_M33)
            #define  RTOS_INT_CONTROLLER_NAME       RTOS_INT_CONTROLLER_ARMV8_M
        #endif

    #elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M3)   || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M4)   || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M7)   || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V7_M))
        #define  RTOS_INT_CONTROLLER_NAME           RTOS_INT_CONTROLLER_ARMV7_M

    #elif ((RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0)   || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M0P)  || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_CORTEX_M1)   || \
           (RTOS_CPU_SEL == RTOS_CPU_SEL_ARM_V6_M))
        #define  RTOS_INT_CONTROLLER_NAME           RTOS_INT_CONTROLLER_ARMV6_M

     #elif (__CORTEX_M == CORTEX_M33)
        #define  RTOS_INT_CONTROLLER_NAME           RTOS_INT_CONTROLLER_ARMV8_M

    #else
        #define  RTOS_INT_CONTROLLER_NAME            RTOS_INT_CONTROLLER_NONE
    #endif

#elif (RTOS_INT_CONTROLLER_SEL == RTOS_INT_CONTROLLER_ARMV7_M)
#define  RTOS_INT_CONTROLLER_NAME                   RTOS_INT_CONTROLLER_ARMV7_M

#elif (RTOS_INT_CONTROLLER_SEL == RTOS_INT_CONTROLLER_ARMV6_M)
#define  RTOS_INT_CONTROLLER_NAME                   RTOS_INT_CONTROLLER_ARMV6_M

#elif (RTOS_INT_CONTROLLER_SEL == RTOS_INT_CONTROLLER_ARMV8_M)
#define  RTOS_INT_CONTROLLER_NAME                   RTOS_INT_CONTROLLER_ARMV8_M

#else
#define  RTOS_INT_CONTROLLER_NAME                   RTOS_INT_CONTROLLER_NONE
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos path module include.                     */
