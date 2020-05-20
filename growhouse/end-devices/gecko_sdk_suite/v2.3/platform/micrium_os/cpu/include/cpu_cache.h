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
*                                           CPU CACHE MODULE
*
* File : cpu_cache.h
*
* @defgroup CPU_CACHE CPU Cache Management API
* @ingroup CPU
* @brief      CPU Cache Management API
*
* @addtogroup CPU_CACHE
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

#ifndef  _CPU_CACHE_H_
#define  _CPU_CACHE_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   EXTERNS
*********************************************************************************************************
*********************************************************************************************************
*/

#ifdef   CPU_CACHE_MODULE
#define  CPU_CACHE_EXT
#else
#define  CPU_CACHE_EXT  extern
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  "cpu.h"

#include  <common/include/rtos_path.h>
#include  <cpu_cfg.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           CACHE CONFIGURATION
*********************************************************************************************************
*/

#ifndef CPU_CFG_CACHE_MGMT_EN
#define CPU_CFG_CACHE_MGMT_EN  DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                       CACHE OPERATIONS DEFINES
*********************************************************************************************************
*/

#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
#ifndef  CPU_DCACHE_RANGE_FLUSH
#define  CPU_DCACHE_RANGE_FLUSH(addr_start, len)  CPU_DCache_RangeFlush(addr_start, len)
#endif /* CPU_DCACHE_RANGE_FLUSH */
#else
#define  CPU_DCACHE_RANGE_FLUSH(addr_start, len)
#endif /* CPU_CFG_CACHE_MGMT_EN */


#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
#ifndef  CPU_DCACHE_RANGE_INV
#define  CPU_DCACHE_RANGE_INV(addr_start, len)  CPU_DCache_RangeInv(addr_start, len)
#endif /* CPU_DCACHE_RANGE_INV */
#else
#define  CPU_DCACHE_RANGE_INV(addr_start, len)
#endif /* CPU_CFG_CACHE_MGMT_EN */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)

#ifdef __cplusplus
extern  "C" {
#endif

void  CPU_Cache_Init       (void);

void  CPU_DCache_RangeFlush(void      *addr_start,
                            CPU_ADDR   len);

void  CPU_DCache_RangeInv  (void      *addr_start,
                            CPU_ADDR   len);

#ifdef __cplusplus
}
#endif

#endif


/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                           MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of CPU cache module include.                     */
