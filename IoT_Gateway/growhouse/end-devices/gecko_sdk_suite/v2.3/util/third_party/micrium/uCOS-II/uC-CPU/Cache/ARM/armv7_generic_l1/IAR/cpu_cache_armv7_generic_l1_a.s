;********************************************************************************************************
;                                                uC/CPU
;                                    CPU CONFIGURATION & PORT LAYER
;
;                          (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
;
;               All rights reserved.  Protected by international copyright laws.
;
;               uC/CPU is provided in source form to registered licensees ONLY.  It is
;               illegal to distribute this source code to any third party unless you receive
;               written permission by an authorized Micrium representative.  Knowledge of
;               the source code may NOT be used to develop a similar product.
;
;               Please help us continue to provide the Embedded community with the finest
;               software available.  Your honesty is greatly appreciated.
;
;               You can find our product's user manual, API reference, release notes and
;               more information at https://doc.micrium.com.
;               You can contact us at www.micrium.com.
;
; File      : cpu_cache_armv7_generic_l1_s.s
; Version   : V1.31.01
; For       : ARMv7 Generic Cache
; Toolchain : IAR EWARM
;
; Notes : none.
;********************************************************************************************************
;

;********************************************************************************************************
;                                           MACROS AND DEFINIITIONS
;********************************************************************************************************

    IMPORT CPU_Cache_Linesize


    PRESERVE8

    RSEG CODE:CODE:NOROOT(2)
    CODE32


;********************************************************************************************************
;                                           CPU_DCache_LineSizeGet()
;
; Description : Returns the cache line size.
;
; Prototypes  : void  CPU_DCache_LineSizeGet (void)
;
; Argument(s) : none.
;********************************************************************************************************

    EXPORT CPU_DCache_LineSizeGet

CPU_DCache_LineSizeGet

    MRC     p15, 0, r0, c0, c0, 1
    AND     r0, r0, #0xF0000
    LSR     r0, r0, #16
    MOV     r1, #1
    LSL     r1, r1, r0
    LSL     r0, r1, #2


    BX      lr


;********************************************************************************************************
;                                      INVALIDATE DATA CACHE RANGE
;
; Description : Invalidate a range of data cache by MVA.
;
; Prototypes  : void  CPU_DCache_RangeInv  (void       *p_mem,
;                                           CPU_SIZE_T  range);
;
; Argument(s) : p_mem    Start address of the region to invalidate.
;
;               range    Size of the region to invalidate in bytes.
;
; Note(s)     : none.
;********************************************************************************************************

    EXPORT CPU_DCache_RangeInv

CPU_DCache_RangeInv
    DSB
    MOV32 r2, CPU_Cache_Linesize
    LDR r2, [r2]
    SUB r3, r2, #1
    ADD r1, r1, r0
    BIC r0, r0, r3

CPU_DCache_RangeInvL1
    MCR p15,0, r0, c7, c6, 1
    ADD r0, r0, r2
    CMP r0, r1
    BLT CPU_DCache_RangeInvL1
    DSB

    BX LR


;********************************************************************************************************
;                                       FLUSH DATA CACHE RANGE
;
; Description : Flush (clean) a range of data cache by MVA.
;
; Prototypes  : void  CPU_DCache_RangeFlush  (void       *p_mem,
;                                             CPU_SIZE_T  range);
;
; Argument(s) : p_mem    Start address of the region to flush.
;
;               range    Size of the region to invalidate in bytes.
;
; Note(s)     : none.
;********************************************************************************************************

    EXPORT CPU_DCache_RangeFlush

CPU_DCache_RangeFlush
    DSB
    MOV32 r2, CPU_Cache_Linesize
    LDR r2, [r2]
    SUB r3, r2, #1
    ADD r1, r1, r0
    BIC r0, r0, r3

CPU_DCache_RangeFlushL1
    MCR p15, 0, r0, c7, c10, 1
    ADD r0, r0, r2
    CMP r0, r1
    BLT CPU_DCache_RangeFlushL1
    DSB

    BX lr


;********************************************************************************************************
;                                 FLUSH & INVALIDATE DATA CACHE RANGE
;
; Description : Flush and invalidate a range of data cache by MVA.
;
; Prototypes  : void  CPU_DCache_RangeFlushInv  (void       *p_mem,
;                                                CPU_SIZE_T  range);
;
; Argument(s) : p_mem    Start address of the region to flush.
;
;               range    Size of the region to invalidate in bytes.
;
; Note(s)     : none.
;********************************************************************************************************

    EXPORT CPU_DCache_RangeFlushInv

CPU_DCache_RangeFlushInv
    DSB
    MOV32 r2, CPU_Cache_Linesize
    LDR r2, [r2]
    SUB r3, r2, #1
    ADD r1, r1, r0
    BIC r0, r0, r3

CPU_DCache_RangeFlushInvL1
    MCR p15, 0, r0, c7, c14, 1
    ADD r0, r0, r2
    CMP r0, r1
    BLT CPU_DCache_RangeFlushInvL1
    DSB

    BX lr


    END
