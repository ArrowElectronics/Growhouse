/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   CPU
*
*                               (c) Copyright 2004; Silicon Laboratories Inc.
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
*                                           CPU PORT SELECTOR
*
* File : cpu_port_sel.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _CPU_PORT_SEL_H_
#define  _CPU_PORT_SEL_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

#ifndef CPU_PORT_PATH
    #if ((RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M) || \
         (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V8_M))
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_ARMCC)
            #include  <ports/source/armcc/armv7m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/armv7m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_IAR)
            #include  <ports/source/iar/armv7m_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_ARMCC)
            #include  <ports/source/armcc/armv6m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/armv6m_cpu_port.h>
        #elif (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_IAR)
            #include  <ports/source/iar/armv6m_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_POSIX)
        #if   (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU)
            #include  <ports/source/gnu/posix_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #elif (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_EMUL_WIN32)
        #if   ((RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_WIN32) || \
               (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_GNU))
        #include  <ports/source/win32/win32_cpu_port.h>
        #else
            #warning  Unknown toolchain
        #endif

    #else
        #warning  Unknown port
    #endif

#else
    #include  CPU_PORT_PATH
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of CPU port selector module include.             */
