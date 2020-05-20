@
@********************************************************************************************************
@                                              Micrium OS
@                                                 CPU
@
@                             (c) Copyright 2004; Silicon Laboratories Inc.
@                                        https://www.micrium.com
@
@********************************************************************************************************
@ Licensing:
@           YOUR USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS OF A MICRIUM SOFTWARE LICENSE.
@   If you are not willing to accept the terms of an appropriate Micrium Software License, you must not
@   download or use this software for any reason.
@   Information about Micrium software licensing is available at https://www.micrium.com/licensing/
@   It is your obligation to select an appropriate license based on your intended use of the Micrium OS.
@   Unless you have executed a Micrium Commercial License, your use of the Micrium OS is limited to
@   evaluation, educational or personal non-commercial uses. The Micrium OS may not be redistributed or
@   disclosed to any third party without the written consent of Silicon Laboratories Inc.
@********************************************************************************************************
@ Documentation:
@   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
@********************************************************************************************************
@ Technical Support:
@   Support is available for commercially licensed users of Micrium's software. For additional
@   information on support, you can contact info@micrium.com.
@********************************************************************************************************

@********************************************************************************************************
@
@                                            CPU PORT FILE
@
@
@ File : armv6m_cpu_a.asm
@********************************************************************************************************
@ @note     (1) This driver targets the following:
@               Core      : ARMv6M Cortex-M
@               Toolchain : GNU C Compiler
@*******************************************************************************************************


@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

        .global  CPU_IntDis
        .global  CPU_IntEn

        .global  CPU_SR_Save
        .global  CPU_SR_Restore

        .global  CPU_WaitForInt
        .global  CPU_WaitForExcept


@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified


@********************************************************************************************************
@                                    DISABLE and ENABLE INTERRUPTS
@
@ @brief    Disable/Enable interrupts.
@
@ Prototypes  : void  CPU_IntDis(void);
@               void  CPU_IntEn (void);
@********************************************************************************************************

.thumb_func
CPU_IntDis:
        CPSID   I
        BX      LR


.thumb_func
CPU_IntEn:
        CPSIE   I
        BX      LR


@********************************************************************************************************
@                                      CRITICAL SECTION FUNCTIONS
@
@ @brief    Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking, the
@               state of the interrupt disable flag is stored in the local variable 'cpu_sr' & interrupts
@               are then disabled ('cpu_sr' is allocated in all functions that need to disable interrupts).
@               The previous interrupt state is restored by copying 'cpu_sr' into the CPU's status register.
@
@ Prototypes  : CPU_SR  CPU_SR_Save   (void);
@               void    CPU_SR_Restore(CPU_SR  cpu_sr);
@
@ @note     (1) These functions are used in general like this :
@
@                       void  Task (void  *p_arg)
@                       {
@                           CPU_SR_ALLOC();                     /* Allocate storage for CPU status register */
@                               :
@                               :
@                           CPU_CRITICAL_ENTER();               /* cpu_sr = CPU_SR_Save()@                  */
@                               :
@                               :
@                           CPU_CRITICAL_EXIT();                /* CPU_SR_Restore(cpu_sr)@                  */
@                               :
@                       }
@********************************************************************************************************

.thumb_func
CPU_SR_Save:
        MRS     R0, PRIMASK                     @ Set prio int mask to mask all (except faults)
        CPSID   I
        BX      LR


.thumb_func
CPU_SR_Restore:                                  @ See Note #2.
        MSR     PRIMASK, R0
        BX      LR


@********************************************************************************************************
@                                         WAIT FOR INTERRUPT
@
@ @brief    Enters sleep state, which will be exited when an interrupt is received.
@
@ Prototypes  : void  CPU_WaitForInt (void)
@********************************************************************************************************

.thumb_func
CPU_WaitForInt:
        WFI                                     @ Wait for interrupt
        BX      LR


@********************************************************************************************************
@                                         WAIT FOR EXCEPTION
@
@ @brief    Enters sleep state, which will be exited when an exception is received.
@
@ Prototypes  : void  CPU_WaitForExcept (void)
@********************************************************************************************************

.thumb_func
CPU_WaitForExcept:
        WFE                                     @ Wait for exception
        BX      LR


@********************************************************************************************************
@                                     CPU ASSEMBLY PORT FILE END
@********************************************************************************************************

.end
