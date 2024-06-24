/*
*********************************************************************************************************
*                                              Micrium OS
*                                                 CPU
*
*                             (c) Copyright 2004; Silicon Laboratories Inc.
*                                        https://www.micrium.com
*
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
*                                      INTERRUPT CONTROLLER DRIVER
*
*
* File : armv6m_v7m_cpu_int.c
*********************************************************************************************************
* @note     (1) This driver targets the following:
*               Core      : ARMv6M Cortex-M
*                           ARMv7M Cortex-M
*               Toolchain : Any
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu_int.h>

#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_path.h>

#ifdef  RTOS_MODULE_KERNEL_AVAIL
#include  <kernel/include/os.h>
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              USAGE GUARD
*********************************************************************************************************
*********************************************************************************************************
*/

#if ((RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV6_M) || \
     (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV7_M) || \
     (RTOS_INT_CONTROLLER_NAME == RTOS_INT_CONTROLLER_ARMV8_M))


/*
*********************************************************************************************************
*                              INTERRUPT CONTROLLER DRIVER IMPLEMENTATION
*
* @note     (1) This interrupt controller driver implements the necessary functions to use the NVIC in
*               ARMv7-M and ARMv6-M devices.
*
* @note     (2) The CPU_INT_ID type contains the 'exception number' as defined in sections B1.5.(2-3)
*               of the 'ARMv7-M and ARMv6-M Architecture Reference Manual'.
*
* @note     (3) The exception numbers 0-15 are core ARM exceptions. The exception numbers are defined below:
*
*                   0       Initial stack pointer.
*                   1       Reset (initial program counter).
*                   2       Non-Maskable Interrupt (NMI).
*                   3       Hard Fault.
*                   4       Memory Management Fault / Reserved, see note #4-C.
*                   5       Bus Fault / Reserved, see note #4-C.
*                   6       Usage Fault / Reserved, see note #4-C.
*                   7-10    Reserved
*                   11      Supervisor Call (SVCall).
*                   12      Debug Monitor / Reserved, see note #4-C.
*                   13      Reserved
*                   14      PendSV.
*                   15      SysTick.
*                   16+     External interrupt.
*
* @note     (4) Several exceptions cannot contain an interrupt source:
*
*               (a) IDs 0 and 1, they contain the initial SP and PC.
*               (b) IDs 7-10 and 13 are RESERVED.
*               (c) IDs 4-6 and 12 are RESERVERD on ARMv6m architecture.
*
* @note     (5) Several exceptions cannot be disabled:
*
*               (a) Reset.
*               (b) Non-Maskable Interrupt (NMI).
*               (c) Hard Fault.
*               (d) Supervisor Call (SVCall).
*               (e) Debug Monitor.
*               (f) PendSV.
*
* @note     (6) Three exceptions have a fixed priority:
*
*               (a) The Reset                        has a fixed priority of -3.
*               (b) The Non-Maskable Interrupt (NMI) has a fixed priority of -2.
*               (c) The Hard Fault                   has a fixed priority of -1.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  CPU_INT_ARMV6_V7_M_LAST_CORE_ARM_ID                15u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/
                                                                /* Vector Table.                                        */
PP_ALIGN(static  CPU_FNCT_VOID    CPU_IntRawVectTbl[CPU_INT_NBR_OF_INT], CPU_INT_TABLE_ALIGN);
                                                                /* Managed interrupt handler table.                     */
         static  CPU_INT_HANDLER  CPU_IntVectTbl[CPU_INT_NBR_OF_INT];


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* Generic empty handler.                               */
void  CPU_IntEmptyHandler     (void);
                                                                /* Handler dispatcher.                                  */
void  CPU_IntHandlerDispatcher(void);


/*
*********************************************************************************************************
*                               INTERRUPT CONTROLLER SUPPORTED FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                             CPU_IntInit()
*
* @brief    Initialize the NVIC Interrupt Controller.
*********************************************************************************************************
*/

void  CPU_IntInit (void)
{
    CPU_INT_ID      i;
    CPU_FNCT_VOID  *vtor_base;
    CPU_SR_ALLOC();
                                                                /* Initialize vector table.                             */
                                                                /* Copy ARM Core exception handlers.                    */
    vtor_base = (CPU_FNCT_VOID *)CPU_REG_NVIC_VTOR;
    for (i = 2; i <= CPU_INT_ARMV6_V7_M_LAST_CORE_ARM_ID; ++i) {
        CPU_IntRawVectTbl[i] = vtor_base[i];
    }
                                                                /* Set external ISRs to managed interrupt dispatcher.   */
    for (i = CPU_INT_ARMV6_V7_M_LAST_CORE_ARM_ID + 1; i < CPU_INT_NBR_OF_INT; ++i) {
        CPU_IntRawVectTbl[i] = CPU_IntHandlerDispatcher;
    }


                                                                /* Initialize managed vector table.                     */
    for (i = 0; i < CPU_INT_NBR_OF_INT; ++i) {
        CPU_IntVectTbl[i].HandlerPtr = (CPU_FNCT_VOID)DEF_NULL;
    }


                                                                /* Point the core to the new vector table.              */
    CPU_CRITICAL_ENTER();
    CPU_REG_NVIC_VTOR = (CPU_REG32)&CPU_IntRawVectTbl[0];
    CPU_CRITICAL_EXIT();
}


/**
*********************************************************************************************************
*                                            CPU_IntSrcDis()
*
* @brief    Disable an interrupt.
*
* @param    id  The ID of the interrupt.
*
* @note     (1) See 'INTERRUPT CONTROLLER DRIVER IMPLEMENTATION'.
*********************************************************************************************************
*/

void  CPU_IntSrcDis (CPU_INT_ID  id)
{
    CPU_INT08U  group;
    CPU_INT08U  nbr;
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:
        case CPU_INT_NMI:
        case CPU_INT_HFAULT:
        case CPU_INT_SVCALL:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:
#endif
        case CPU_INT_PENDSV:
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:                                       /* Memory Management.                                   */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR &= ~CPU_REG_NVIC_SHCSR_MEMFAULTENA;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_BUSFAULT:                                  /* Bus Fault.                                           */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR &= ~CPU_REG_NVIC_SHCSR_BUSFAULTENA;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_USAGEFAULT:                                /* Usage Fault.                                         */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR &= ~CPU_REG_NVIC_SHCSR_USGFAULTENA;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ST_CTRL &= ~CPU_REG_NVIC_ST_CTRL_ENABLE;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPT ---------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 group = (id - 16) / 32;
                 nbr   = (id - 16) % 32;

                 CPU_CRITICAL_ENTER();
                 CPU_REG_NVIC_CLREN(group) = DEF_BIT(nbr);
                 CPU_CRITICAL_EXIT();
             }
             break;
    }
}


/**
*********************************************************************************************************
*                                            CPU_IntSrcEn()
*
* @brief    Enable an interrupt.
*
* @param    id  The ID of the interrupt.
*
* @note     (1) See 'INTERRUPT CONTROLLER DRIVER IMPLEMENTATION'.
*********************************************************************************************************
*/

void  CPU_IntSrcEn (CPU_INT_ID  id)
{
    CPU_INT08U  group;
    CPU_INT08U  nbr;
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
            case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
            case CPU_INT_RSVD_04:
            case CPU_INT_RSVD_05:
            case CPU_INT_RSVD_06:
#endif
            case CPU_INT_RSVD_07:
            case CPU_INT_RSVD_08:
            case CPU_INT_RSVD_09:
            case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
            case CPU_INT_RSVD_12:
#endif
            case CPU_INT_RSVD_13:
                 break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:
        case CPU_INT_NMI:
        case CPU_INT_HFAULT:
        case CPU_INT_SVCALL:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:
#endif
        case CPU_INT_PENDSV:
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:                                       /* Memory Management.                                   */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_MEMFAULTENA;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_BUSFAULT:                                  /* Bus Fault.                                           */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_BUSFAULTENA;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_USAGEFAULT:                                /* Usage Fault.                                         */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_SHCSR |= CPU_REG_NVIC_SHCSR_USGFAULTENA;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ST_CTRL |= CPU_REG_NVIC_ST_CTRL_ENABLE;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPT ---------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 group = (id - 16) / 32;
                 nbr   = (id - 16) % 32;

                 CPU_CRITICAL_ENTER();
                 CPU_REG_NVIC_SETEN(group) = DEF_BIT(nbr);
                 CPU_CRITICAL_EXIT();
             }
             break;
    }
}


/**
*********************************************************************************************************
*                                       CPU_IntSrcHandlerSetKA()
*
* @brief    Set the kernel-aware handler for a specific interrupt.
*
* @param    id          The ID of the interrupt.
*
* @param    handler     Handler to call to service the interrupt.
*********************************************************************************************************
*/

void  CPU_IntSrcHandlerSetKA (CPU_INT_ID     id,
                              CPU_FNCT_VOID  handler)
{
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
        case CPU_INT_RESET:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ----------------- OTHER EXCEPTIONS ----------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 CPU_CRITICAL_ENTER();
                 CPU_IntRawVectTbl[id] = CPU_IntHandlerDispatcher;
                 CPU_IntVectTbl[id].HandlerPtr = handler;
                 CPU_CRITICAL_EXIT();
             }
             break;
     }
}


/**
*********************************************************************************************************
*                                       CPU_IntSrcHandlerSetNKA()
*
* @brief    Sets the non kernel-aware handler for a specific interrupt. This handler is called directly
*               by the controller and does not pass through the generic handler.
*
* @param    id          The ID of the interrupt.
*
* @param    handler     The handler to call to service the interrupt.
*********************************************************************************************************
*/

void  CPU_IntSrcHandlerSetNKA (CPU_INT_ID     id,
                               CPU_FNCT_VOID  handler)
{
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
        case CPU_INT_RESET:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ----------------- OTHER EXCEPTIONS ----------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 CPU_CRITICAL_ENTER();
                 CPU_IntRawVectTbl[id] = handler;
                 CPU_CRITICAL_EXIT();
             }
             break;
     }
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcPendClr()
*
* @brief    Clears the pending status of a specific interrupt.
*
* @param    id  The ID of the interrupt.
*********************************************************************************************************
*/

void  CPU_IntSrcPendClr (CPU_INT_ID  id)
{
    CPU_INT08U  group;
    CPU_INT08U  nbr;
    CPU_SR_ALLOC();


    switch (id) {
    /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:
        case CPU_INT_NMI:
        case CPU_INT_HFAULT:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:
#endif
        case CPU_INT_SVCALL:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:
        case CPU_INT_BUSFAULT:
        case CPU_INT_USAGEFAULT:
#endif
             break;


        case CPU_INT_PENDSV:                                    /* PendSV.                                              */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ICSR |= CPU_REG_NVIC_ICSR_PENDSVCLR;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ICSR |= CPU_REG_NVIC_ICSR_PENDSTCLR;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPTS --------------- */
        default:
            if (id < CPU_INT_NBR_OF_INT) {
                 group = (id - 16) / 32;
                 nbr   = (id - 16) % 32;

                 CPU_CRITICAL_ENTER();
                 CPU_REG_NVIC_CLRPEND(group) = DEF_BIT(nbr);
                 CPU_CRITICAL_EXIT();
             }
             break;
    }
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcPendSet()
*
* @brief    Triggers a software generated interrupt for a specific interrupt.
*
* @param    id  The ID of the interrupt.
*********************************************************************************************************
*/

void  CPU_IntSrcPendSet (CPU_INT_ID  id)
{
    CPU_INT08U  group;
    CPU_INT08U  nbr;
    CPU_SR_ALLOC();


    switch (id) {
    /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:
        case CPU_INT_HFAULT:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:
#endif
        case CPU_INT_SVCALL:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:
        case CPU_INT_BUSFAULT:
        case CPU_INT_USAGEFAULT:
#endif
             break;

        case CPU_INT_NMI:                                       /* Non-Maskable Interrupt.                              */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ICSR |= CPU_REG_NVIC_ICSR_NMIPENDSET;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_PENDSV:                                    /* PendSV.                                              */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ICSR |= CPU_REG_NVIC_ICSR_PENDSVSET;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             CPU_REG_NVIC_ICSR |= CPU_REG_NVIC_ICSR_PENDSTSET;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPTS --------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 group = (id - 16) / 32;
                 nbr   = (id - 16) % 32;

                 CPU_CRITICAL_ENTER();
                 CPU_REG_NVIC_SETPEND(group) = DEF_BIT(nbr);
                 CPU_CRITICAL_EXIT();
             }
             break;
    }
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcPrioGet()
*
* @brief    Gets the current priority for a specific interrupt.
*
* @param    id  The ID of the interrupt.
*
* @note     (1) See 'INTERRUPT CONTROLLER DRIVER IMPLEMENTATION'.
*********************************************************************************************************
*/

CPU_INT_PRIO_RET  CPU_IntSrcPrioGet (CPU_INT_ID  id)
{
    CPU_INT08U        group;
    CPU_INT08U        nbr;
    CPU_INT_PRIO_RET  prio;
    CPU_INT32U        temp;
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
                                                                /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             prio = DEF_INT_16S_MIN_VAL;
             break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:                                     /* Reset.                                               */
             prio = -3;
             break;

        case CPU_INT_NMI:                                       /* Non-Maskable Interrupt.                              */
             prio = -2;
             break;

        case CPU_INT_HFAULT:                                    /* Hard Fault.                                          */
             prio = -1;
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:                                       /* Memory Management.                                   */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI1;
             prio = (temp >> (0 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;


        case CPU_INT_BUSFAULT:                                  /* Bus Fault.                                           */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI1;
             prio = (temp >> (1 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;


        case CPU_INT_USAGEFAULT:                                /* Usage Fault.                                         */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI1;
             prio = (temp >> (2 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_SVCALL:                                    /* SVCall.                                              */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI2;
             prio = (temp >> (3 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:                                    /* Debug Monitor.                                       */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI3;
             prio = (temp >> (0 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_PENDSV:                                    /* PendSV.                                              */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI3;
             prio = (temp >> (2 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             temp = CPU_REG_NVIC_SHPRI3;
             prio = (temp >> (3 * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPTS --------------- */
        default:
            if (id < CPU_INT_NBR_OF_INT) {
                 group = (id - 16) / 4;
                 nbr   = (id - 16) % 4;

                 CPU_CRITICAL_ENTER();
                 temp  = CPU_REG_NVIC_PRIO(group);
                 CPU_CRITICAL_EXIT();

                 prio  = (temp >> (nbr * DEF_OCTET_NBR_BITS)) & DEF_OCTET_MASK;
             } else {
                 prio  = DEF_INT_16S_MIN_VAL;
             }
             break;
    }

    return (prio);
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcPrioSet()
*
* @brief    Sets the priority of a specific interrupt.
*
* @param    id      The ID of the interrupt.
*
* @param    prio    The priority.
*
* @note     (1) See 'INTERRUPT CONTROLLER DRIVER IMPLEMENTATION'.
*********************************************************************************************************
*/

void  CPU_IntSrcPrioSet (CPU_INT_ID    id,
                         CPU_INT_PRIO  prio)
{
    CPU_INT08U  group;
    CPU_INT08U  nbr;
    CPU_INT32U  temp;
    CPU_SR_ALLOC();


    switch (id) {
                                                                /* ---------------- INVALID OR RESERVED --------------- */
        case CPU_INT_STK_PTR:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_04:
        case CPU_INT_RSVD_05:
        case CPU_INT_RSVD_06:
#endif
        case CPU_INT_RSVD_07:
        case CPU_INT_RSVD_08:
        case CPU_INT_RSVD_09:
        case CPU_INT_RSVD_10:
#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V6_M)
        case CPU_INT_RSVD_12:
#endif
        case CPU_INT_RSVD_13:
             break;
                                                                /* ------------------ CORE EXCEPTIONS ----------------- */
        case CPU_INT_RESET:                                     /* Reset.                                               */
        case CPU_INT_NMI:                                       /* Non-Maskable Interrupt.                              */
        case CPU_INT_HFAULT:                                    /* Hard Fault.                                          */
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_MEM:                                       /* Memory Management.                                   */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI1;
             temp                &= ~(DEF_OCTET_MASK << (0 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio           << (0 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI1  = temp;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_BUSFAULT:                                  /* Bus Fault.                                           */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI1;
             temp                &= ~(DEF_OCTET_MASK << (1 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio           << (1 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI1  = temp;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_USAGEFAULT:                                /* Usage Fault.                                         */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI1;
             temp                &= ~(DEF_OCTET_MASK << (2 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio           << (2 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI1  = temp;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_SVCALL:                                    /* SVCall.                                              */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI2;
             temp                &= ~((CPU_INT32U)DEF_OCTET_MASK << (3 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio                       << (3 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI2  = temp;
             CPU_CRITICAL_EXIT();
             break;

#if (RTOS_CPU_PORT_NAME == RTOS_CPU_SEL_ARM_V7_M)
        case CPU_INT_DBGMON:                                    /* Debug Monitor.                                       */
             CPU_CRITICAL_ENTER();
             temp                = CPU_REG_NVIC_SHPRI3;
             temp                &= ~(DEF_OCTET_MASK << (0 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio           << (0 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI3  = temp;
             CPU_CRITICAL_EXIT();
             break;
#endif

        case CPU_INT_PENDSV:                                    /* PendSV.                                              */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI3;
             temp                &= ~(DEF_OCTET_MASK << (2 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio           << (2 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI3  = temp;
             CPU_CRITICAL_EXIT();
             break;

        case CPU_INT_SYSTICK:                                   /* SysTick.                                             */
             CPU_CRITICAL_ENTER();
             temp                 = CPU_REG_NVIC_SHPRI3;
             temp                &= ~((CPU_INT32U)DEF_OCTET_MASK << (3 * DEF_OCTET_NBR_BITS));
             temp                |=  (prio                       << (3 * DEF_OCTET_NBR_BITS));
             CPU_REG_NVIC_SHPRI3  = temp;
             CPU_CRITICAL_EXIT();
             break;
                                                                /* ---------------- EXTERNAL INTERRUPTS --------------- */
        default:
             if (id < CPU_INT_NBR_OF_INT) {
                 group                    = (id - 16) / 4;
                 nbr                      = (id - 16) % 4;

                 CPU_CRITICAL_ENTER();
                 temp                     = CPU_REG_NVIC_PRIO(group);
                 temp                    &= ~(DEF_OCTET_MASK << (nbr * DEF_OCTET_NBR_BITS));
                 temp                    |=  (prio           << (nbr * DEF_OCTET_NBR_BITS));
                 CPU_REG_NVIC_PRIO(group) = temp;
                 CPU_CRITICAL_EXIT();
             }
             break;
    }
}


/*
*********************************************************************************************************
*                                      CORE ARM EXCEPTION HANDLERS
*
* @note     (1) See (1) in 'CORE ARM EXCEPTION HANDLERS' in 'os_int_armv7-m_priv.h'
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                  CPU_IntARMv6M_v7M_HandlerBusFault()
*
* @brief    ARMv7-M BusFault Exception handler.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerBusFault (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/**
*********************************************************************************************************
*                                  CPU_IntARMv6M_v7M_HandlerDefault()
*
* @brief    ARMv7-M Default handler. Used to trap all other exceptions.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerDefault (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/**
*********************************************************************************************************
*                                 CPU_IntARMv6M_v7M_HandlerHardFault()
*
* @brief    ARMv7-M HardFault Exception handler.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerHardFault (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/**
*********************************************************************************************************
*                                  CPU_IntARMv6M_v7M_HandlerMemFault()
*
* @brief    ARMv7-M MemFault Exception handler.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerMemFault (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/**
*********************************************************************************************************
*                                    CPU_IntARMv6M_v7M_HandlerNMI()
*
* @brief    ARMv7-M Non-Maskable Interrupt (NMI) handler.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerNMI (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/**
*********************************************************************************************************
*                                 CPU_IntARMv6M_v7M_HandlerUsageFault()
*
* @brief    ARMv7-M UsageFault Exception handler.
*
* @note     (1) This is a default exception handler to be used by the initial vector table.
*********************************************************************************************************
*/

void  CPU_IntARMv6M_v7M_HandlerUsageFault (void)
{
    while (DEF_TRUE) {
        ;
    }
}


/*
*********************************************************************************************************
*                              INTERRUPT CONTROLLER UNSUPPORTED FUNCTIONS
*
* @note     (1) If the driver implementation cannot support certain CPU interrupt functions, they must be
*               defined empty.
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                            CPU_IntSrcAck()
*
* @brief    Acknowledge the servicing of an interrupt.
*
* @param    id  The ID of the interrupt.
*
* @note     (1) Interrupts are automatically acknowledged in a ARMv7-M device.
*********************************************************************************************************
*/

void  CPU_IntSrcAck (CPU_INT_ID  id)
{
    PP_UNUSED_PARAM(id);
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcSensGet()
*
* @brief    Gets the current sensitivity of an interrupt.
*
* @param    id  The ID of the interrupt.
*
* @return   The sensitivity of the interrupt.
*
* @note     (1) The interrupt sensitivity is fixed in a ARMv7-M device.
*********************************************************************************************************
*/

CPU_INT_SENS  CPU_IntSrcSensGet (CPU_INT_ID  id)
{
    PP_UNUSED_PARAM(id);

    return (0u);
}


/**
*********************************************************************************************************
*                                          CPU_IntSrcSensSet()
*
* @brief    Sets the sensitivity of an interrupt.
*
* @param    id      The ID of the interrupt.
*
* @param    prio    The sensitivity.
*
* @note     (1) The interrupt sensitivity is fixed in a ARMv7-M device.
*********************************************************************************************************
*/

void  CPU_IntSrcSensSet (CPU_INT_ID    id,
                         CPU_INT_SENS  sens)

{
    PP_UNUSED_PARAM(id);
    PP_UNUSED_PARAM(sens);
}


/**
*********************************************************************************************************
*                                            CPU_IntSrcGet()
*
* @brief    Gets the current source for a multiplexed interrupt.
*
* @param    id  The ID of the interrupt.
*
* @note     (1) Interrupts are not multiplexed in a ARMv7-M device.
*********************************************************************************************************
*/

CPU_INT_SRC  CPU_IntSrcGet (CPU_INT_ID  id)
{
    PP_UNUSED_PARAM(id);

    return (0u);
}


/**
*********************************************************************************************************
*                                            CPU_IntSrcSet()
*
* @brief    Sets the source of a multiplexed interrupt.
*
* @param    id      The ID of the interrupt.
*
* @param    src     The interrupt source.
*
* @note     (1) Interrupts are not multiplexed in a ARMv7-M device.
*********************************************************************************************************
*/

void  CPU_IntSrcSet (CPU_INT_ID   id,
                     CPU_INT_SRC  src)

{
    PP_UNUSED_PARAM(id);
    PP_UNUSED_PARAM(src);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                         CPU_IntEmptyHandler()
*
* @brief    Default empty handler.
*********************************************************************************************************
*/

void  CPU_IntEmptyHandler (void)
{
    CPU_SW_EXCEPTION();
}


/**
*********************************************************************************************************
*                                      CPU_IntHandlerDispatcher()
*
* @brief    Basic ISR dispatcher that gets called for every interrupt.
*
* @note     (1) This dispatcher calls the registered handler for a specific interrupt.
*********************************************************************************************************
*/

void  CPU_IntHandlerDispatcher (void)
{
    CPU_FNCT_VOID  handler;
    CPU_INT_ID     id;
    CPU_SR_ALLOC();

                                                                /* This is a kernel aware ISR.                          */
    CPU_CRITICAL_ENTER();
    OSIntEnter();
    CPU_CRITICAL_EXIT();

                                                                /* Get active interrupt ID.                             */
    id = CPU_REG_NVIC_ICSR & CPU_MSK_NVIC_ICSR_VECT_ACTIVE;

                                                                /* Should always be true.                               */
    if (id < CPU_INT_NBR_OF_INT) {
        handler = CPU_IntVectTbl[id].HandlerPtr;

        if (handler != (CPU_FNCT_VOID)DEF_NULL) {
                                                                /* If registered, call user specified handler.          */
            handler();
        } else {
                                                                /* Otherwise, call default empty handler.               */
            CPU_IntEmptyHandler();
        }
    }

    OSIntExit();
}


#endif                                                          /* RTOS_INT_CONTROLLER_NAME                             */
