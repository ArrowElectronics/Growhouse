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
*                                       INTERRUPT CONTROLLER DRIVER
*
*                                               EMULATION
*
* File : emul_cpu_int_priv.h
*********************************************************************************************************
* Note(s) : (1) This driver targets the following:
*                   Core : POSIX and Win32
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _CPU_INT_NONE_PRIV_H_
#define  _CPU_INT_NONE_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_path.h>

#include  <cpu/include/cpu_port_sel.h>

#include  <common/include/lib_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               USAGE GUARD
*********************************************************************************************************
*********************************************************************************************************
*/

#if (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_NONE)

/*
*********************************************************************************************************
*                                           CPU INTERRUPT TYPES
*
* Note(s) : (1) The driver implementation MUST define, based on CPU types, the following types:
*
*                   CPU_INT_ID          Type used to identify an interrupt.
*
*                   CPU_INT_PRIO        Type used to hold the priority of an interrupt.
*
*                   CPU_INT_PRIO_RET    Type used to hold the effective priority. Can be nagative.
*
*                   CPU_INT_SENS        Type used to hold the sensitivity (edge, level or pulse) of an
*                                       interrupt.
*
*                   CPU_INT_SRC         Type used to hold the source of a multiplexed interrupt line.
*********************************************************************************************************
*/

typedef  CPU_INT16U  CPU_INT_ID;
typedef  CPU_INT08U  CPU_INT_PRIO;
typedef  CPU_INT16S  CPU_INT_PRIO_RET;
typedef  CPU_INT08U  CPU_INT_SENS;
typedef  CPU_INT08U  CPU_INT_SRC;


/*
*********************************************************************************************************
*                                       CPU INTERRUPT CONFIGURATION
*
* Note(s) : (1) The driver implementation MUST define the following configuration parameters:
*
*                   CPU_INT_HANDLER_HAS_ARG     Indicates whether a handler can receive an argument.
*
*                   CPU_INT_NBR_OF_INT          Indicates the number of interrupts the controller supports.
*********************************************************************************************************
*/

#define  CPU_INT_HANDLER_HAS_ARG                        DEF_NO

#define  CPU_INT_NBR_OF_INT                             128u


/*
*********************************************************************************************************
*                                                   DEFINES
*
* Note(s) : (1) These defines should only be used by code which explicitly requires this interrupt
*               controller.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*
* Note(s) : (1) These data types should only be used by code which explicitly requires this interrupt
*               controller.
*********************************************************************************************************
*/


#endif                                                          /* RTOS_INT_CONTROLLER_NAME                             */

#endif                                                          /* _CPU_INT_NONE_PRIV_H_                                */
