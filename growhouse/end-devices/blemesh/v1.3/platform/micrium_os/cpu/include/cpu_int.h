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

/**
*********************************************************************************************************
*
*                                       GENERIC INTERRUPT SUPPORT
*
* File : cpu_int.h
*
* @defgroup CPU_INT CPU Interrupt API
* @ingroup CPU
* @brief      CPU Interrupt API
*
* @addtogroup CPU_INT
* @{
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _CPU_INT_H_
#define  _CPU_INT_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_path.h>


#if  (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_NONE)
#include  <ports/source/generic/emul_cpu_int_priv.h>
#elif ((RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV6_M) || \
       (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV7_M) || \
       (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV8_M))
#include  <ports/source/generic/armv6m_v7m_cpu_int_priv.h>
#else
#error  "Unsupported RTOS_INT_CONTROLLER_NAME specified."
#endif

#include  "cpu_port_sel.h"


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                   GENERIC INTERRUPT SUPPORT CONFIGURATION
*********************************************************************************************************
*/
                                                                /* The emulator controller is a special case.           */
#if (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_NONE)
#define  CPU_INT_CONTROLLER_EN                                  DEF_DISABLED
#else
#define  CPU_INT_CONTROLLER_EN                                  DEF_ENABLED
#endif
                                                                /* Default value for ROMable Vector table.              */
#ifndef RTOS_INT_IN_ROM
#define  RTOS_INT_IN_ROM                                        DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                       INTERRUPT VECTOR ASSIGNMENT
*
* Note(s) : (1) If the user requires the interrupt vectors to be stored in ROM, the
*               CPU_INT_SRC_HANDLER_SET_KA and CPU_INT_SRC_HANDLER_SET_NKA macros resolve to nothing.
*               If the vectors are to be stored in RAM, the macros resolve to the corresponding functions
*               in the interrupt controller driver.
*********************************************************************************************************
*/

#if (RTOS_INT_IN_ROM == DEF_ENABLED)
#if (CPU_INT_HANDLER_HAS_ARG == DEF_YES)
#define  CPU_INT_SRC_HANDLER_SET_KA(_id, _handler, _arg)        ((void)0)
#define  CPU_INT_SRC_HANDLER_SET_NKA(_id, _handler, _arg)       ((void)0)
#else
#define  CPU_INT_SRC_HANDLER_SET_KA(_id, _handler)              ((void)0)
#define  CPU_INT_SRC_HANDLER_SET_NKA(_id, _handler)             ((void)0)
#endif
#else
#define  CPU_INT_SRC_HANDLER_SET_KA                             CPU_IntSrcHandlerSetKA
#define  CPU_INT_SRC_HANDLER_SET_NKA                            CPU_IntSrcHandlerSetNKA
#endif


/*
*********************************************************************************************************
*                                           INTERRUPT SENSITIVITY
*
* Note(s) : (1) The interupt sensitivity is expressed using two masks. One for the type, the other for
*               the polarity. Bit 0 is the polarity, either 0 for LOW and 1 for HIGH. Bits 2..1 are for
*               the type. Either 00 for EDGE, 01 for LEVEL and 10 for PULSE. If, for example, a rising
*               edge sensitivity is needed, one would use the mask:
*                   'CPU_INT_SENSITIVITY_TYPE_EDGE | CPU_INT_SENSITIVITY_POL_HIGH'.
*********************************************************************************************************
*/
                                                                /* Bit  0  is  'Polarity'. 0 is LOW , 1 is HIGH.        */
#define  CPU_INT_SENSITIVITY_POL_HIGH           (CPU_INT_SENS)(1u)
#define  CPU_INT_SENSITIVITY_POL_LOW            (CPU_INT_SENS)(0u)
#define  CPU_INT_SENSITIVITY_POL_MASK           (CPU_INT_SENS)(DEF_BIT_00)
                                                                /* Bits 21 are 'Type'.    00 is EDGE, 01 is LEVEL,      */
                                                                /*                        10 is PULSE                   */
#define  CPU_INT_SENSITIVITY_TYPE_EDGE          (CPU_INT_SENS)(0u)
#define  CPU_INT_SENSITIVITY_TYPE_LEVEL         (CPU_INT_SENS)(2u)
#define  CPU_INT_SENSITIVITY_TYPE_PULSE         (CPU_INT_SENS)(4u)
#define  CPU_INT_SENSITIVITY_TYPE_MASK          (CPU_INT_SENS)(DEF_BIT_01 | DEF_BIT_02)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           INTERRUPT HANDLER
*********************************************************************************************************
*/

struct  cpu_int_handler {
#if (CPU_INT_HANDLER_HAS_ARG == DEF_YES)
    CPU_FNCT_PTR         HandlerPtr;
    void                *HandlerArgPtr;
#else
    CPU_FNCT_VOID        HandlerPtr;
#endif
};

typedef  struct  cpu_int_handler  CPU_INT_HANDLER;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {
#endif

void              CPU_IntInit            (void);

void              CPU_IntSrcAck          (CPU_INT_ID      id);

void              CPU_IntSrcDis          (CPU_INT_ID      id);

void              CPU_IntSrcEn           (CPU_INT_ID      id);

#if (CPU_INT_HANDLER_HAS_ARG == DEF_YES)
void              CPU_IntSrcHandlerSetKA (CPU_INT_ID      id,
                                          CPU_FNCT_PTR    handler,
                                          void           *p_arg);
#else
void              CPU_IntSrcHandlerSetKA (CPU_INT_ID      id,
                                          CPU_FNCT_VOID   handler);
#endif

#if (CPU_INT_HANDLER_HAS_ARG == DEF_YES)
void              CPU_IntSrcHandlerSetNKA(CPU_INT_ID      id,
                                          CPU_FNCT_PTR    handler,
                                          void           *p_arg);
#else
void              CPU_IntSrcHandlerSetNKA(CPU_INT_ID      id,
                                          CPU_FNCT_VOID   handler);
#endif

void              CPU_IntSrcPendClr      (CPU_INT_ID      id);

void              CPU_IntSrcPendSet      (CPU_INT_ID      id);

CPU_INT_PRIO_RET  CPU_IntSrcPrioGet      (CPU_INT_ID      id);

void              CPU_IntSrcPrioSet      (CPU_INT_ID      id,
                                          CPU_INT_PRIO    prio);

CPU_INT_SENS      CPU_IntSrcSensGet      (CPU_INT_ID      id);

void              CPU_IntSrcSensSet      (CPU_INT_ID      id,
                                          CPU_INT_SENS    sens);

CPU_INT_SRC       CPU_IntSrcGet          (CPU_INT_ID      id);

void              CPU_IntSrcSet          (CPU_INT_ID      id,
                                          CPU_INT_SRC     src);

#ifdef __cplusplus
}
#endif


/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                           MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of CPU interrupt module include.                 */
