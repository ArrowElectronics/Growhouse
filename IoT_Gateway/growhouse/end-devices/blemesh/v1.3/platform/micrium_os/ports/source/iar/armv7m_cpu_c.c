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
*                                           ARM Cortex-M Port
*
* File : armv7m_cpu_c.c
*********************************************************************************************************
* Note(s) : (1) This port targets the following:
*                   Core      : ARMv7M and ARMv8M Cortex-M
*                   Mode      : Thumb-2 ISA
*                   Toolchain : IAR EWARM
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define    MICRIUM_SOURCE
#include  "armv7m_cpu_port.h"
#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>


#ifdef __cplusplus
extern  "C" {
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  CPU_INT_SRC_POS_MAX                  ((((CPU_REG_NVIC_NVIC + 1) & 0x1F) * 32) + 16)

#define  CPU_BIT_BAND_SRAM_REG_LO                 0x20000000
#define  CPU_BIT_BAND_SRAM_REG_HI                 0x200FFFFF
#define  CPU_BIT_BAND_SRAM_BASE                   0x22000000


#define  CPU_BIT_BAND_PERIPH_REG_LO               0x40000000
#define  CPU_BIT_BAND_PERIPH_REG_HI               0x400FFFFF
#define  CPU_BIT_BAND_PERIPH_BASE                 0x42000000


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               LOCAL TABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               CPU_BitBandClr()
*
* @brief    Clear bit in bit-band region.
*
* @param    addr        Byte address in memory space.
*
* @param    bit_nbr     Bit number in byte.
*********************************************************************************************************
*/

void  CPU_BitBandClr (CPU_ADDR    addr,
                      CPU_INT08U  bit_nbr)
{
    CPU_ADDR  bit_word_off;
    CPU_ADDR  bit_word_addr;


    if ((addr >= CPU_BIT_BAND_SRAM_REG_LO) &&
        (addr <= CPU_BIT_BAND_SRAM_REG_HI)) {
        bit_word_off  = ((addr - CPU_BIT_BAND_SRAM_REG_LO  ) * 32) + (bit_nbr * 4);
        bit_word_addr = CPU_BIT_BAND_SRAM_BASE   + bit_word_off;

      *(volatile CPU_INT32U *)(bit_word_addr) = 0;

    } else if ((addr >= CPU_BIT_BAND_PERIPH_REG_LO) &&
               (addr <= CPU_BIT_BAND_PERIPH_REG_HI)) {
        bit_word_off  = ((addr - CPU_BIT_BAND_PERIPH_REG_LO) * 32) + (bit_nbr * 4);
        bit_word_addr = CPU_BIT_BAND_PERIPH_BASE + bit_word_off;

      *(volatile CPU_INT32U *)(bit_word_addr) = 0;
    }
}


/**
*********************************************************************************************************
*                                               CPU_BitBandSet()
*
* @brief    Set bit in bit-band region.
*
* @param    addr        Byte address in memory space.
*
* @param    bit_nbr     Bit number in byte.
*********************************************************************************************************
*/

void  CPU_BitBandSet (CPU_ADDR    addr,
                      CPU_INT08U  bit_nbr)
{
    CPU_ADDR  bit_word_off;
    CPU_ADDR  bit_word_addr;


    if ((addr >= CPU_BIT_BAND_SRAM_REG_LO) &&
        (addr <= CPU_BIT_BAND_SRAM_REG_HI)) {
        bit_word_off  = ((addr - CPU_BIT_BAND_SRAM_REG_LO  ) * 32) + (bit_nbr * 4);
        bit_word_addr = CPU_BIT_BAND_SRAM_BASE   + bit_word_off;

      *(volatile CPU_INT32U *)(bit_word_addr) = 1;

    } else if ((addr >= CPU_BIT_BAND_PERIPH_REG_LO) &&
               (addr <= CPU_BIT_BAND_PERIPH_REG_HI)) {
        bit_word_off  = ((addr - CPU_BIT_BAND_PERIPH_REG_LO) * 32) + (bit_nbr * 4);
        bit_word_addr = CPU_BIT_BAND_PERIPH_BASE + bit_word_off;

      *(volatile CPU_INT32U *)(bit_word_addr) = 1;
    }
}


/*
*********************************************************************************************************
*                                           CPU_IntSrcDis()
*
* Description : Disable an interrupt source.
*
* Note(s)     : (1) This function has been moved in the ARMv7-M Interrupt Controller Driver. Please set
*                   'RTOS_INT_CONTROLLER_SEL' to 'RTOS_INT_CONTROLLER_ARMV7_M' in 'rtos_description.h'
*                   to use the proper interrupt controller.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           CPU_IntSrcEn()
*
* Description : Enable an interrupt source.
*
* Note(s)     : (1) This function has been moved in the ARMv7-M Interrupt Controller Driver. Please set
*                   'RTOS_INT_CONTROLLER_SEL' to 'RTOS_INT_CONTROLLER_ARMV7_M' in 'rtos_description.h'
*                   to use the proper interrupt controller.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           CPU_IntSrcPendClr()
*
* Description : Clear a pending interrupt.
*
* Note(s)     : (1) This function has been moved in the ARMv7-M Interrupt Controller Driver. Please set
*                   'RTOS_INT_CONTROLLER_SEL' to 'RTOS_INT_CONTROLLER_ARMV7_M' in 'rtos_description.h'
*                   to use the proper interrupt controller.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           CPU_IntSrcPrioSet()
*
* Description : Set priority of an interrupt source.
*
* Note(s)     : (1) This function has been moved in the ARMv7-M Interrupt Controller Driver. Please set
*                   'RTOS_INT_CONTROLLER_SEL' to 'RTOS_INT_CONTROLLER_ARMV7_M' in 'rtos_description.h'
*                   to use the proper interrupt controller.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           CPU_IntSrcPrioGet()
*
* Description : Get priority of an interrupt source.
*
* Note(s)     : (1) This function has been moved in the ARMv7-M Interrupt Controller Driver. Please set
*                   'RTOS_INT_CONTROLLER_SEL' to 'RTOS_INT_CONTROLLER_ARMV7_M' in 'rtos_description.h'
*                   to use the proper interrupt controller.
*********************************************************************************************************
*/


#ifdef __cplusplus
}
#endif
