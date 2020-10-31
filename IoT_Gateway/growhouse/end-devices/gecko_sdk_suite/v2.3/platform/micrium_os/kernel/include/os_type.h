/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Kernel
*
*                               (c) Copyright 2009; Silicon Laboratories Inc.
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
*                                               KERNEL TYPES
*
* File : os_type.h
*
* @defgroup KERNEL_TYPES Kernel Types API
* @ingroup  KERNEL
* @brief   Kernel Types API
* @addtogroup KERNEL_TYPES
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

#ifndef  _OS_TYPE_H_
#define  _OS_TYPE_H_

#ifdef    VSC_INCLUDE_H_FILE_NAMES
const     CPU_CHAR  *os_type__h = "$Id: $";
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /*       Description                             # Bits */
                                                                /*                                        <recommended> */
                                                                /* ---------------------------------------------------- */

typedef   CPU_INT16U      OS_CPU_USAGE;                         /**< CPU Usage 0..10000                           <16>/32 */

typedef   CPU_INT32U      OS_CTR;                               /**< Counter                                           32 */

typedef   CPU_INT32U      OS_CTX_SW_CTR;                        /**< Counter of context switches                       32 */

typedef   CPU_INT32U      OS_CYCLES;                            /**< CPU clock cycles                             <32>/64 */

typedef   CPU_INT32U      OS_FLAGS;                             /**< Event flags                                8/16/<32> */

typedef   CPU_INT32U      OS_IDLE_CTR;                          /**< Holds number of times the idle task runs     <32>/64 */

typedef   CPU_INT16U      OS_MSG_QTY;                           /**< Number of OS_MSGs in the msg pool            <16>/32 */
typedef   CPU_INT16U      OS_MSG_SIZE;                          /**< Size of messages in number of bytes          <16>/32 */

typedef   CPU_INT08U      OS_NESTING_CTR;                       /**< Interrupt and scheduler nesting            <8>/16/32 */

typedef   CPU_INT16U      OS_OBJ_QTY;                           /**< Number of kernel objects counter             <16>/32 */
typedef   CPU_INT32U      OS_OBJ_TYPE;                          /**< Special flag to determine object type             32 */

typedef   CPU_INT16U      OS_OPT;                               /**< Holds function options                       <16>/32 */

typedef   CPU_INT32U      OS_MON_RES;                           /**< Monitor result flags                                 */

typedef   CPU_INT08U      OS_PRIO;                              /**< Priority of a task                         <8>/16/32 */

typedef   CPU_INT16U      OS_QTY;                               /**< Quantity                                     <16>/32 */

typedef   CPU_INT32U      OS_RATE_HZ;                           /**< Rate in Hertz                                     32 */

#if (CPU_CFG_ADDR_SIZE == CPU_WORD_SIZE_64)                     /**< Task register                           8/16/<32/64> */
typedef   CPU_INT64U      OS_REG;
#else
typedef   CPU_INT32U      OS_REG;
#endif
typedef   CPU_INT08U      OS_REG_ID;                            /**< Index to task register                     <8>/16/32 */

typedef   CPU_INT32U      OS_SEM_CTR;                           /**< Semaphore value                              16/<32> */

typedef   CPU_INT08U      OS_STATE;                             /**< State variable                             <8>/16/32 */

typedef   CPU_INT08U      OS_STATUS;                            /**< Status                                     <8>/16/32 */

typedef   CPU_INT32U      OS_TICK;                              /**< Clock tick counter                           <32>/64 */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         DEPRECATED DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef   CPU_INT16U      OS_MEM_QTY;                           /**< Number of memory blocks                      <16>/32 */
typedef   CPU_INT16U      OS_MEM_SIZE;                          /**< Size in bytes of a memory block              <16>/32 */


/**
*********************************************************************************************************
*********************************************************************************************************
* @}                                           MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of Kernel types module include.                  */
